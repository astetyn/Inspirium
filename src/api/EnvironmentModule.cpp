#include "api/EnvironmentModule.h"

#include "api/IUtils.h"
#include "api/API.h"

#include "Arduino.h"

void EnvironmentModule::begin() {

    pinMode(BATT_PIN, INPUT);
    pinMode(ACC_MAG_INT_PIN, INPUT_PULLDOWN);
    pinMode(GYR_INT_PIN, INPUT_PULLDOWN);

    if(!bme280.init()) {
        powerState = PowerState::UNAVAILABLE;
        return;
    }

    powerState = PowerState::ACTIVE;

}

void EnvironmentModule::update() {
    
}

void EnvironmentModule::sleep() {

    powerState = PowerState::SLEEPING;

}

// Returns air temperature in Celsius.
float EnvironmentModule::readTemperature() {

    if(powerState == PowerState::SLEEPING) return -1;

    return bme280.getTemperature();
}

// Returns air pressure in Pascals.
int EnvironmentModule::readPressure() {

    if(powerState == PowerState::SLEEPING) return -1;

    return bme280.getPressure();
}

// Returns air humidity in percentages.
int EnvironmentModule::readHumidity() {

    if(powerState == PowerState::SLEEPING) return -1;

    return bme280.getHumidity();
}

// Returns battery voltage in Volts.
float EnvironmentModule::readBatteryVoltage() {

    analogReadResolution(12);
    return analogRead(BATT_PIN)/4095.0F*3.3F*5; // here we assume voltage divider 200/50

}

void EnvironmentModule::checkMsg(const uint8_t subFeature, uint8_t buff[], const int &buffSize) {

    if(subFeature == FT_ENVIRO_SYNC) {

        uint8_t year = buff[3];
        uint8_t month = buff[4];
        uint8_t day = buff[5];
        uint8_t hour = buff[6];
        uint8_t minute = buff[7];
        uint8_t second = buff[8];

        rtc.setDate(day, month, year);
        rtc.setTime(hour, minute, second);

        API.getRadio().sendAck();

        timeSynced = true;

    }
}