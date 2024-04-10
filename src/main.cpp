#include <Arduino.h>
#include "BatReader.h"
#include "driver/adc.h"


BatReader reader;

void setup() {
    Serial.begin(9600);

    #if defined(AUDRINO)
        reader.initArduinoReader(A0, 5.44, 4.8); // according to battery to be measured to set max voltage and min voltage
        reader.setDivider(10000, 110000);  // according to battery to be measured
    #endif

    #if defined(ESP32)
        reader.initESP32Reader(ADC1_CHANNEL_0, 5.44, 4.8, ADC_ATTEN_DB_11, ADC_UNIT_1, ADC_WIDTH_BIT_12);
        reader.setDivider(300000, 78000);
    #endif
}

void loop() {
    BatInfo info = reader.readBatInfo();
    Serial.print("Voltage: ");
    Serial.print(info.voltage);
    Serial.print("V, Percent: ");
    Serial.print(info.percent);
    Serial.println("%");
    delay(1000);
}