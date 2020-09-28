#ifndef ENVIRONMENT_MODULE_H
#define ENVIRONMENT_MODULE_H

#include "RTCZero.h"

static const int BATT_PIN = 19;

class EnvironmentModule {

    public:

        void begin();
        void sleep();
        float readTemperature();
        float readHumidity();
        float readPressure();
        float readBatteryVoltage();
        int getTime();
        RTCZero &getRTC() {return rtc;}

    private:
        RTCZero rtc;

};

#endif