#ifndef UTILS_H
#define UTILS_H

#include "api/libraries/L_RTCZero.h"

#include "stdint.h"

union {
    int i;
    float f;
    double d;
    uint8_t b[8];
} converter;

struct IPacket {

    uint8_t subFeature;
    uint8_t *buff;
    int buffSize;
    bool cancelled = false;

    IPacket(uint8_t subFeature, uint8_t buff[], int buffSize);
    ~IPacket();

};

struct Time {

    int years;
    int months;
    int days;
    int hours;
    int mins;
    int secs;

    Time(int &years, int &months, int &days, int &hours, int &mins, int &secs);
    Time(uint8_t &years, uint8_t &months, uint8_t &days, uint8_t hours, uint8_t mins, uint8_t secs);
    Time(RTCZero &rtc);

    void add(int hours, int mins, int secs);

};

uint8_t read8(const uint8_t &i2cAddr, const uint8_t &addr);
void write8(const uint8_t &i2cAddr, const uint8_t &addr, const uint8_t &val);

void itba(int val, uint8_t arr[], int offset);
void ftba(float val, uint8_t arr[], int offset);
void dtba(double val, uint8_t arr[], int offset);

int bati(uint8_t arr[], int offset);

int changeToSeconds(int &hours, int &minutes, int &seconds);
int getHoursFromSecs(int &seconds);
int getMinsFromSecs(int &seconds);

#endif