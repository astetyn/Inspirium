#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "Wire.h"

static const int ENCODER_INTERRUPT_PIN_1 = 6;
static const int ENCODER_INTERRUPT_PIN_2 = 12;
static const int MOTOR_1_A_PIN = 0;
static const int MOTOR_1_B_PIN = 1;
static const int MOTOR_2_A_PIN = 2;
static const int MOTOR_2_B_PIN = 3;
static const uint32_t FREQUENCY_OSCILLATOR = 27000000;
static const uint8_t I2C_ADDR_PCA9685 = 0x40;
static const uint8_t PCA9685_MODE1 = 0x00;
static const uint8_t MODE1_RESTART = 0x80;
static const uint8_t MODE1_SLEEP = 0x10;
static const uint8_t MODE1_AI = 0x20;
static const uint8_t PCA9685_PRESCALE = 0xFE;
static const int PCA9685_PRESCALE_MIN = 3;
static const int PCA9685_PRESCALE_MAX = 255;

class MotorController {

    public:
        MotorController();
        void begin();
        void update();
        void reset();
        void sleep();
        void wakeup();
        void setPWMFreq(float freq);
        void stop();
        void moveForward(uint8_t speed, bool reverse);
        void moveForward(uint32_t millimetres, uint8_t speed, bool reverse);
        void moveForwardMillis(uint32_t millis, uint8_t speed, bool reverse);
        void rotateAngle(float angle, uint8_t speed);
        void rotateMillis(uint32_t millis, uint8_t speed, bool reverse);
        void addTask();

    private:
        long stopT = 0;
        int stopCount = 0;
        int countEnc1 = 0;
        int countEnc2 = 0;
        bool correctionActive = false;
        TwoWire *_i2c;
        uint8_t read8(uint8_t addr);
        void write8(uint8_t addr, uint8_t d);
        void setPWM(uint8_t num, uint16_t on);
        void executeNextTask();
        void handleEnc1Interrupt();
        void handleEnc2Interrupt();
        static void onEnc1Fall();
        static void onEnc2Fall();
};

#endif