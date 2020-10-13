#include "IUtils.h"
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