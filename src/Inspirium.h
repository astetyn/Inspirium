#ifndef INSPIRIUM_H
#define INSPIRIUM_H

#include "MotorController.h"
#include "EnvironmentModule.h"
#include "RadioModule.h"
#include "PositionModule.h"
#include "LightModule.h"
#include "StorageModule.h"
#include "CameraModule.h"

static const int EQP_PIN = 14;
static const int POWER_3V3_PIN = 15;
static const int POWER_5V_PIN = 16;
static const int SOLAR_EN_PIN = 27;

class InspiriumClass {

    public:

        InspiriumClass();
        
        void begin();

        void update();

        void idle();

        void sleep();

        void wakeUp();

        void deepSleep(int hours, int minutes, int seconds, void (*callback)() = &awake);

        void enableSolar(){digitalWrite(SOLAR_EN_PIN, LOW);}
        void disableSolar(){digitalWrite(SOLAR_EN_PIN, HIGH);}

        void enable5V(){digitalWrite(POWER_5V_PIN, HIGH);}
        void disable5V(){digitalWrite(POWER_5V_PIN, HIGH);}

        void disableSPI();

        MotorController &getMC() {return motorController;}
        EnvironmentModule &getEnviro() {return environmentModule;}
        RadioModule &getRadio() {return radioModule;}
        PositionModule &getPosition() {return positionModule;}
        LightModule &getLights() {return lightModule;}
        StorageModule &getStorage() {return storageModule;}
        CameraModule &getCam() {return cameraModule;}

    private:
        MotorController motorController;
        EnvironmentModule environmentModule;
        RadioModule radioModule;
        PositionModule positionModule;
        LightModule lightModule;
        StorageModule storageModule;
        CameraModule cameraModule;

};

void awake();

extern InspiriumClass Inspi;

#endif