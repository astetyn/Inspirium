#ifndef ENVIRONMENT_MODULE_H
#define ENVIRONMENT_MODULE_H

#include "api/libraries/L_RTCZero.h"
#include "api/libraries/L_BME280.h"
#include "api/PowerState.h"

const int BATT_PIN = 19;
const int ACC_MAG_INT_PIN = 13;
const int GYR_INT_PIN = 11;

const uint8_t FT_ENVIRO_SYNC = 0x00;

const int MEASURE_INTERVAL_SECS = 600;
const int RECORDS_COUNT = 144;

class EnvironmentModule {

    public:

        void begin();
        void update();
        void sleep();
        float readTemperature();
        int readPressure();
        int readHumidity();
        float readBatteryVoltage();
        uint8_t *getTempsRecs();
        uint8_t *getPressRecs();
        uint8_t *getHumisRecs();
        uint8_t *getVoltsRecs();
        int getRecsLen();
        void checkMsg(const uint8_t subFeature, uint8_t buff[], const int &buffSize);
        bool isTimeSynced() {return timeSynced;}
        RTCZero &getRTC() {return rtc;}
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        RTCZero rtc;
        BME280 bme280;
        bool timeSynced;
        uint8_t voltages[RECORDS_COUNT*4];
        uint8_t temperatures[RECORDS_COUNT*4];
        uint8_t humidities[RECORDS_COUNT*4];
        uint8_t pressures[RECORDS_COUNT*4];
        uint32_t lastMeasure = 0;
        int indexer = 0;
        bool full = false;
        uint8_t *shiftArr(uint8_t arr[], int len, int shift);

};

#endif