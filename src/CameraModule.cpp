#include "CameraModule.h"
#include "Inspirium.h"
#include "Arduino.h"
#include "SPI.h"
#include "RadioModule.h"

void CameraModule::begin() {

    pinMode(CAM_CS_PIN, OUTPUT);
    digitalWrite(CAM_CS_PIN, HIGH);

    uint8_t vid, pid;
    uint8_t temp;

    cam.write_reg(0x07, 0x80);
    delay(100);
    cam.write_reg(0x07, 0x00);
    delay(100);

    //Check if the ArduCAM SPI bus is OK
    cam.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = cam.read_reg(ARDUCHIP_TEST1);

    if (temp != 0x55) {
        powerState = UNAVAILABLE;
        Inspi.getLights().showColor(255,0,0);
        return;
    }
    
    //Check if the camera module type is OV2640
    cam.wrSensorReg8_8(0xff, 0x01);
    cam.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    cam.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
        powerState = UNAVAILABLE;
        Inspi.getLights().showColor(255,0,0);
        return;
    }

    cam.set_format(JPEG);
    cam.InitCAM();
    cam.OV2640_set_JPEG_size(OV2640_320x240);
    delay(1000);
    cam.clear_fifo_flag();

    powerState = ACTIVE;

    buff[0] = FL_KEY;
    buff[1] = FL_IMG_PART;

}

void CameraModule::slowUpdate() {

    if(!capturing) {
        return;
    }

    if(cam.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        delay(50);
        readFifoBurst();
        cam.clear_fifo_flag();
        capturing = false;
    }

}

void CameraModule::idle() {

    if(powerState != ACTIVE) {
        return;
    }

    cam.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
    powerState = IDLE;

}

void CameraModule::wakeUp() {

    if(powerState != IDLE) {
        return;
    }

    cam.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
    delay(10);

    powerState = ACTIVE;
    
}

void CameraModule::sleep() {
    
    if(Inspi.getState() != SLEEPING) {
        return;
    }

    pinMode(CAM_CS_PIN, INPUT_PULLDOWN);
    powerState = SLEEPING;

}

void CameraModule::captureToSD() {

    if(powerState == IDLE) {
        wakeUp();
    }

}

void CameraModule::captureAndSend() {

    if(powerState == IDLE) {
        wakeUp();
    }

    Inspi.getRadio().forbidListening();

    cam.flush_fifo();
    cam.clear_fifo_flag();
    cam.start_capture();
    capturing = true;

    delay(1000);
}

void CameraModule::readFifoBurst() {

    uint8_t temp = 0, temp_last = 0;
    bool is_header = false;
    int length = 0;
    length = cam.read_fifo_length();
    cam.CS_LOW();
    cam.set_fifo_burst();
    temp = SPI.transfer(0x00);
    length--;
    while(length--) {
        temp_last = temp;
        temp = SPI.transfer(0x00);
        if(is_header) {
            addToBuff(temp);
        }else if ((temp == 0xD8) & (temp_last == 0xFF)) {
            is_header = true;
            addToBuff(temp_last);
            addToBuff(temp);
            delay(1);
        }
        if((temp == 0xD9) && (temp_last == 0xFF)) {
            break;
        }
        delayMicroseconds(15);
    }
    cam.CS_HIGH();
    is_header = false;
    completeBuff();

}

void CameraModule::addToBuff(uint8_t val){

    buff[buff_i] = val;
    buff_i++;

    if(buff_i == BUFF_MAX) {
        cam.CS_HIGH();
        Inspi.getRadio().send(buff, buff_i);
        cam.CS_LOW();
        cam.set_fifo_burst();
        buff_i = 2;
    }
}

void CameraModule::completeBuff(){

    Inspi.getRadio().send(buff, buff_i);
    Inspi.getRadio().sendFlag(FL_IMG_COMPLETE);
    buff_i = 2;
    Inspi.getRadio().allowListening();
  
}