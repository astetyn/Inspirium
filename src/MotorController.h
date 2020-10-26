#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "TaskList.h"
#include "PowerState.h"
#include "stdint.h"

const uint8_t FT_HANDLING_RES = 0x00;
const uint8_t FT_HANDLING_MOVE_05S = 0x01;
const uint8_t FT_HANDLING_F_1S = 0x02;
const uint8_t FT_HANDLING_B_1S = 0x03;
const uint8_t FT_HANDLING_RR_1S = 0x04;
const uint8_t FT_HANDLING_RL_1S = 0x05;
const uint8_t FT_HANDLING_SENSORS = 0x06;

static const int DRIVER_EN_PIN = 7;
static const int ENCODER_INTERRUPT_PIN_1 = 6;
static const int ENCODER_INTERRUPT_PIN_2 = 12;
static const int DRIVER_FAULT_INT_PIN = 10;
static const int MOTOR_1_A_PIN = 0;
static const int MOTOR_1_B_PIN = 1;
static const int MOTOR_2_A_PIN = 2;
static const int MOTOR_2_B_PIN = 3;
static const int DEFAULT_PWM_FREQ = 50;
static const int STEPS_COEF = 10;

static const int FREQUENCY_OSCILLATOR = 27000000;
static const int I2C_ADDR_PCA9685 = 0x40;
static const int PCA9685_MODE1 = 0x00;
static const int MODE1_RESTART = 0x80;
static const int MODE1_SLEEP = 0x10;
static const int MODE1_AI = 0x20;
static const int PCA9685_PRESCALE = 0xFE;
static const int PCA9685_PRESCALE_MIN = 3;
static const int PCA9685_PRESCALE_MAX = 255;

class MotorController {

    public:
        MotorController();
        void begin();
        void update();
        void reset();
        void idle();
        void wakeUp();
        void sleep();
        void setPWMFreq(int freq);
        void addTask(TaskNode *task);
        void executeNextTask();
        void stopCurrentTask();
        void forceStop();
        void move(int motor1, int motor2);
        void processMsg(const uint8_t subFeature, uint8_t buff[], const int &buffSize);
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