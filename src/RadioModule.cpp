#include "RadioModule.h"
#include "Arduino.h"
#include "libraries/L_LoRa.h"
#include "Inspirium.h"

void RadioModule::begin(void (*callback)(int)) {

    LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_INT_PIN);
    LoRa.begin(868E6);
    LoRa.setSpreadingFactor(9);
    LoRa.onReceive(receiveISR);
    LoRa.onTxDone(txDoneISR);
    listening = false;

    powerState = ACTIVE;

}

void RadioModule::idle() {

    if(powerState != ACTIVE) {
        return;
    }
    
    LoRa.sleep();
    listening = false;
    
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

void RadioModule::send(const uint8_t *buffer, const int &len) {

    if(powerState == IDLE) {
        wakeUp();
    }

    PowerState camLastState = Inspi.getCam().getState();

    if(camLastState == IDLE) {
        Inspi.getCam().wakeUp();
    }

    if(len>MAX_PAYLOAD) {
        return;
    }

    LoRa.beginPacket();
    LoRa.write(buffer, len);
    LoRa.endPacket();

    if(listening) {
        listen();
    }

    if(camLastState == IDLE) {
        Inspi.getCam().idle();
    }

}

void RadioModule::sendFlag(uint8_t flag) {

    if(powerState == IDLE) {
        wakeUp();
    }

    PowerState camLastState = Inspi.getCam().getState();

    if(camLastState == IDLE) {
        Inspi.getCam().wakeUp();
    }

    LoRa.beginPacket();
    LoRa.write(FL_KEY);
    LoRa.write(flag);
    LoRa.endPacket();

    if(listening) {
        listen();
    }

    if(camLastState == IDLE) {
        Inspi.getCam().idle();
    }

}

void RadioModule::sendStatus() {

    if(powerState == IDLE) {
        wakeUp();
    }

    SerialUSB.println("sending status");

    PowerState camLastState = Inspi.getCam().getState();

    SerialUSB.println(Inspi.getCam().getState());

    if(camLastState == IDLE) {
        Inspi.getCam().wakeUp();
    }

    float volts = Inspi.getEnviro().readBatteryVoltage();
    float temp = Inspi.getEnviro().readTemperature();
    int pressure = Inspi.getEnviro().readPressure();
    int humidity = Inspi.getEnviro().readHumidity();
    double lat = Inspi.getLocation().getLat();
    double lng = Inspi.getLocation().getLng();
    double alt = Inspi.getLocation().getAlt();

    LoRa.beginPacket();
    LoRa.write(FL_KEY);
    LoRa.write(FL_STATUS);
    LoRa.write((uint8_t)Inspi.getMC().getState());
    LoRa.write((uint8_t)Inspi.getEnviro().getState());
    LoRa.write((uint8_t)Inspi.getRadio().getState());
    LoRa.write((uint8_t)Inspi.getOrientation().getState());
    LoRa.write((uint8_t)Inspi.getLocation().getState());
    LoRa.write((uint8_t)Inspi.getLights().getState());
    LoRa.write((uint8_t)Inspi.getStorage().getState());
    LoRa.write((uint8_t)camLastState);
    LoRa.write((uint8_t *)(&Inspi.getLights().areLightsOn()), 1);
    LoRa.write((uint8_t *)(&volts), 4);
    LoRa.write((uint8_t *)(&temp), 4);
    LoRa.write((uint8_t *)(&pressure), 4);
    LoRa.write((uint8_t *)(&humidity), 4);
    LoRa.write((uint8_t *)(&lat), 8);
    LoRa.write((uint8_t *)(&lng), 8);
    LoRa.write((uint8_t *)(&alt), 8);
    LoRa.endPacket();

    if(listening) {
        listen();
    }

    if(camLastState == IDLE) {
        Inspi.getCam().idle();
    }

    SerialUSB.println("status is now sending");

}

// This will put radio into continuos receive mode. 
// Note that if message arrive, interrupt will take all processing power and time and may cause delays.
void RadioModule::listen() {

    if(forbidList) {
        return;
    }

    if(powerState == IDLE) {
        wakeUp();
    }

    LoRa.receive();
    listening = true;

}

void RadioModule::stopListening() {

    if(forbidList) {
        return;
    }

    LoRa.idle();
    listening = false;
}

void RadioModule::forbidListening() {
    LoRa.idle();
    forbidList = true;
}

void RadioModule::allowListening() {
    forbidList = false;
    if(listening) {
        LoRa.receive();
    }
}

void RadioModule::onReceive(const int &packetSize) {

    PowerState camLastState = Inspi.getCam().getState();

    if(camLastState == IDLE) {
        Inspi.getCam().wakeUp();
    }

    SerialUSB.println("Incoming packet with "+String(packetSize)+" bytes.");

    uint8_t buff[packetSize];
    LoRa.readBytes(buff, packetSize);

    if(camLastState == IDLE) {
        Inspi.getCam().idle();
    }

    if(buff[0] != FL_KEY){
        SerialUSB.println("Invalid key.");
        return;
    }

    switch (buff[1]) {

        case FL_LIGHTS_ON:
            Inspi.getLights().turnOnLights();
            break;

        case FL_LIGHTS_OFF:
            Inspi.getLights().turnOffLights();
            break;

        case FL_IMG_TAKE:
            Inspi.getCam().captureAndSend();
            break;
    }

}

void RadioModule::onTxDone() {

    SerialUSB.println("TX completed.");

}

void receiveISR(int packetSize) {
    Inspi.getRadio().onReceive(packetSize);
}

void txDoneISR() {
    Inspi.getRadio().onTxDone();
}