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

    motorController.idle();
    environmentModule.idle();
    radioModule.idle();
    orientationModule.idle();
    locationModule.idle();
    lightModule.idle();
    cameraModule.idle();

    powerState = ACTIVE;
    
}

// This update is heartbeat for whole library, try to call it as often as you can.
void InspiriumClass::update() {

    processMsg();

    motorController.update();
    locationModule.update();
    lightModule.update();
    cameraModule.update();
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
    
    if(radioPSMode) {
        SPI.begin();
        radioPSMode = false;
    }

    digitalWrite(POWER_3V3_PIN, HIGH);

    powerState = ACTIVE;

    delay(500);

    if(motorController.getState() == SLEEPING) {
        motorController.begin();
        motorController.idle();
    }
    if(environmentModule.getState() == SLEEPING) {
        environmentModule.begin();
        environmentModule.idle();
    }
    if(radioModule.getState() == SLEEPING) {
        radioModule.begin();
        radioModule.idle();
    }
    if(orientationModule.getState() == SLEEPING) {
        orientationModule.begin();
        orientationModule.idle();
    }
    if(locationModule.getState() == SLEEPING) {
        locationModule.begin();
        locationModule.idle();
    }
    if(lightModule.getState() == SLEEPING) {
        lightModule.begin();
        lightModule.idle();
    }
    if(storageModule.getState() == SLEEPING) {
        storageModule.begin();
    }
    if(cameraModule.getState() == SLEEPING) {
        cameraModule.begin();
        cameraModule.idle();
    }
}

// This will put device into the lowest possible current consuption.
// It will pause code execution and will continue from call-point after time given.
// Passed method will be called as soon as processor will wake up.
// If no callback method is passed, Inspi.begin() will be executed.
void InspiriumClass::deepSleep(int hours, int minutes, int seconds, void (*callback)()) {

    radioPSMode = false;

    sleep();

    RTCZero rtc = getEnviro().getRTC();

    rtc.setAlarmTime(rtc.getHours()+hours, rtc.getMinutes()+minutes, rtc.getSeconds()+seconds);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);
    rtc.attachInterrupt(callback);
    rtc.standbyMode();
}

// This will put device into the lowest possible current consuption.
// Radio module  will be still active and receiving.
void InspiriumClass::idleRadioMode() {

    sleep();

    radioPSMode = true;

    SPI.begin();

    radioModule.begin();
    radioModule.listen();

    disableSPI();

}

void InspiriumClass::disableSPI() {

    pinMode(22, INPUT_PULLDOWN);
    pinMode(23, INPUT_PULLDOWN);
    pinMode(24, INPUT_PULLDOWN);

}

void InspiriumClass::incomingMessage(IncomingPacket *packet) {

    incomingPacket = packet;

}

void InspiriumClass::processMsg() {

    if(incomingPacket == 0) return;

    switch(incomingPacket->subFeature) {

        case FT_POWER_DS: {

            int hours = bati(incomingPacket->buff, 3);
            int minutes = bati(incomingPacket->buff, 7);
            int seconds = bati(incomingPacket->buff, 11);

            radioModule.sendAck();

            while(radioModule.isSending()) delay(1);

            deepSleep(hours, minutes, seconds);

            idleRadioMode();
            break;
        }
        case FT_POWER_AWAKE: {

            radioModule.sendAck();

            while(radioModule.isSending()) delay(1);
            
            wakeUpFromSleep();
            cameraModule.wakeUp();
            radioModule.listen();
            break;
        }   
        case FT_POWER_IDLE: {

            radioModule.sendAck();

            while(radioModule.isSending()) delay(1);

            idleRadioMode();
            break;
        }   
    }

    delete incomingPacket;
    incomingPacket = 0;

}

void awake() {

}

InspiriumClass Inspi;