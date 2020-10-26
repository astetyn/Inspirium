#ifndef LIGHT_MODULE_H
#define LIGHT_MODULE_H

#include "libraries/L_NeoPixel.h"
#include "PowerState.h"
#include "stdint.h"

static const int MAIN_LED_PIN = 42;
static const int NEO_LED_PIN = 9;

const uint8_t FT_LIGHTS_OFF = 0x00;
const uint8_t FT_LIGHTS_ON = 0x01;

class LightModule {

    public:

        void begin();
        void update();
        void idle();
        void sleep();
        void shineMillis(const int &ms);
        void turnOnLights();
        void turnOffLights();
        void processMsg(const uint8_t flag, uint8_t buff[], const int &buffSize);
        bool &areLightsOn() {return lightsOn;}
        void showColor(uint8_t r, uint8_t g, uint8_t b);
        void setBrightness(uint8_t level);
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEO_LED_PIN, NEO_GRB + NEO_KHZ800);
        int stopMillis = -1;
        bool lightsOn = false;

        
};

#endif