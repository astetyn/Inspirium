#include "api/StorageModule.h"

#include "Arduino.h"

void StorageModule::begin() {

    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    powerState = PowerState::ACTIVE;

}

void StorageModule::sleep() {
    
    pinMode(SD_CS_PIN, INPUT_PULLDOWN);

    powerState = PowerState::SLEEPING;

}