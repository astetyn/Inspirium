#include "MotorController.h"
#include "Arduino.h"
#include "Arduino.h"
#include "Wire.h"
#include "Inspirium.h"

MotorController::MotorController() {

}

void MotorController::begin() {
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  pinMode(ENCODER_INTERRUPT_PIN_1, INPUT_PULLUP);
  pinMode(ENCODER_INTERRUPT_PIN_2, INPUT_PULLUP);

  _i2c = &Wire;
  _i2c->begin();
  reset();
  setPWMFreq(50);

  attachInterrupt(ENCODER_INTERRUPT_PIN_1, onEnc1Fall, FALLING);
  attachInterrupt(ENCODER_INTERRUPT_PIN_2, onEnc2Fall, FALLING);
}

void MotorController::update() {
  if(millis()>=stopT){
    executeNextTask();
  }
}

void MotorController::stop() {
  correctionActive = false;
  stopT = -1;
  stopCount = -1;
  setPWM(MOTOR_1_A_PIN, 0);
  setPWM(MOTOR_1_B_PIN, 0);
  setPWM(MOTOR_2_A_PIN, 0);
  setPWM(MOTOR_2_B_PIN, 0);
}

void MotorController::moveForward(uint8_t speed, bool reverse) {
  correctionActive = true;
  stopT = -1;
  stopCount = -1;
  uint16_t onPeriod = (4095/255)*speed;
  
  if(reverse) {
    setPWM(MOTOR_1_A_PIN, onPeriod);
    setPWM(MOTOR_1_B_PIN, 0);
    setPWM(MOTOR_2_A_PIN, 0);
    setPWM(MOTOR_2_B_PIN, onPeriod);
  }else{
    setPWM(MOTOR_1_A_PIN, 0);
    setPWM(MOTOR_1_B_PIN, onPeriod);
    setPWM(MOTOR_2_A_PIN, onPeriod);
    setPWM(MOTOR_2_B_PIN, 0);
  }
}

void MotorController::moveForward(uint32_t centimetres, uint8_t speed, bool reverse) {

  if(centimetres == 0||speed == 0) {
    executeNextTask();
  }

  countEnc1 = 0;
  countEnc2 = 0;
  stopCount = 30*centimetres; //TODO - odmerat presnu hodnotu
  stopT = -1;
  correctionActive = true;
  uint16_t onPeriod = (4095/255)*speed;

  if(reverse) {
    setPWM(MOTOR_1_A_PIN, onPeriod);
    setPWM(MOTOR_1_B_PIN, 0);
    setPWM(MOTOR_2_A_PIN, 0);
    setPWM(MOTOR_2_B_PIN, onPeriod);
  }else{
    setPWM(MOTOR_1_A_PIN, 0);
    setPWM(MOTOR_1_B_PIN, onPeriod);
    setPWM(MOTOR_2_A_PIN, onPeriod);
    setPWM(MOTOR_2_B_PIN, 0);
  }
}

void MotorController::moveForwardMillis(uint32_t millisecs, uint8_t speed, bool reverse) {

  if(millisecs == 0||speed == 0) {
    executeNextTask();
  }

  countEnc1 = 0;
  countEnc2 = 0;
  stopCount = -1;
  stopT = millis()+millisecs;
  correctionActive = true;
  uint16_t onPeriod = (4095/255)*speed;

  if(reverse) {
    setPWM(MOTOR_1_A_PIN, onPeriod);
    setPWM(MOTOR_1_B_PIN, 0);
    setPWM(MOTOR_2_A_PIN, 0);
    setPWM(MOTOR_2_B_PIN, onPeriod);
  }else{
    setPWM(MOTOR_1_A_PIN, 0);
    setPWM(MOTOR_1_B_PIN, onPeriod);
    setPWM(MOTOR_2_A_PIN, onPeriod);
    setPWM(MOTOR_2_B_PIN, 0);
  }
}

