#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include "stdint.h"
#include "PowerState.h"

static const int LORA_CS_PIN = 5;
static const int LORA_RST_PIN = 4;
static const int LORA_INT_PIN = 3;
static const int MAX_PAYLOAD = 100;

const uint8_t FL_KEY = 0x44;
const uint8_t FL_STATUS = 0x01;
const uint8_t FL_LIGHTS_ON = 0x02;
const uint8_t FL_LIGHTS_OFF = 0x03;
const uint8_t FL_IMG_TAKE = 0x04;
const uint8_t FL_IMG_PART = 0x05;
const uint8_t FL_IMG_COMPLETE = 0x06;
const uint8_t FL_IMG_GET = 0x07;
const uint8_t FL_HANDLING_START = 0xEE;
const uint8_t FL_HANDLING_STOP = 0xED;
const uint8_t FL_NOT_AVAILABLE = 0x88;

void receiveISR(const int packetSize);
void txDoneISR();

class RadioModule {

    public:
        void begin(void (*callback)(int) = receiveISR);
        void idle();
        void wakeUp();
        void sleep();
        void send(const uint8_t *buffer, const int &len);
        void sendFlag(uint8_t flag);
        void sendStatus();
        void listen();
        void stopListening();
        void forbidListening();
        void allowListening();
        void onReceive(const int &packetSize);
        void onTxDone();
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        bool listening;
        bool forbidList;
};

#endif