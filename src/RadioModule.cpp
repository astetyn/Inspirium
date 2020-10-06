#include "RadioModule.h"
#include "Inspirium.h"

void RadioModule::begin() {

    LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_INT_PIN);
    LoRa.begin(868E6);
    LoRa.setSpreadingFactor(9);
    LoRa.onReceive(receiveISR);

    powerState = ACTIVE;

}

void RadioModule::idle() {

    if(powerState != ACTIVE) {
        return;
    }
    
    LoRa.sleep();
    
    powerState = IDLE;
}

void RadioModule::wakeUp() {
    
    powerState = ACTIVE;

}

void RadioModule::sleep() {

    idle();
    pinMode(LORA_CS_PIN, INPUT_PULLDOWN);

    powerState = SLEEPING;

}

void RadioModule::sendByte(const uint8_t &val) {
    LoRa.beginPacket();
    LoRa.write(val);
    LoRa.endPacket();
}

void RadioModule::sendBytes(const uint8_t *buffer, const int &len) {

    if(len>MAX_PAYLOAD) {
        return;
    }

    LoRa.beginPacket();
    LoRa.write(buffer, len);
    LoRa.endPacket();

}

void RadioModule::sendString(String &s) {

    LoRa.beginPacket();
    LoRa.print(s);
    LoRa.endPacket();

}

void RadioModule::sendStatus() {

    float volts = Inspi.getEnviro().readBatteryVoltage();
    float temp = Inspi.getEnviro().readTemperature();
    int pressure = Inspi.getEnviro().readPressure();
    int humidity = Inspi.getEnviro().readHumidity();

    LoRa.beginPacket();
    LoRa.write(FL_PASSWORD);
    LoRa.write(FL_STATUS);
    LoRa.write((uint8_t *)(&volts), sizeof(volts));
    LoRa.write((uint8_t *)(&temp), sizeof(temp));
    LoRa.write((uint8_t *)(&pressure), sizeof(pressure));
    LoRa.write((uint8_t *)(&humidity), sizeof(humidity));
    LoRa.endPacket();

}

// This will put radio into continuos receive mode. 
// Note that if message arrive, interrupt will take all processing power and time and may cause delays.
void RadioModule::listen() {
    LoRa.receive();
}

void RadioModule::stopListening() {
    LoRa.idle();
}

void RadioModule::onReceive(const int &packetSize) {

    //DEBUG TEST

    SerialUSB.println("packet size: "+packetSize);

    if(packetSize!=8) {
        return;
    }

    SerialUSB.println("received");

    uint8_t buff[packetSize];
    LoRa.readBytes(buff, packetSize);
    int i1 = *((int *)&buff[4]);
    int i2 = *((int *)&buff[0]);

    int mot1 = i1+i2;
    int mot2 = i1-i2;
    SerialUSB.println(mot1);
    SerialUSB.println(mot2);
    Inspi.getMC().move(mot1, mot2);

    listen();

    //DEBUG TEST

}

void receiveISR(int packetSize) {
    Inspi.getRadio().onReceive(packetSize);
}