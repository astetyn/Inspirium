#include "Inspirium.h"

InspiriumClass::InspiriumClass() {
}

// This must be called as a fist method from this library.
void InspiriumClass::begin() {

    pinMode(POWER_3V3_PIN, OUTPUT);
    digitalWrite(POWER_3V3_PIN, HIGH);
    delay(1000);

    motorController.begin();
    environmentModule.begin();
    radioModule.begin();
    positionModule.begin();

}
// This update is heartbeat for whole library, try to call it as often as you can.
void InspiriumClass::update() {
    motorController.update();
    positionModule.update();
}

// This will put device into the lowest possible current consuption.
void InspiriumClass::deepSleep(int hours, int minutes, int seconds) {
    motorController.sleep();
    environmentModule.sleep();
    radioModule.sleep();
    positionModule.sleep();

    // SPI pins
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
    digitalWrite(22, LOW);
    digitalWrite(23, LOW);
    digitalWrite(24, LOW);

    digitalWrite(POWER_3V3_PIN, LOW);
    
    //Debug Zone
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);
    pinMode(28, OUTPUT);
    digitalWrite(28, LOW);
    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
    //Debug zone


    RTCZero rtc = getEnviroMod().getRTC();

    rtc.setAlarmTime(rtc.getHours()+hours, rtc.getMinutes()+minutes, rtc.getSeconds()+seconds);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);
    rtc.standbyMode();
}

InspiriumClass Inspi;