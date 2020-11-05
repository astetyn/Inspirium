#include "api/RadioModule.h"

#include "api/libraries/L_LoRa.h"
#include "api/API.h"
#include "api/IUtils.h"

#include "Arduino.h"

void RadioModule::begin(void (*callback)(int)) {

    LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_INT_PIN);
    LoRa.begin(868E6);
    LoRa.setSpreadingFactor(9);
    LoRa.onReceive(receiveISR);
    LoRa.onTxDone(txDoneISR);
    listening = false;
    sending = false;

    powerState = PowerState::ACTIVE;

}

void RadioModule::sleep() {

    if(!API.getPower().is3V3Enabled()) SPI.begin();

    LoRa.sleep();

    if(!API.getPower().is3V3Enabled()) API.disableSPI();

    listening = false;
    sending = false;

    pinMode(LORA_CS_PIN, INPUT_PULLDOWN);

    powerState = PowerState::SLEEPING;

}

void RadioModule::sendAck() {
    send(ACK_END, 0, 0);
}

// Sends data with key, flag and buffer. Note that this is async and you can check if data has been
// sent with isSending().
void RadioModule::send(const uint8_t &ack, const uint8_t buffer[], const int &len) {

    if(sending) return;

    if(len > MAX_PAYLOAD) return;

    if(powerState == PowerState::UNAVAILABLE) return;

    if(!API.getPower().is3V3Enabled()) SPI.begin();

    sending = true;

    LoRa.beginPacket();
    LoRa.write(MFLTP_KEY);
    LoRa.write(ack);
    LoRa.write(buffer, len);
    LoRa.endPacket(true);

    if(!API.getPower().is3V3Enabled()) API.disableSPI();

}

// This will put radio into continuos receive mode. 
// Note that if message arrive, interrupt will take all processing power and time and may cause delays.
void RadioModule::listen() {

    if(forbidList) return;

    if(sending) return;

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

    uint8_t *buff = new uint8_t[packetSize];
    LoRa.readBytes(buff, packetSize);

    if(!API.getPower().is3V3Enabled()) API.disableSPI();

    if(buff[0] != MFLTP_KEY) {
        return;
    }

    uint8_t feature = buff[1];
    uint8_t subFeature = buff[2];

    IPacket *packet = new IPacket(subFeature, buff, packetSize);

    for(int i = 0; i < receiveEvent.size(); i++) {

        receiveEvent.getAt(i)(packet);

        if(packet->cancelled) {
            delete packet;
            return;
        }
    }

    switch (feature) {

        case FT_LIGHTS:

            API.getLights().incomingPacket(packet);
            break;

        case FT_CAM:

            API.getCam().incomingPacket(packet);
            break;

        case FT_STATUS:

            API.getStatMng().incomingPacket(packet);
            break;

        case FT_HANDLING:
            API.getMC().checkMsg(subFeature, buff, packetSize);
            delete packet;
            break;

        case FT_POWER:
            API.getPower().incomingPacket(packet);
            break;
        
        case FT_ENVIRO:
            API.getEnviro().checkMsg(subFeature, buff, packetSize);
            delete packet;
            break;
    }

}

bool RadioModule::isSending() {
    return sending;
}

void RadioModule::onTxDone() {

    sending = false;

    if(listening) listen();

    if(!API.getPower().is3V3Enabled()) API.disableSPI();

}

void receiveISR(int packetSize) {
    API.getRadio().onReceive(packetSize);
}

void txDoneISR() {
    API.getRadio().onTxDone();
}