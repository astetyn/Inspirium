#include "RadioModule.h"
#include "Arduino.h"
#include "libraries/L_LoRa.h"
#include "Inspirium.h"
#include "IUtils.h"

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

// Sends data with key, flag and buffer. Note that this is async and you can check if data has been
// sent with isSending().
void RadioModule::send(const uint8_t flag, const uint8_t *buffer, const int &len) {

    if(sending) return;

    if(len>MAX_PAYLOAD) return;

    if(powerState == UNAVAILABLE) return;

    if(powerState == IDLE) wakeUp();

    if(Inspi.getCam().getState() == IDLE) return;

    sending = true;

    LoRa.beginPacket();
    LoRa.write(FL_KEY);
    LoRa.write(flag);
    LoRa.write(buffer, len);
    LoRa.endPacket(true);

}

void RadioModule::sendStatus() {

    float volts = Inspi.getEnviro().readBatteryVoltage();
    float temp = Inspi.getEnviro().readTemperature();
    int pressure = Inspi.getEnviro().readPressure();
    int humidity = Inspi.getEnviro().readHumidity();
    double lat = Inspi.getLocation().getLat();
    double lng = Inspi.getLocation().getLng();
    double alt = Inspi.getLocation().getAlt();

    uint8_t buff[49];
    buff[0] = Inspi.getMC().getState();
    buff[1] = Inspi.getEnviro().getState();
    buff[2] = Inspi.getRadio().getState();
    buff[3] = Inspi.getOrientation().getState();
    buff[4] = Inspi.getLocation().getState();
    buff[5] = Inspi.getLights().getState();
    buff[6] = Inspi.getStorage().getState();
    buff[7] = Inspi.getCam().getState();
    buff[8] = Inspi.getLights().areLightsOn();
    ftba(volts, buff, 9);
    ftba(temp, buff, 13);
    itba(pressure, buff, 17);
    itba(humidity, buff, 21);
    dtba(lat, buff, 25);
    dtba(lng, buff, 33);
    dtba(alt, buff, 41);

    send(FL_STATUS, buff, 49);

}

// This will put radio into continuos receive mode. 
// Note that if message arrive, interrupt will take all processing power and time and may cause delays.
void RadioModule::listen() {

    if(forbidList) return;

    if(sending) return;

    if(Inspi.getCam().getState() == IDLE) return;

    if(powerState == IDLE) wakeUp();    

    LoRa.receive();
    listening = true;

}

void RadioModule::stopListening() {

    if(forbidList) return;

    LoRa.idle();
    listening = false;
}

void RadioModule::forbidListening() {
    LoRa.idle();
    forbidList = true;
}

void RadioModule::allowListening() {
    forbidList = false;
    if(listening) LoRa.receive();
}

void RadioModule::onReceive(const int &packetSize) {

    if(Inspi.getCam().getState() == IDLE) return;

    uint8_t buff[packetSize];
    LoRa.readBytes(buff, packetSize);


    if(buff[0] != FL_KEY){
        return;
    }

    switch (buff[1]) {

        case FL_LIGHTS_ON:
            Inspi.getLights().turnOnLights();
            send(FL_ACK, 0, 0);
            break;

        case FL_LIGHTS_OFF:
            Inspi.getLights().turnOffLights();
            send(FL_ACK, 0, 0);
            break;

        case FL_IMG:
            Inspi.getCam().captureAndSend();
            send(FL_ACK, 0, 0);
            break;

        case FL_STATUS:
            sendStatus();
            break;
    }

}

bool RadioModule::isSending() {
    return sending;
}

void RadioModule::onTxDone() {

    sending = false;

    if(listening) listen();

}

void receiveISR(int packetSize) {
    Inspi.getRadio().onReceive(packetSize);
}

void txDoneISR() {
    Inspi.getRadio().onTxDone();
}