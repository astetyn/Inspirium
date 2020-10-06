#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include "Arduino.h"
#include "LoRa.h"
#include "PowerState.h"

static const int LORA_CS_PIN = 5;
static const int LORA_RST_PIN = 4;
static const int LORA_INT_PIN = 3;
static const int MAX_PAYLOAD = 64;

static const uint8_t FL_PASSWORD = 0x44;
static const uint8_t FL_STATUS = 0x01;


class RadioModule {

    public:
        void begin();
        void idle();
        void wakeUp();
        void sleep();
        void sendByte(const uint8_t &val);
        void sendBytes(const uint8_t *buffer, const int &len);
        void sendString(String &s);
        void sendStatus();
        void listen();
        void stopListening();
        void onReceive(const int &packetSize);
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
};

void receiveISR(const int packetSize);

#endif