void MotorController::rotateAngle(float angle, uint8_t speed) {
  //TODO
}

void MotorController::rotateMillis(uint32_t millisecs, uint8_t speed, bool reverse) {

  if(millisecs == 0||speed == 0) {
    executeNextTask();
  }

  stopCount = -1;
  stopT = millis()+millisecs;
  uint16_t onPeriod = (4095/255)*speed;

  if(reverse) {
    setPWM(MOTOR_1_A_PIN, onPeriod);
    setPWM(MOTOR_1_B_PIN, 0);
    setPWM(MOTOR_2_A_PIN, onPeriod);
    setPWM(MOTOR_2_B_PIN, 0);
  }else{
    setPWM(MOTOR_1_A_PIN, 0);
    setPWM(MOTOR_1_B_PIN, onPeriod);
    setPWM(MOTOR_2_A_PIN, 0);
    setPWM(MOTOR_2_B_PIN, onPeriod);
  }
}

void MotorController::addTask() {

}

void MotorController::executeNextTask() {
  stop();
}

void MotorController::reset() {
  write8(PCA9685_MODE1, MODE1_RESTART);
  delay(10);
}

// on = on period time, on = 0 - fully closed 4095 - fully open
void MotorController::setPWM(uint8_t num, uint16_t on) {

  uint16_t off = 0;

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

void MotorController::setPWMFreq(float freq) {

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

void MotorController::sleep() {
  detachInterrupt(ENCODER_INTERRUPT_PIN_1);
  detachInterrupt(ENCODER_INTERRUPT_PIN_2);
  pinMode(ENCODER_INTERRUPT_PIN_1, INPUT_PULLDOWN);
  pinMode(ENCODER_INTERRUPT_PIN_2, INPUT_PULLDOWN);
  uint8_t awake = read8(PCA9685_MODE1);
  uint8_t sleep = awake | MODE1_SLEEP;
  write8(PCA9685_MODE1, sleep);
  delay(5);
}

void MotorController::wakeup() {
  pinMode(ENCODER_INTERRUPT_PIN_1, INPUT_PULLUP);
  pinMode(ENCODER_INTERRUPT_PIN_2, INPUT_PULLUP);
  attachInterrupt(ENCODER_INTERRUPT_PIN_1, onEnc1Fall, FALLING);
  attachInterrupt(ENCODER_INTERRUPT_PIN_2, onEnc2Fall, FALLING);
  uint8_t sleep = read8(PCA9685_MODE1);
  uint8_t wakeup = sleep & ~MODE1_SLEEP;
  write8(PCA9685_MODE1, wakeup);
}

uint8_t MotorController::read8(uint8_t addr) {
  _i2c->beginTransmission(I2C_ADDR_PCA9685);
  _i2c->write(addr);
  _i2c->endTransmission();

  _i2c->requestFrom((uint8_t)I2C_ADDR_PCA9685, (uint8_t)1);
  return _i2c->read();
}

void MotorController::write8(uint8_t addr, uint8_t d) {
  _i2c->beginTransmission(I2C_ADDR_PCA9685);
  _i2c->write(addr);
  _i2c->write(d);
  _i2c->endTransmission();
}

void MotorController::handleEnc1Interrupt() {
  countEnc1++;
  if(countEnc1==stopCount) {
    executeNextTask();
  }
  if(!correctionActive){
    return;
  }
  //TODO podmienky pre correction
}

void MotorController::handleEnc2Interrupt() {
  countEnc2++;
  if(countEnc2==stopCount) {
    executeNextTask();
  }
  if(!correctionActive){
    return;
  }
  //TODO podmienky pre correction
}

void MotorController::onEnc1Fall() {
  Inspirium.getMotorController().handleEnc1Interrupt();
}

void MotorController::onEnc2Fall() {
  Inspirium.getMotorController().handleEnc2Interrupt();
}