#include "BatReader.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

// #if defined(ESP32)
//     #include <driver/adc.h>
//     #include "esp_adc_cal.h"
// #endif

BatReader::BatReader() {

}

#if defined(ARDUINO)
void BatReader::initArduinoReader(int _p, float _max, float _min) {
	_max_vcc = _max;
	_min_vcc = _min;
	_pin = _p;
	_vref = 5.0;
}
#endif

#if defined(ESP32)
void BatReader::initESP32Reader(adc1_channel_t _channel, float _max, float _min, adc_atten_t _adc_atten, adc_unit_t _adc_num, adc_bits_width_t _adc_width) {
	_max_vcc = _max;
	_min_vcc = _min;
	_channel = _channel;
	_vref = 1.1;
	_adc_atten_db = _adc_atten;
	_adc_num = _adc_num;
	_adc_width = _adc_width;

	adc1_config_width(_adc_width);
    adc1_config_channel_atten(_channel, _adc_atten_db);

	adcChar = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t cal_mode = esp_adc_cal_characterize(_adc_num, _adc_atten_db, _adc_width, ESP_ADC_CAL_VAL_DEFAULT_VREF, adcChar);
}
#endif

BatInfo BatReader::readBatInfo(void) {
	BatInfo info = {
		.voltage = 0.0,
		.percent = 0.0
	};
	#if defined(ARDUINO)	
		float readed = (float)(analogRead(_pin)*_vref)/1024;
		if(readed > _max_vcc)
			readed = _max_vcc;
		// check divider exists or not, R1 AND R2 should not be empty
		if (!_divider || _R1 == 0 || _R2 == 0) {
			info.voltage = readed;
			info.percent = (readed - _min_vcc) / (_max_vcc - _min_vcc) * 100;
			return info;
		}
		info.voltage = float(_R1 + _R2) / _R2 * readed + _vol_offset;
		if (info.voltage > _max_vcc)
			info.voltage = _max_vcc;
		info.percent = ((info.voltage - _min_vcc) / (_max_vcc - _min_vcc)) * 100;
	#else
		int samplingFrequency = 500; // sampling frequency
		long sum = 0;
		float samples = 0.0;      // samples average

		for (int i = 0; i < samplingFrequency; i++)
		{
			sum += adc1_get_raw(_channel);
			delayMicroseconds(1000);
		}
		samples = sum / (float)samplingFrequency;

		uint32_t voltage = esp_adc_cal_raw_to_voltage(samples, adcChar);

		if (!_divider || _R1 == 0 || _R2 == 0) {
			return info;
		}
		
		float adc_voltage = (float)voltage / 1000.0f + _adc_offset;
		info.voltage = (_R1 + _R2) / _R2 * adc_voltage + _vol_offset;
		if (info.voltage > _max_vcc)
			info.voltage = _max_vcc;
		info.percent = ((info.voltage - _min_vcc) / (_max_vcc - _min_vcc)) * 100;
	#endif
	return info;
}
