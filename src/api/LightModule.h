#ifndef LIGHT_MODULE_H
#define LIGHT_MODULE_H

#include "api/libraries/L_NeoPixel.h"
#include "api/PowerState.h"
#include "api/IUtils.h"
#include "api/TaskList.h"

#include "stdint.h"

const int MAIN_LED_PIN = 42;
const int NEO_LED_PIN = 9;

const uint8_t FT_LIGHTS_OFF = 0x00;
const uint8_t FT_LIGHTS_ON = 0x01;
const uint8_t FT_LIGHTS_RGB = 0x02;
const uint8_t FT_LIGHTS_PARTY = 0x03;

class LightModule {

    public:

        void begin();
        void update();
        void sleep();
        void shineMillis(const int &ms);
        void turnOnLights();
        void turnOffLights();
        void incomingPacket(IPacket *packet);
        bool &areLightsOn() {return lightsOn;}
        void showColor(uint8_t r, uint8_t g, uint8_t b);
        void setBrightness(uint8_t level);
        PowerState &getState(){return powerState;}
        EventList<void (*)()> &getPartyEndEvent() {return partyEndEvent;}

    private:
        PowerState powerState;
        Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEO_LED_PIN, NEO_GRB + NEO_KHZ800);
        void checkMsg();
        int stopMillis = -1;
        int partyMillis = -1;
        int partyStop = 0;
        bool lightsOn = false;
        bool party = false;
        IPacket *packet;
        EventList<void (*)()> partyEndEvent;
        
};

#endif