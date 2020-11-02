#include "PowerModule.h"

#include "api/API.h"

#include "Arduino.h"
#include "SPI.h"

void PowerModule::begin() {

    pinMode(EQP_PIN, OUTPUT);
    pinMode(POWER_3V3_PIN, OUTPUT);
    pinMode(POWER_5V_PIN, OUTPUT);
    pinMode(SOLAR_EN_PIN, OUTPUT);

    disable5V();
    disableEQP();
    disableSolar();
    disable3V3();

}

void PowerModule::update() {

    checkMsg();

}

void PowerModule::enable3V3() {
    digitalWrite(POWER_3V3_PIN, HIGH);
    SPI.begin();
    active3V3 = true;
}

void PowerModule::disable3V3() {
    API.disableSPI();
    digitalWrite(POWER_3V3_PIN, LOW);
    active3V3 = false;
}

void PowerModule::enable5V() {
    digitalWrite(POWER_5V_PIN, HIGH);
    active5V = true;
}

void PowerModule::disable5V() {
    digitalWrite(POWER_5V_PIN, LOW);
    active5V = false;
}

void PowerModule::enableEQP() {
    digitalWrite(EQP_PIN, HIGH);
    activeEQP = true;
}

void PowerModule::disableEQP() {
    digitalWrite(EQP_PIN, LOW);
    activeEQP = false;
}

void PowerModule::enableSolar() {
    digitalWrite(SOLAR_EN_PIN, LOW);
    charging = true;
}

void PowerModule::disableSolar() {
    digitalWrite(SOLAR_EN_PIN, HIGH);
    charging = false;
}

void PowerModule::incomingPacket(IPacket *packet) {

    this->packet = packet;

}

void PowerModule::checkMsg() {

    if(packet == 0) return;

    switch(packet->subFeature) {

        case FT_POWER_CHAR: {

            uint8_t year = packet->buff[3];
            uint8_t month = packet->buff[4];
            uint8_t day = packet->buff[5];
            uint8_t hour = packet->buff[6];
            uint8_t minute = packet->buff[7];
            uint8_t second = packet->buff[8];

            Time t(year, month, day, hour, minute, second);

            API.getRadio().sendAck();
            while(API.getRadio().isSending()) delay(1);

            enableSolar();
            deepSleep(t);
            disableSolar();

            break;
        }
    }

    delete packet;
    packet = 0;

}

// This will put all modules into sleep and disable 3.3EN. Processor remains active.
// You can not begin() Location module, CAM module, NEO Pixel and steps feature from MC
// until you call wakeUpFromSleep().
void PowerModule::sleep() {

    API.getMC().sleep();
    API.getEnviro().sleep();
    API.getRadio().sleep();
    API.getOrientation().sleep();
    API.getLocation().sleep();
    API.getLights().sleep();
    API.getStorage().sleep();
    API.getCam().sleep();

    disable3V3();
    disable5V();
    disableEQP();

}

// This will put device into the lowest possible current consuption.
// It will pause code execution and will continue from call-point after time given.
void PowerModule::deepSleep(const Time &t) {

    sleep();

    RTCZero rtc = API.getEnviro().getRTC();

    rtc.setAlarmDate(t.days, t.months, t.years);
    rtc.setAlarmTime(t.hours, t.mins, t.secs);
    rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
    rtc.attachInterrupt(dummyISR);

    rtc.standbyMode();
    
}

void PowerModule::dummyISR() {}