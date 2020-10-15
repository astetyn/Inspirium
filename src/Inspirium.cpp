#include "Inspirium.h"
#include "Wire.h"
#include "SPI.h"

InspiriumClass::InspiriumClass() {
}

// This must be called as a fist method from this library.
void InspiriumClass::begin() {

    pinMode(EQP_PIN, OUTPUT);
    pinMode(POWER_3V3_PIN, OUTPUT);
    pinMode(POWER_5V_PIN, OUTPUT);
    pinMode(SOLAR_EN_PIN, OUTPUT);
    digitalWrite(EQP_PIN, LOW);
    digitalWrite(POWER_3V3_PIN, HIGH);
    digitalWrite(POWER_5V_PIN, LOW);
    digitalWrite(SOLAR_EN_PIN, HIGH);
    
    delay(1000);

    Wire.begin();
    SPI.begin();

    motorController.begin();
    environmentModule.begin();
    radioModule.begin();
    orientationModule.begin();
    locationModule.begin();
    lightModule.begin();
    storageModule.begin();
    cameraModule.begin();

    idle();

    powerState = ACTIVE;
    
}

// This update is heartbeat for whole library, try to call it as often as you can.
void InspiriumClass::update() {
    motorController.update();
    locationModule.update();
    lightModule.update();
    cameraModule.update();
}

// This will put all active modules into idle state.
void InspiriumClass::idle() {

    motorController.idle();
    environmentModule.idle();
    radioModule.idle();
    orientationModule.idle();
    locationModule.idle();
    lightModule.idle();
    //cameraModule.idle();
}

// This will put all modules into sleep and disable 3.3EN. Processor remains active.
// You can not begin() Location module, CAM module, NEO Pixel and steps feature from MC
// until you call wakeUpFromSleep().
void InspiriumClass::sleep() {

    powerState = SLEEPING;

    motorController.sleep();
    environmentModule.sleep();
    radioModule.sleep();
    orientationModule.sleep();
    locationModule.sleep();
    lightModule.sleep();
    storageModule.sleep();
    cameraModule.sleep();

    disableSPI();

    digitalWrite(EQP_PIN, LOW);
    digitalWrite(POWER_3V3_PIN, LOW);
    digitalWrite(POWER_5V_PIN, LOW);

}

// This will begin all sleeping modules and enable 3.3EN.
void InspiriumClass::wakeUpFromSleep() {
    
    digitalWrite(POWER_3V3_PIN, HIGH);

    powerState = ACTIVE;

    delay(500);

    if(motorController.getState() == SLEEPING) {
        motorController.begin();
    }
    if(environmentModule.getState() == SLEEPING) {
        environmentModule.begin();
    }
    if(radioModule.getState() == SLEEPING) {
        radioModule.begin();
    }
    if(orientationModule.getState() == SLEEPING) {
        orientationModule.begin();
    }
    if(locationModule.getState() == SLEEPING) {
        locationModule.begin();
    }
    if(lightModule.getState() == SLEEPING) {
        lightModule.begin();
    }
    if(storageModule.getState() == SLEEPING) {
        storageModule.begin();
    }
    if(cameraModule.getState() == SLEEPING) {
        cameraModule.begin();
    }
}

// This will put device into the lowest possible current consuption.
// It will pause code execution and will continue from call-point after time given.
// Passed method will be called as soon as processor will wake up.
// If any callback method is passed, Inspi.begin() will be executed.
void InspiriumClass::deepSleep(int hours, int minutes, int seconds, void (*callback)()) {

    sleep();

    RTCZero rtc = getEnviro().getRTC();

    rtc.setAlarmTime(rtc.getHours()+hours, rtc.getMinutes()+minutes, rtc.getSeconds()+seconds);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);
    rtc.attachInterrupt(callback);
    rtc.standbyMode();
}

void InspiriumClass::disableSPI() {

    pinMode(22, INPUT_PULLDOWN);
    pinMode(23, INPUT_PULLDOWN);
    pinMode(24, INPUT_PULLDOWN);

}

void awake() {

    Inspi.begin();

}

InspiriumClass Inspi;