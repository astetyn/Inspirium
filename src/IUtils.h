#ifndef UTILS_H
#define UTILS_H

#include "stdint.h"

union {
    int i;
    float f;
    double d;
    uint8_t b[8];
} converter;

struct IncomingPacket {

    IncomingPacket(uint8_t subFeature, uint8_t buff[], int buffSize) {
        this->subFeature = subFeature;
        this->buff = new uint8_t[buffSize];
        for(int i = 0; i < buffSize; i++) {
            this->buff[i] = buff[i];
        }
        this->buffSize = buffSize;
    }

    uint8_t subFeature;
    uint8_t *buff;
    int buffSize;

    ~IncomingPacket() {
        delete[] buff;
    }

};

uint8_t read8(const uint8_t &i2cAddr, const uint8_t &addr);
void write8(const uint8_t &i2cAddr, const uint8_t &addr, const uint8_t &val);

void itba(int val, uint8_t arr[], int offset);
void ftba(float val, uint8_t arr[], int offset);
void dtba(double val, uint8_t arr[], int offset);

int bati(uint8_t arr[], int offset);

#endif