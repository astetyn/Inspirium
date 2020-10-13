#include "LightModule.h"
#include "Arduino.h"

void LightModule::begin() {

    pinMode(MAIN_LED_PIN, OUTPUT);
    pinMode(NEO_LED_PIN, OUTPUT);
    digitalWrite(MAIN_LED_PIN, LOW);

    strip.begin();
    showColor(0, 0, 0);

    powerState = ACTIVE;

}

void LightModule::update() {

    if(stopMillis!=-1&&millis()>stopMillis) {
        turnOffLights();
        stopMillis = -1;
    }

}

void LightModule::idle() {

    if(powerState != ACTIVE) {
        return;
    }

    showColor(0, 0, 0);
    turnOffLights();
    powerState = IDLE;

}

void LightModule::sleep() {

    showColor(0, 0, 0);
    digitalWrite(NEO_LED_PIN, LOW);
    turnOffLights();

    powerState = SLEEPING;
    
}

void LightModule::shineMillis(const int &ms) {

    turnOnLights();
    stopMillis = millis()+ms;

}

// Turns the main lights on.
void LightModule::turnOnLights() {
    digitalWrite(MAIN_LED_PIN, HIGH);
    lightsOn = true;
    powerState = ACTIVE;
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
    powerState = ACTIVE;
}

// Sets brightness of the Neo pixel.
void LightModule::setBrightness(uint8_t level) {
    strip.setBrightness(level);
    strip.show();
    powerState = ACTIVE;
}