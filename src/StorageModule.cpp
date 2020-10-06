#include "StorageModule.h"
#include "Arduino.h"

void StorageModule::begin() {

    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    powerState = ACTIVE;

}

void StorageModule::sleep() {
    
    pinMode(SD_CS_PIN, INPUT_PULLDOWN);

    powerState = SLEEPING;

}