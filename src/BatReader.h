/*
 * @Author: lrtxpra 
 * @Date: 2024-04-10 14:40:34 
 * @Last Modified by: lrtxpra
 * @Last Modified time: 2024-04-10 14:47:27
 */

#ifndef __BATREADER_H__
#define __BATREADER_H__

#include "Arduino.h"

#if defined(ARDUINO) && ARDUINO >= 100
    #include "pins_arduino.h"
#endif

#if defined(ESP32)
    #include <driver/adc.h>
    #include "hal/adc_types.h"
    #include "esp_adc_cal.h"
    
#endif

////////////////////////////
// Definitions            //
////////////////////////////
#define BAT_PIN A7
#define BAT_MAX	5.44
#define BAT_MIN	4.8
#define ANA_REF	5
#define SLOPE	100/(BAT_MAX - BAT_MIN)
#define OFFSET	(100*BAT_MIN)/(BAT_MAX - BAT_MIN)

typedef struct BatInfo {
    float voltage;
    float percent;
} BatInfo;

class BatReader
{
public:

    /**
     * if platform is esp32, _p represents the channel number of the ADC
    */
	BatReader();
    #if defined(ARDUINO)
        void initArduinoReader(int _p, float _max, float _min);
    #endif
    #if defined(ESP32)
        void initESP32Reader(adc1_channel_t _channel, float _max, float _min, adc_atten_t _adc_atten, adc_unit_t _adc_num, adc_bits_width_t _adc_width);
    #endif

	BatInfo readBatInfo(void);

    void setADCOffset(float _o) {
        _adc_offset = _o;
    }

    void setVOLOffset(float _o) {
        _vol_offset = _o;
    }

    void setMaxVcc(float _v) {
        _max_vcc = _v;
    }

    void setMinVcc(float _v) {
        _min_vcc = _v;
    }

    void setVref(float _v) {
        _vref = _v;
    }

    void setR2(int _r) {
        _R2 = _r;
    }

    void setDivider(long _r1, long _r2) {
        _divider = true;
        _R1 = _r1;
        _R2 = _r2;
    }

private:
    int _pin;
    float _max_vcc;
    float _min_vcc;
    float _vref;

    float _adc_offset;
    float _vol_offset;

    bool _divider;
    long _R1;
    long _R2;

    // esp32
    #if defined(ESP32)
        adc1_channel_t _adc_channel;
        adc_atten_t _adc_atten_db;
        adc_unit_t _adc_num;
        adc_bits_width_t _adc_width;
        esp_adc_cal_characteristics_t * adcChar;
    #endif

    void _init(void);
};

#endif // BATREADER_H //
