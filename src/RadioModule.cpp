#include "RadioModule.h"

void RadioModule::begin() {

    LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_INT_PIN);
    LoRa.begin(868E6);
    LoRa.setSpreadingFactor(7);

}

void RadioModule::sleep() {

    LoRa.sleep();

}

void RadioModule::sendBytes(const uint8_t *buffer, const int &len) {

    if(len>MAX_PAYLOAD_BYTES) {
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