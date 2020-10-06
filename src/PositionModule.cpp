#include "PositionModule.h"
#include "Arduino.h"
#include "Inspirium.h"
#include "GNSSEncoder.h"

void PositionModule::begin() {

    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIG1_PIN, OUTPUT);
    pinMode(TRIG2_PIN, OUTPUT);
    pinMode(TRIG3_PIN, OUTPUT);
    pinMode(TRIG4_PIN, OUTPUT);
    pinMode(TRIG5_PIN, OUTPUT);
    pinMode(TRIG6_PIN, OUTPUT);

    Serial1.begin(9600);

    powerState = ACTIVE;
}

void PositionModule::update() {
    while(Serial1.available()>0){
        gnss.encode(Serial1.read());
        //SerialUSB.write(Serial1.read());
    }
    /*SerialUSB.println(gnss.satellites.value());
    SerialUSB.println(gnss.location.lat());
    SerialUSB.println(gnss.location.lng());
    SerialUSB.println(gnss.altitude.meters());
    SerialUSB.println(gnss.satellites.value());
    SerialUSB.println(gnss.satellites.value());*/
}

void PositionModule::slowUpdate() {
    //TODO recalculate pitch, roll, yaw
    checkDistances();
}

void PositionModule::idle() {

    if(powerState != ACTIVE) {
        return;
    }

    powerState = IDLE;

}

void PositionModule::wakeUp() {

    powerState = ACTIVE;

}

void PositionModule::sleep() {
    
    // RX+TX PINS NEO
    pinMode(0, INPUT_PULLDOWN);
    pinMode(1, INPUT_PULLDOWN);

    powerState = SLEEPING;

}

void PositionModule::checkDistances() {

    int m2 = measureDistance(TRIG2_PIN);
    int m5 = measureDistance(TRIG5_PIN);

    if(m2==0||m5==0||m2>400||m5>400){
        return;
    }

    if(m2<=10||m5<=10) {
        Inspi.getMC().stopCurrentTask();
        Inspi.getMC().getTaskList().clear();
        TimeTask *tt = new TimeTask();
        tt->motor1 = -30;
        tt->motor2 = -30;
        tt->millis = 500;
        Inspi.getMC().addTask(tt);
        Inspi.getMC().executeNextTask();
        Inspi.getLights().shineMillis(500);
    }

}

int PositionModule::measureDistance(const int &pin) {

    digitalWrite(pin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    int duration = pulseIn(ECHO_PIN, HIGH);
    // Calculating the distance
    int distance = duration*0.034/2;
    return distance;

}