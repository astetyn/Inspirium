#ifndef ENVIRONMENT_MODULE_H
#define ENVIRONMENT_MODULE_H

#include "libraries/L_RTCZero.h"
#include "libraries/L_BME280.h"
#include "PowerState.h"

static const int BATT_PIN = 19;
static const int ACC_MAG_INT_PIN = 13;
static const int GYR_INT_PIN = 11;

class EnvironmentModule {

    public:

        void begin();
        void idle();
        void wakeUp();
        void sleep();
        float readTemperature();
        int readPressure();
        int readHumidity();
        float readBatteryVoltage();
        int getTime();
        RTCZero &getRTC() {return rtc;}
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        RTCZero rtc;
        BME280 bme280;

};

#endif