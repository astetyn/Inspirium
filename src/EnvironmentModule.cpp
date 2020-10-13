#include "EnvironmentModule.h"
#include "Arduino.h"

void EnvironmentModule::begin() {

    pinMode(BATT_PIN, INPUT);
    pinMode(ACC_MAG_INT_PIN, INPUT_PULLDOWN);
    pinMode(GYR_INT_PIN, INPUT_PULLDOWN);

    rtc.begin();
    rtc.setDate(0, 0, 0);
    rtc.setTime(0, 0, 0);

    if(!bme280.init()) {
        powerState = UNAVAILABLE;
        return;
    }

    powerState = ACTIVE;

}

void EnvironmentModule::idle() {

    if(powerState != ACTIVE) {
        return;
    }

    powerState = IDLE;

}

void EnvironmentModule::wakeUp() {
    
    powerState = ACTIVE;

}

void EnvironmentModule::sleep() {

    powerState = SLEEPING;

}

// Returns air temperature in Celsius.
float EnvironmentModule::readTemperature() {

    if(powerState == IDLE) {
        wakeUp();
    }

    return bme280.getTemperature();
}

// Returns air pressure in Pascals.
int EnvironmentModule::readPressure() {

    if(powerState == IDLE) {
        wakeUp();
    }

    return bme280.getPressure();
}

// Returns air humidity in percentages.
int EnvironmentModule::readHumidity() {

    if(powerState == IDLE) {
        wakeUp();
    }

    return bme280.getHumidity();
}

// Returns battery voltage in Volts.
float EnvironmentModule::readBatteryVoltage() {

    analogReadResolution(12);
    return analogRead(BATT_PIN)/4095.0F*3.3F*5; // here we assume voltage divider 200/50

}

// Returns UTC time of the day in seconds, 0 = midnight, 43200 = high noon.
int EnvironmentModule::getTime() {

    

}