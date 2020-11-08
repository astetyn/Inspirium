#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include "api/PowerState.h"
#include "api/IUtils.h"
#include "api/TaskList.h"

#include "stdint.h"

const int LORA_CS_PIN = 5;
const int LORA_RST_PIN = 4;
const int LORA_INT_PIN = 3;

const uint8_t MFLTP_KEY = 0x44;

const uint8_t FT_STATUS = 0x01;
const uint8_t FT_LIGHTS = 0x02;
const uint8_t FT_CAM = 0x03;
const uint8_t FT_HANDLING = 0x04;
const uint8_t FT_POWER = 0x05;
const uint8_t FT_ENVIRO = 0x06;

const uint8_t FT_STATUS_MAIN = 0x00;
const uint8_t FT_STATUS_WEATHER = 0x01;
const uint8_t FT_STATUS_BATT = 0x02;

const uint8_t ACK_END = 0x00;
const uint8_t ACK_CONTINUE = 0x01;
const uint8_t ACK_FAILED = 0x02;

void receiveISR(const int packetSize);
void txDoneISR();

class RadioModule {

    public:
        void begin(void (*callback)(int) = receiveISR);
        void sleep();
        void sendAck();
        void send(const uint8_t &ack, const uint8_t buffer[], const int &len);
        void listen();
        void stopListening();
        void forbidListening();
        void allowListening();
        void onReceive(const int &packetSize);
        void onTxDone();
        bool isSending();
        EventList<void (*)(IPacket *)> &getReceiveEvent() {return receiveEvent;}
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        bool listening;
        bool sending;
        bool forbidList;
        EventList<void (*)(IPacket *)> receiveEvent;
};

#endif