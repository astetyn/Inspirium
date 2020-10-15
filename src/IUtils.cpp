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

// Converts byte array into the int.
int bati(uint8_t arr[]) {
    
    int val = 0;
    
    val = (int) arr[0];
    val |=  (int) arr[1] << 8;
    val |= (int) arr[2] << 16;
    val |= (int) arr[3] << 24;
    
    return val;
    
}