#include "EnvironmentModule.h"
#include "Arduino.h"
#include "RTCZero.h"

void EnvironmentModule::begin() {

    pinMode(BATT_PIN, INPUT);

    rtc.begin();
    rtc.setDate(0, 0, 0);
    rtc.setTime(0, 0, 0);

}

void EnvironmentModule::sleep() {

}

// Returns air temperature in Celsius.
float EnvironmentModule::readTemperature() {

}

// Returns air humidity in percentages.
float EnvironmentModule::readHumidity() {
    
}

// Returns air pressure in Pascals.
float EnvironmentModule::readPressure() {
    
}

// Returns battery voltage in Volts.
float EnvironmentModule::readBatteryVoltage() {

    analogReadResolution(12);
    return analogRead(BATT_PIN)/4095.0F*3.3F*5; // here we assume voltage divider 200/50

}

// Returns UTC time of the day in seconds, 0 = midnight, 43200 = high noon.
int EnvironmentModule::getTime() {

}