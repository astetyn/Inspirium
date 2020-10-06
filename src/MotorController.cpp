#include "MotorController.h"
#include "Arduino.h"
#include "Wire.h"
#include "Inspirium.h"

MotorController::MotorController() {

    stopMillis = 0;
    stepsEnc1 = 0;
    stepsEnc2 = 0;
    stepsToPass1 = 0;
    stepsToPass2 = 0;

}

void MotorController::begin() {
  
    pinMode(DRIVER_EN_PIN, OUTPUT);
    digitalWrite(DRIVER_EN_PIN, HIGH);

    pinMode(ENCODER_INTERRUPT_PIN_1, INPUT_PULLUP);
    pinMode(ENCODER_INTERRUPT_PIN_2, INPUT_PULLUP);

    pinMode(DRIVER_FAULT_INT_PIN, INPUT_PULLDOWN);

    _i2c = &Wire;
    _i2c->begin();
    reset();
    setPWMFreq(DEFAULT_PWM_FREQ);

    attachInterrupt(ENCODER_INTERRUPT_PIN_1, onEnc1Fall, FALLING);
    attachInterrupt(ENCODER_INTERRUPT_PIN_2, onEnc2Fall, FALLING);

    forceStop();

    powerState = ACTIVE;

}

void MotorController::update() {

    if(currentTask == 0){
      return;
    }

    if(currentTask->type == TaskNode::TYPE_TIME) {
      if(millis()>=stopMillis) {
        executeNextTask();
      }
    }
}

void MotorController::addTask(TaskNode *task) {

    taskList.add(task);

}

void MotorController::executeNextTask() {
  
    if(powerState == IDLE) {
        wakeUp();
    }

    if(currentTask!=0) {
      taskList.pop();
    }

    currentTask = taskList.front();

    if(currentTask==0) {
      forceStop();
      return;
    }

    if(currentTask->type == TaskNode::TYPE_TIME) {

      TimeTask *tt = (TimeTask *)currentTask;
      stopMillis = millis() + tt->millis;
      
    }else if(currentTask->type == TaskNode::TYPE_DISTANCE) {

      DistanceTask *dt = (DistanceTask *)currentTask;
      stepsToPass1 = dt->centimetres/127.0*currentTask->motor1*STEPS_COEF;
      stepsToPass2 = dt->centimetres/127.0*currentTask->motor2*STEPS_COEF;

    }

    int onPeriod1_A = abs(max((int)(4095/127.0*-currentTask->motor1), 0));
    int onPeriod1_B = abs(min((int)(4095/127.0*-currentTask->motor1), 0));
    int onPeriod2_A = abs(max((int)(4095/127.0*currentTask->motor2), 0));
    int onPeriod2_B = abs(min((int)(4095/127.0*currentTask->motor2), 0));

    setPWM(MOTOR_1_A_PIN, onPeriod1_A);
    setPWM(MOTOR_1_B_PIN, onPeriod1_B);
    setPWM(MOTOR_2_A_PIN, onPeriod2_A);
    setPWM(MOTOR_2_B_PIN, onPeriod2_B);

}

void MotorController::stopCurrentTask() {

    taskList.pop();
    currentTask = 0;
    forceStop();

}

void MotorController::forceStop() {

    setPWM(MOTOR_1_A_PIN, 0);
    setPWM(MOTOR_1_B_PIN, 0);
    setPWM(MOTOR_2_A_PIN, 0);
    setPWM(MOTOR_2_B_PIN, 0);

}

void MotorController::move(int motor1, int motor2) {

    if(powerState == IDLE) {
        wakeUp();
    }

    if(currentTask!=0) {
        taskList.pop();
        currentTask = 0;
    }

    int onPeriod1_A = abs(max((int)(4095/127.0*-motor1), 0));
    int onPeriod1_B = abs(min((int)(4095/127.0*-motor1), 0));
    int onPeriod2_A = abs(max((int)(4095/127.0*motor2), 0));
    int onPeriod2_B = abs(min((int)(4095/127.0*motor2), 0));

    setPWM(MOTOR_1_A_PIN, onPeriod1_A);
    setPWM(MOTOR_1_B_PIN, onPeriod1_B);
    setPWM(MOTOR_2_A_PIN, onPeriod2_A);
    setPWM(MOTOR_2_B_PIN, onPeriod2_B);

}

