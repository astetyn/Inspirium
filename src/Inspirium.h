#ifndef INSPIRIUM_H
#define INSPIRIUM_H

#include "MotorController.h"
#include "EnvironmentModule.h"
#include "RadioModule.h"
#include "PositionModule.h"

static const int POWER_3V3_PIN = 15;

class InspiriumClass {

    public:

        InspiriumClass();
        
        void begin();

        void update();

        void deepSleep(int hours, int minutes, int seconds);

        MotorController &getMC() {return motorController;}
        EnvironmentModule &getEnviroMod() {return environmentModule;}
        RadioModule &getRadioMod() {return radioModule;}
        PositionModule &getPositionMod() {return positionModule;}

    private:
        MotorController motorController;
        EnvironmentModule environmentModule;
        RadioModule radioModule;
        PositionModule positionModule;

};

extern InspiriumClass Inspi;

#endif