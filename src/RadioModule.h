#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include "Arduino.h"
#include "LoRa.h"

static const int LORA_CS_PIN = 5;
static const int LORA_RST_PIN = 4;
static const int LORA_INT_PIN = 3;
static const int MAX_PAYLOAD_BYTES = 64;


class RadioModule {

    public:
        void begin();
        void sleep();
        void sendString(String &s);
        void sendBytes(const uint8_t *buffer, const int &len);
};

#endif