void MotorController::reset() {

    write8(PCA9685_MODE1, MODE1_RESTART);
    delay(10);

}

// on = on period time, on = 0 - fully closed 4095 - fully open
void MotorController::setPWM(const int &num, int on) {

    int off = 0;

    if(on >= 4095){
      on = 4096;
    }else if(on == 0){
      off = 4096;
    }else{
      on = 4095 - on;
    }

    _i2c->beginTransmission(I2C_ADDR_PCA9685);
    _i2c->write(0x06 + 4 * num); // 0x06 = PCA9685_LED0_ON_L
    _i2c->write(on);
    _i2c->write(on >> 8);
    _i2c->write(off);
    _i2c->write(off >> 8);
    _i2c->endTransmission();

}

void MotorController::setPWMFreq(int freq) {

    if(powerState == IDLE) {
        wakeUp();
    }

    if (freq < 1) {
      freq = 1;
    }
    if (freq > 3500) {
      freq = 3500;
    }
    float prescaleval = ((FREQUENCY_OSCILLATOR / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
      prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
      prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    uint8_t oldmode = read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP;
    write8(PCA9685_MODE1, newmode);
    write8(PCA9685_PRESCALE, prescale);
    write8(PCA9685_MODE1, oldmode);
    delay(5);
    write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);

}

void MotorController::idle() {

    if(powerState != ACTIVE) {
        return;
    }

    detachInterrupt(ENCODER_INTERRUPT_PIN_1);
    detachInterrupt(ENCODER_INTERRUPT_PIN_2);
    pinMode(ENCODER_INTERRUPT_PIN_1, INPUT_PULLDOWN);
    pinMode(ENCODER_INTERRUPT_PIN_2, INPUT_PULLDOWN);
    uint8_t awake = read8(PCA9685_MODE1);
    uint8_t sleep = awake | MODE1_SLEEP;
    write8(PCA9685_MODE1, sleep);
    delay(5);
    digitalWrite(DRIVER_EN_PIN, LOW);

    powerState = IDLE;

}

void MotorController::wakeUp() {

    pinMode(ENCODER_INTERRUPT_PIN_1, INPUT_PULLUP);
    pinMode(ENCODER_INTERRUPT_PIN_2, INPUT_PULLUP);
    attachInterrupt(ENCODER_INTERRUPT_PIN_1, onEnc1Fall, FALLING);
    attachInterrupt(ENCODER_INTERRUPT_PIN_2, onEnc2Fall, FALLING);
    uint8_t sleep = read8(PCA9685_MODE1);
    uint8_t wakeup = sleep & ~MODE1_SLEEP;
    write8(PCA9685_MODE1, wakeup);
    digitalWrite(DRIVER_EN_PIN, HIGH);

    powerState = ACTIVE;

}

void MotorController::sleep() {

    idle();
    _i2c->end();

    powerState = SLEEPING;

}

uint8_t MotorController::read8(const int &addr) {

    _i2c->beginTransmission(I2C_ADDR_PCA9685);
    _i2c->write(addr);
    _i2c->endTransmission();

    _i2c->requestFrom(I2C_ADDR_PCA9685, (uint8_t)1);
    return _i2c->read();

}

void MotorController::write8(const int &addr, const int &d) {

    _i2c->beginTransmission(I2C_ADDR_PCA9685);
    _i2c->write(addr);
    _i2c->write(d);
    _i2c->endTransmission();

}

void MotorController::handleEnc1Interrupt() {

    if(currentTask->type != TaskNode::TYPE_DISTANCE) {
      return;
    }

    stepsEnc1++;

    if(stepsEnc1 == stepsToPass1) {
      executeNextTask();
    }

    //TODO correction
}

void MotorController::handleEnc2Interrupt() {

    if(currentTask->type != TaskNode::TYPE_DISTANCE) {
      return;
    }

    stepsEnc2++;

    if(stepsEnc2 == stepsToPass2) {
      executeNextTask();
    }

    //TODO correction
}

void MotorController::onEnc1Fall() {
    Inspi.getMC().handleEnc1Interrupt();
}

void MotorController::onEnc2Fall() {
    Inspi.getMC().handleEnc2Interrupt();
}

TaskList &MotorController::getTaskList() {
    return taskList;
}