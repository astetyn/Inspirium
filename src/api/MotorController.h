#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "api/TaskList.h"
#include "api/PowerState.h"

#include "stdint.h"

const uint8_t FT_HANDLING_RES = 0x00;
const uint8_t FT_HANDLING_MOVE_05S = 0x01;
const uint8_t FT_HANDLING_F_1S = 0x02;
const uint8_t FT_HANDLING_B_1S = 0x03;
const uint8_t FT_HANDLING_RR_1S = 0x04;
const uint8_t FT_HANDLING_RL_1S = 0x05;
const uint8_t FT_HANDLING_SENSORS = 0x06;

const int DRIVER_EN_PIN = 7;
const int ENCODER_INTERRUPT_PIN_1 = 6;
const int ENCODER_INTERRUPT_PIN_2 = 12;
const int DRIVER_FAULT_INT_PIN = 10;
const int MOTOR_1_A_PIN = 0;
const int MOTOR_1_B_PIN = 1;
const int MOTOR_2_A_PIN = 2;
const int MOTOR_2_B_PIN = 3;
const int DEFAULT_PWM_FREQ = 50;
const int STEPS_COEF = 10;
const int FREQUENCY_OSCILLATOR = 27000000;
const int I2C_ADDR_PCA9685 = 0x40;
const int PCA9685_MODE1 = 0x00;
const int MODE1_RESTART = 0x80;
const int MODE1_SLEEP = 0x10;
const int MODE1_AI = 0x20;
const int PCA9685_PRESCALE = 0xFE;
const int PCA9685_PRESCALE_MIN = 3;
const int PCA9685_PRESCALE_MAX = 255;

class MotorController {

    public:
        MotorController();
        void begin();
        void update();
        void reset();
        void sleep();
        void setPWMFreq(int freq);
        void addTask(TaskNode *task);
        void executeNextTask();
        void stopCurrentTask();
        void forceStop();
        void move(int motor1, int motor2);
        void checkMsg(const uint8_t subFeature, uint8_t buff[], const int &buffSize);
        TaskList &getTaskList();
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        TaskList taskList;
        TaskNode *currentTask;
        unsigned long stopMillis;
        int stepsEnc1;
        int stepsEnc2;
        int stepsToPass1;
        int stepsToPass2;
        void setPWM(const int &num, int on);
        void handleEnc1Interrupt();
        void handleEnc2Interrupt();
        static void onEnc1Fall();
        static void onEnc2Fall();
};

#endif