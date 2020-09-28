#include "PositionModule.h"
#include "Arduino.h"

void PositionModule::begin() {
    Serial.begin(9600);
}

void PositionModule::update() {
    while(Serial.available()>0){
        //feed parser
    }
}

void PositionModule::slowUpdate() {
    //TODO recalculate pitch, roll, yaw
}

void PositionModule::sleep() {
    
    // RX+TX PINS NEO
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);

}