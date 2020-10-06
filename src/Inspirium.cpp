#include "Inspirium.h"

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

    motorController.begin();
    environmentModule.begin();
    radioModule.begin();
    positionModule.begin();
    lightModule.begin();
    storageModule.begin();
    cameraModule.begin();

    idle();
    
}

// This update is heartbeat for whole library, try to call it as often as you can.
void InspiriumClass::update() {
    motorController.update();
    positionModule.update();
    lightModule.update();
}

// This will put all active modules into idle state.
void InspiriumClass::idle() {

    motorController.idle();
    environmentModule.idle();
    radioModule.idle();
    positionModule.idle();
    lightModule.idle();
    cameraModule.idle();
}

// This will put all modules into sleep and disable 3.3EN. Processor remains active.
void InspiriumClass::sleep() {

    motorController.sleep();
    environmentModule.sleep();
    radioModule.sleep();
    positionModule.sleep();
    lightModule.sleep();
    storageModule.sleep();

    disableSPI();

    digitalWrite(EQP_PIN, LOW);
    digitalWrite(POWER_3V3_PIN, LOW);
    digitalWrite(POWER_5V_PIN, LOW);

}

// This will begin all modules which were sleeping and enable 3.3EN.
void InspiriumClass::wakeUp() {
    //TODO
}

// This will put device into the lowest possible current consuption.
// Passed method will be called as soon as processor will wake up.
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