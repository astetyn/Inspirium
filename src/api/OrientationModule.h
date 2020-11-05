#ifndef ORIENTATION_MODULE_H
#define ORIENTATION_MODULE_H

#include "api/PowerState.h"

const int ECHO_PIN = 25;
const int TRIG1_PIN = 26;
const int TRIG2_PIN = 31;
const int TRIG3_PIN = 30;
const int TRIG4_PIN = 17;
const int TRIG5_PIN = 18;
const int TRIG6_PIN = 8;

class OrientationModule {

    public:

        void begin();
        void sleep();
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        void checkDistances();
        int measureDistance(const int &pin);

};

#endif