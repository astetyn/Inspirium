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

void RadioModule::sendAck() {

    send(ACK_END, 0, 0);

}

// Sends data with key, flag and buffer. Note that this is async and you can check if data has been
// sent with isSending().
void RadioModule::send(const uint8_t &ack, const uint8_t buffer [], const int &len) {

    if(sending) return;

    if(len>MAX_PAYLOAD) return;

    if(powerState == UNAVAILABLE) return;

    if(powerState == IDLE) wakeUp();

    if(Inspi.getCam().getState() == IDLE) return;

    if(Inspi.isInRadioPSMode()) SPI.begin();

    sending = true;

    LoRa.beginPacket();
    LoRa.write(MFLTP_KEY);
    LoRa.write(ack);
    LoRa.write(buffer, len);
    LoRa.endPacket(true);

    if(Inspi.isInRadioPSMode()) Inspi.disableSPI();

}

void RadioModule::sendStatusMain() {

    float volts = Inspi.getEnviro().readBatteryVoltage();
    float temp = Inspi.getEnviro().readTemperature();
    double lat = Inspi.getLocation().getLat();
    double lng = Inspi.getLocation().getLng();

    uint8_t buff[25];
    buff[0] = Inspi.getLights().areLightsOn();
    ftba(volts, buff, 1);
    ftba(temp, buff, 5);
    dtba(lat, buff, 9);
    dtba(lng, buff, 17);

    send(ACK_END, buff, 25);

}

void RadioModule::sendStatusWeather() {

    float temp = Inspi.getEnviro().readTemperature();
    int pressure = Inspi.getEnviro().readPressure();
    int humidity = Inspi.getEnviro().readHumidity();

    uint8_t buff[12];
    ftba(temp, buff, 0);
    itba(pressure, buff, 4);
    itba(humidity, buff, 8);

    send(ACK_END, buff, 12);

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

    uint8_t *buff = new uint8_t[packetSize];
    LoRa.readBytes(buff, packetSize);

    if(Inspi.isInRadioPSMode()) Inspi.disableSPI();

    if(buff[0] != MFLTP_KEY){
        return;
    }

    uint8_t feature = buff[1];
    uint8_t subFeature = buff[2];

    switch (feature) {

        case FT_LIGHTS:

            Inspi.getLights().processMsg(subFeature, buff, packetSize);
            delete[] buff;
            break;

        case FT_CAM:

            Inspi.getCam().processMsg(subFeature, buff, packetSize);
            delete[] buff;
            break;

        case FT_STATUS:

            if(subFeature == FT_STATUS_MAIN) {
                sendStatusMain();
            }else if(subFeature == FT_STATUS_WEATHER) {
                sendStatusWeather();
            }
            delete[] buff;
            break;

        case FT_HANDLING:

            Inspi.getMC().processMsg(subFeature, buff, packetSize);
            delete[] buff;
            break;

        case FT_POWER:

            IncomingPacket *packet = new IncomingPacket(subFeature, buff, packetSize);
            Inspi.incomingMessage(packet);
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