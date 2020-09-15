#include "Inspirium.h"
#include "Arduino.h"

InspiriumClass::InspiriumClass() {
}

void InspiriumClass::begin() {
    motorController.begin();
}

void InspiriumClass::update() {
    motorController.update();
}

InspiriumClass Inspirium;