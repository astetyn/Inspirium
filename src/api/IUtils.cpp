#include "api/IUtils.h"

#include "Wire.h"

uint8_t read8(const uint8_t &i2cAddr, const uint8_t &addr) {

    Wire.beginTransmission(i2cAddr);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddr, (uint8_t)1);
    return Wire.read();

}

void write8(const uint8_t &i2cAddr, const uint8_t &addr, const uint8_t &val) {

    Wire.beginTransmission(i2cAddr);
    Wire.write(addr);
    Wire.write(val);
    Wire.endTransmission();

}

// Converts int into the byte array. Bytes are stored in array given.
void itba(int val, uint8_t arr[], int offset) {
    
    converter.i = val;
    for(int i = 0; i < 4; i++) {
        arr[i + offset] = converter.b[i];
    }
    
}

// Converts float into the byte array. Bytes are stored in array given.
void ftba(float val, uint8_t arr[], int offset) {
    
    converter.f = val;
    for(int i = 0; i < 4; i++) {
        arr[i + offset] = converter.b[i];
    }
    
}

// Converts double into the byte array. Bytes are stored in array given.
void dtba(double val, uint8_t arr[], int offset) {
    
    converter.d = val;
    for(int i = 0; i < 8; i++) {
        arr[i + offset] = converter.b[i];
    }
    
}

int bati(uint8_t arr[], int offset) {

    for(int i = 0; i<4;i++) {
        converter.b[i] = arr[i+offset];
    }
    return converter.i;

}

IPacket::IPacket(uint8_t subFeature, uint8_t buff[], int buffSize) {

    this->subFeature = subFeature;
    this->buff = new uint8_t[buffSize];
    for(int i = 0; i < buffSize; i++) {
        this->buff[i] = buff[i];
    }
    this->buffSize = buffSize;

}

IPacket::~IPacket() {
    delete[] buff;
}

Time::Time(int &years, int &months, int &days, int &hours, int &mins, int &secs) : 
    years(years), months(months), days(days),
    hours(hours), mins(mins), secs(secs)  {

}

Time::Time(uint8_t &years, uint8_t &months, uint8_t &days, uint8_t hours, uint8_t mins, uint8_t secs) : 
    years(years), months(months), days(days),
    hours(hours), mins(mins), secs(secs)  {

}

Time::Time(RTCZero &rtc) : 
    years(rtc.getYear()), months(rtc.getMonth()), days(rtc.getDay()),
    hours(rtc.getHours()), mins(rtc.getMinutes()), secs(rtc.getSeconds())  {

}

void Time::add(int hours, int mins, int secs) {

    int seconds = 0;

    seconds += days * 3600 * 24;
    seconds += (hours + this->hours) * 3600;
    seconds += (mins + this->mins) * 60;
    seconds += secs + this->secs;

    days = seconds / (3600 * 24);
    seconds -= days * 3600 * 24;

    this->hours = seconds / 3600;
    seconds -= this->hours * 3600;

    this->mins = seconds / 60;
    seconds -= this->mins * 60;

    this->secs = seconds;

}