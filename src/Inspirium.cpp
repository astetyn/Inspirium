#include "Inspirium.h"

#include "api/API.h"
#include "SPI.h"

void InspiriumClass::begin() {

    API.begin();

    switchToManual();

}

void InspiriumClass::update() {

    API.update();

    if(mode == Mode::MANUAL) {
        manualMode.update();
    }else {
        autoMode.update();
    }

}

void InspiriumClass::switchToManual() {

    if(mode == Mode::AUTO) {
        autoMode.stop();
    }
    manualMode.start();
    mode = Mode::MANUAL;

}

void InspiriumClass::switchToAuto() {

    if(mode == Mode::MANUAL) {
        manualMode.stop();
    }
    autoMode.start();
    mode = Mode::AUTO;

}

InspiriumClass Inspi;