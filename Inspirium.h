#ifndef INSPIRIUM_H
#define INSPIRIUM_H

#include "MotorController.h"

class InspiriumClass {

    public:

        InspiriumClass();
        
        void begin();

        // This update is heartbeat for whole library, try to call it as often as you can.
        void update();

        MotorController &getMotorController() {return motorController;}

    private:
        MotorController motorController;

};

extern InspiriumClass Inspirium;

#endif