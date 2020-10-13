#ifndef INSPIRIUM_H
#define INSPIRIUM_H

#include "Arduino.h"
#include "MotorController.h"
#include "EnvironmentModule.h"
#include "RadioModule.h"
#include "OrientationModule.h"
#include "LocationModule.h"
#include "LightModule.h"
#include "StorageModule.h"
#include "CameraModule.h"
#include "PowerState.h"

static const int EQP_PIN = 14;
static const int POWER_3V3_PIN = 15;
static const int POWER_5V_PIN = 16;
static const int SOLAR_EN_PIN = 27;

void awake();

class InspiriumClass {

    public:

        InspiriumClass();
        
        void begin();

        void update();

        void idle();

        void sleep();

        void wakeUpFromSleep();

        void deepSleep(int hours, int minutes, int seconds, void (*callback)() = &awake);

        void enableSolar(){digitalWrite(SOLAR_EN_PIN, LOW);}
        void disableSolar(){digitalWrite(SOLAR_EN_PIN, HIGH);}

        void enable5V(){digitalWrite(POWER_5V_PIN, HIGH);}
        void disable5V(){digitalWrite(POWER_5V_PIN, HIGH);}

        void disableSPI();

        MotorController &getMC() {return motorController;}
        EnvironmentModule &getEnviro() {return environmentModule;}
        RadioModule &getRadio() {return radioModule;}
        OrientationModule &getOrientation() {return orientationModule;}
        LocationModule &getLocation() {return locationModule;}
        LightModule &getLights() {return lightModule;}
        StorageModule &getStorage() {return storageModule;}
        CameraModule &getCam() {return cameraModule;}
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        MotorController motorController;
        EnvironmentModule environmentModule;
        RadioModule radioModule;
        OrientationModule orientationModule;
        LocationModule locationModule;
        LightModule lightModule;
        StorageModule storageModule;
        CameraModule cameraModule;

};

extern InspiriumClass Inspi;

#endif