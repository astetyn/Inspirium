#ifndef LIGHT_MODULE_H
#define LIGHT_MODULE_H

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "PowerState.h"

static const int MAIN_LED_PIN = 42;
static const int NEO_LED_PIN = 9;

class LightModule {

    public:

        void begin();
        void update();
        void idle();
        void sleep();
        void shineMillis(const int &ms);
        void turnOnLights();
        void turnOffLights();
        void showColor(uint8_t r, uint8_t g, uint8_t b);
        void setBrightness(uint8_t level);
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEO_LED_PIN, NEO_GRB + NEO_KHZ800);
        int stopMillis = -1;

        
};

#endif