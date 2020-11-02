#include "api/API.h"

#include "Wire.h"
#include "SPI.h"

#include "Arduino.h"

// This must be called as a fist method from this library.
void InspiAPIClass::begin() {
    
    powerModule.begin();
    powerModule.enable3V3();

    delay(500);

    Wire.begin();

    API.getLights().begin();
    API.getEnviro().begin();
    API.getMC().begin();
    API.getLocation().begin();
    API.getStorage().begin();
    API.getOrientation().begin();
    API.getCam().begin();
    API.getRadio().begin();

    environmentModule.getRTC().begin();
    environmentModule.getRTC().setDate(0, 0, 0);
    environmentModule.getRTC().setTime(0, 0, 0);

    API.getPower().sleep();
    
}

// This update is heartbeat for whole library, try to call it as often as you can.
void InspiAPIClass::update() {

    motorController.update();
    locationModule.update();
    lightModule.update();
    cameraModule.update();
    environmentModule.update();
    powerModule.update();

}

void InspiAPIClass::disableSPI() {

    pinMode(22, INPUT_PULLDOWN);
    pinMode(23, INPUT_PULLDOWN);
    pinMode(24, INPUT_PULLDOWN);

}

InspiAPIClass API;