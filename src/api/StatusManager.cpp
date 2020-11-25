#include "StatusManager.h"

#include "api/API.h"

void StatusManager::update() {
    checkMsg();
}

void StatusManager::sendStatusMain() {

    float volts = API.getEnviro().readBatteryVoltage();
    float temp = API.getEnviro().readTemperature();
    double lat = API.getLocation().getLat();
    double lng = API.getLocation().getLng();

    uint8_t buff[25];
    buff[0] = API.getLights().areLightsOn();
    ftba(volts, buff, 1);
    ftba(temp, buff, 5);
    dtba(lat, buff, 9);
    dtba(lng, buff, 17);

    API.getRadio().send(ACK_END, buff, 25);

}

void StatusManager::sendStatusWeather() {

    int len = API.getEnviro().getRecsLen() * 4; // floats and ints have 4 bytes

    uint8_t *temps = API.getEnviro().getTempsRecs();
    uint8_t *press = API.getEnviro().getPressRecs();
    uint8_t *humis = API.getEnviro().getHumisRecs();

    int maxPacketS = 200;

    int c = len / maxPacketS;

    for(int i = 0; i < c; i++) {
        API.getRadio().send(ACK_CONTINUE, temps + i*maxPacketS, maxPacketS);
        while(API.getRadio().isSending()) delay(1);
    }
    API.getRadio().send(ACK_CONTINUE, temps + c*maxPacketS, len - c*maxPacketS);
    while(API.getRadio().isSending()) delay(1);
    
    for(int i = 0; i < c; i++) {
        API.getRadio().send(ACK_CONTINUE, press + i*maxPacketS, maxPacketS);
        while(API.getRadio().isSending()) delay(1);
    }
    API.getRadio().send(ACK_CONTINUE, press + c*maxPacketS, len - c*maxPacketS);
    while(API.getRadio().isSending()) delay(1);

    for(int i = 0; i < c; i++) {
        API.getRadio().send(ACK_CONTINUE, humis + i*maxPacketS, maxPacketS);
        while(API.getRadio().isSending()) delay(1);
    }
    API.getRadio().send(ACK_END, humis + c*maxPacketS, len - c*maxPacketS);

    delete[] temps;
    delete[] press;
    delete[] humis;

}

void StatusManager::sendStatusBatt() {

    int len = API.getEnviro().getRecsLen() * 4; // floats and ints have 4 bytes

    uint8_t *voltages = API.getEnviro().getVoltsRecs();

    API.getRadio().send(ACK_END, voltages, len);

    delete[] voltages;

}

void StatusManager::incomingPacket(IPacket *packet) {
    this->packet = packet;
}

void StatusManager::checkMsg() {

    if(packet == 0) return;

    switch(packet->subFeature) {

        case FT_STATUS_MAIN:
            sendStatusMain();
            break;
        case FT_STATUS_WEATHER:
            sendStatusWeather();
            break;
        case FT_STATUS_BATT:
            sendStatusBatt();
            break;
    }

    delete packet;
    packet = 0;

}