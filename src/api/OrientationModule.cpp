#include "api/OrientationModule.h"

#include "api/API.h"
#include "api/TaskList.h"

#include "Arduino.h"

void OrientationModule::begin() {

    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIG1_PIN, OUTPUT);
    pinMode(TRIG2_PIN, OUTPUT);
    pinMode(TRIG3_PIN, OUTPUT);
    pinMode(TRIG4_PIN, OUTPUT);
    pinMode(TRIG5_PIN, OUTPUT);
    pinMode(TRIG6_PIN, OUTPUT);

    powerState = PowerState::ACTIVE;
}

void OrientationModule::sleep() {
    

}

void OrientationModule::checkDistances() {

    int m2 = measureDistance(TRIG2_PIN);
    int m5 = measureDistance(TRIG5_PIN);

    if(m2==0||m5==0||m2>400||m5>400){
        return;
    }

    if(m2<=10||m5<=10) {
        API.getMC().stopCurrentTask();
        API.getMC().getTaskList().clear();
        TimeTask *tt = new TimeTask();
        tt->motor1 = -30;
        tt->motor2 = -30;
        tt->millis = 500;
        API.getMC().addTask(tt);
        API.getMC().executeNextTask();
        API.getLights().shineMillis(500);
    }

}

int OrientationModule::measureDistance(const int &pin) {

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