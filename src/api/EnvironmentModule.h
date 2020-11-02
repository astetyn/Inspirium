#ifndef ENVIRONMENT_MODULE_H
#define ENVIRONMENT_MODULE_H

#include "api/libraries/L_RTCZero.h"
#include "api/libraries/L_BME280.h"
#include "api/PowerState.h"

static const int BATT_PIN = 19;
static const int ACC_MAG_INT_PIN = 13;
static const int GYR_INT_PIN = 11;

static const uint8_t FT_ENVIRO_SYNC = 0x00;

class EnvironmentModule {

    public:

        void begin();
        void update();
        void sleep();
        float readTemperature();
        int readPressure();
        int readHumidity();
        float readBatteryVoltage();
        void checkMsg(const uint8_t subFeature, uint8_t buff[], const int &buffSize);
        bool isTimeSynced() {return timeSynced;}
        RTCZero &getRTC() {return rtc;}
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        RTCZero rtc;
        BME280 bme280;
        bool timeSynced;

};

#endif