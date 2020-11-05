#ifndef INSPIRIUM_API_H
#define INSPIRIUM_API_H

#include "api/MotorController.h"
#include "api/EnvironmentModule.h"
#include "api/RadioModule.h"
#include "api/OrientationModule.h"
#include "api/LocationModule.h"
#include "api/LightModule.h"
#include "api/StorageModule.h"
#include "api/CameraModule.h"
#include "api/PowerModule.h"
#include "api/IUtils.h"
#include "api/StatusManager.h"

class InspiAPIClass {

    public:
        
        void begin();

        void update();

        void disableSPI();

        MotorController &getMC() {return motorController;}
        EnvironmentModule &getEnviro() {return environmentModule;}
        RadioModule &getRadio() {return radioModule;}
        OrientationModule &getOrientation() {return orientationModule;}
        LocationModule &getLocation() {return locationModule;}
        LightModule &getLights() {return lightModule;}
        StorageModule &getStorage() {return storageModule;}
        CameraModule &getCam() {return cameraModule;}
        PowerModule &getPower() {return powerModule;}
        StatusManager &getStatMng() {return statusManager;}

    private:
        MotorController motorController;
        EnvironmentModule environmentModule;
        RadioModule radioModule;
        OrientationModule orientationModule;
        LocationModule locationModule;
        LightModule lightModule;
        StorageModule storageModule;
        CameraModule cameraModule;
        PowerModule powerModule;
        StatusManager statusManager;

};

extern InspiAPIClass API;

#endif