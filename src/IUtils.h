#ifndef UTILS_H
#define UTILS_H

#include "stdint.h"

uint8_t read8(const uint8_t &i2cAddr, const uint8_t &addr);
void write8(const uint8_t &i2cAddr, const uint8_t &addr, const uint8_t &val);

#endif