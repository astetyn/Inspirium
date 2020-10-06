#ifndef POSITION_MODULE_H
#define POSITION_MODULE_H

#include "GNSSEncoder.h"
#include "PowerState.h"

static const int ECHO_PIN = 25;
static const int TRIG1_PIN = 26;
static const int TRIG2_PIN = 31;
static const int TRIG3_PIN = 30;
static const int TRIG4_PIN = 17;
static const int TRIG5_PIN = 18;
static const int TRIG6_PIN = 8;

class PositionModule {

    public:

        void begin();
        void update();
        void slowUpdate();
        void idle();
        void wakeUp();
        void sleep();
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        void checkDistances();
        int measureDistance(const int &pin);
        TinyGPSPlus gnss;

};

#endif