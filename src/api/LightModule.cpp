#include "api/LightModule.h"

#include "api/API.h"

#include "stdlib.h"

void LightModule::begin() {

    pinMode(MAIN_LED_PIN, OUTPUT);
    pinMode(NEO_LED_PIN, OUTPUT);
    digitalWrite(MAIN_LED_PIN, LOW);

    strip.begin();
    showColor(0, 0, 0);

    packet = 0;

    powerState = PowerState::ACTIVE;

}

void LightModule::update() {

    checkMsg();

    if(party && partyMillis + 200 < millis()) {
        showColor(rand()%255, rand()%255, rand()%255);
        
        partyStop -= millis() - partyMillis;
        if(partyStop <= 0) {

            for(int i = 0; i < partyEndEvent.size(); i++) {
                partyEndEvent.getAt(i)();
            }
            party = false;
        }

        partyMillis = millis();
    }

    if(stopMillis != -1 && millis() > stopMillis) {
        turnOffLights();
        stopMillis = -1;
    }

}

void LightModule::sleep() {

    showColor(0, 0, 0);
    digitalWrite(NEO_LED_PIN, LOW);
    turnOffLights();

    powerState = PowerState::SLEEPING;
    
}

void LightModule::shineMillis(const int &ms) {

    turnOnLights();
    stopMillis = millis()+ms;

}

// Turns the main lights on.
void LightModule::turnOnLights() {
    digitalWrite(MAIN_LED_PIN, HIGH);
    lightsOn = true;
    powerState = PowerState::ACTIVE;
}

// Turns the main lights off.
void LightModule::turnOffLights() {
    digitalWrite(MAIN_LED_PIN, LOW);
    lightsOn = false;
}

// Sets the color of the Neo pixel and show it.
void LightModule::showColor(uint8_t r, uint8_t g, uint8_t b) {
    strip.setPixelColor(0, r, g, b);
    strip.show();
    powerState = PowerState::ACTIVE;
}

// Sets brightness of the Neo pixel.
void LightModule::setBrightness(uint8_t level) {
    strip.setBrightness(level);
    strip.show();
    powerState = PowerState::ACTIVE;
}

void LightModule::incomingPacket(IPacket *packet) {

    this->packet = packet;

}

void LightModule::checkMsg() {

    if(packet == 0) return;

    switch(packet->subFeature) {

        case FT_LIGHTS_ON:
            turnOnLights();
            API.getRadio().sendAck();
            break;

        case FT_LIGHTS_OFF:
            turnOffLights();
            API.getRadio().sendAck();
            break;
        
        case FT_LIGHTS_RGB: {
            API.getPower().enable3V3();
            delay(10);
            API.getRadio().begin();
            API.getRadio().listen();
            API.getRadio().sendAck();
            uint8_t r = packet->buff[3];
            uint8_t g = packet->buff[4];
            uint8_t b = packet->buff[5];
            showColor(r, g, b);
            break;
        }
        case FT_LIGHTS_PARTY:
            party = true;
            partyMillis = millis();
            partyStop = 10000;
            API.getPower().enable3V3();
            delay(10);
            API.getRadio().begin();
            API.getRadio().sendAck();
            break;

    }

    delete packet;
    packet = 0;

}