#ifndef POWER_MODULE_H
#define POWER_MODULE_H

#include "api/IUtils.h"

const int EQP_PIN = 14;
const int POWER_3V3_PIN = 15;
const int POWER_5V_PIN = 16;
const int SOLAR_EN_PIN = 27;

const uint8_t FT_POWER_CHAR = 0x00;

class PowerModule {

    public:
        
        void begin();

        void update();

        void enable3V3();
        void disable3V3();

        void enable5V();
        void disable5V();

        void enableEQP();
        void disableEQP();

        void enableSolar();
        void disableSolar();

        void incomingPacket(IPacket *packet);
        void checkMsg();

        bool is3V3Enabled() {return active3V3;}
        bool is5VEnabled() {return active5V;}
        bool isEQPEnabled() {return activeEQP;}
        bool isCharging() {return charging;}

        void sleep();

        void deepSleep(const Time &t);

    private:

        bool active3V3;
        bool active5V;
        bool activeEQP;
        bool charging;
        IPacket *packet;
        static void dummyISR();

};

#endif