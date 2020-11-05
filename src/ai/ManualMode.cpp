#include "ManualMode.h"

#include "api/API.h"
#include "SPI.h"

int ISRFlag;

bool ManualMode::synced = false;

void ManualMode::start() {

    ISRFlag = 0;

    API.getCam().getImgCompleteEvent().registerCallback(onImageCompleted);
    API.getRadio().getReceiveEvent().registerCallback(onPreReceive);
    API.getLights().getPartyEndEvent().registerCallback(onPartyEnd);
    API.getPower().getCharDoneEvent().registerCallback(onChargingDone);

    SPI.begin();
    API.getEnviro().begin();
    API.getRadio().begin();
    API.getRadio().listen();

}

void ManualMode::stop() {

    API.getCam().getImgCompleteEvent().unregisterCallback(onImageCompleted);
    API.getRadio().getReceiveEvent().unregisterCallback(onPreReceive);
    API.getLights().getPartyEndEvent().unregisterCallback(onPartyEnd);

    API.getPower().sleep();

}

void ManualMode::update() {

    if(ISRFlag == 1) {

        ISRFlag = 0;
        syncExpired();

    }
}

void ManualMode::onImageCompleted() {

    API.getPower().disable3V3();
    SPI.begin();
    API.getRadio().begin();
    API.getRadio().listen();
    API.disableSPI();

    API.getEnviro().begin();

}

void ManualMode::onPreReceive(IPacket *packet) {

    if(!ManualMode::synced && (packet->buff[1] != FT_ENVIRO || packet->buff[2] != FT_ENVIRO_SYNC)) {
        packet->cancelled = true;
        return;
    }

    extendTimeout(0, 0, LISTEN_SYNC_TIME);
    ManualMode::synced = true;
    

}

void ManualMode::onPartyEnd() {

    extendTimeout(0, 0, LISTEN_SYNC_TIME);
    API.getPower().sleep();
    
    SPI.begin();
    API.getRadio().begin();
    API.getRadio().listen();
    API.disableSPI();

    API.getEnviro().begin();

}

void ManualMode::onChargingDone() {

    extendTimeout(0, 0, LISTEN_SYNC_TIME);

    SPI.begin();
    API.getRadio().begin();
    API.getRadio().listen();
    API.disableSPI();

    API.getEnviro().begin();

}

void ManualMode::extendTimeout(int hours, int minutes, int seconds) {

    RTCZero rtc = API.getEnviro().getRTC();

    Time t = Time(rtc);
    t.add(hours, minutes, seconds);
    rtc.setAlarmDate(t.days, t.months, t.years);
    rtc.setAlarmTime(t.hours, t.mins, t.secs);
    rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
    rtc.attachInterrupt(syncExpiredISR);

}

void ManualMode::syncExpired() {

    ManualMode::synced = false;

    RTCZero rtc = API.getEnviro().getRTC();

    Time t = Time(rtc);
    t.add(0, 0, SLEEP_TIME);

    API.getPower().deepSleep(t);

    sleepEnd();

}

void ManualMode::sleepEnd() {

    extendTimeout(0, 0, LISTEN_WINDOW_TIME);
    
    SPI.begin();
    API.getEnviro().begin();
    API.getRadio().begin();
    API.getRadio().listen();
    API.disableSPI();

}

void ManualMode::syncExpiredISR() {

    ISRFlag = 1;

}