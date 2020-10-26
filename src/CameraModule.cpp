#include "CameraModule.h"
#include "Inspirium.h"
#include "Arduino.h"
#include "SPI.h"
#include "RadioModule.h"
#include "IUtils.h"

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
        return;
    }
    
    //Check if the camera module type is OV2640
    cam.wrSensorReg8_8(0xff, 0x01);
    cam.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    cam.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
        powerState = UNAVAILABLE;
        return;
    }

    cam.set_format(JPEG);
    cam.InitCAM();
    cam.OV2640_set_JPEG_size(OV2640_160x120);
    delay(1000);
    cam.clear_fifo_flag();

    powerState = ACTIVE;

}

void CameraModule::update() {

    if(powerState != ACTIVE) return;

    if(bufferReady && !Inspi.getRadio().isSending()) {
        if(imgCompleted) {
            Inspi.getRadio().send(ACK_END, buff, buff_i);
        }else {
            Inspi.getRadio().send(ACK_CONTINUE, buff, buff_i);
        }
        bufferReady = false;
    }

    if(length != 0 && imgCompleted && !Inspi.getRadio().isSending()) {
        Inspi.getRadio().allowListening();
        length = 0;
    }

    if(!imgCompleted && !Inspi.getRadio().isSending()) {
        if(length == 0) {
            readFifoBurst(false);
        }else{
            readFifoBurst(true);
        }
    }

    if(capturing && cam.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        capturing = false;
        imgCompleted = false;
        length = 0;
    }

}

// Put camera into low power mode.
// Note that you can not use Radio and Storage with this power state.
void CameraModule::idle() {

    if(powerState != ACTIVE) return;

    cam.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
    delay(100);
    powerState = IDLE;

}

// This will put camera into active mode. Note that this causes
// significant delay.
void CameraModule::wakeUp() {

    if(powerState != IDLE) return;

    cam.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
    delay(1000);

    powerState = ACTIVE;
    
}

void CameraModule::sleep() {
    
    if(Inspi.getState() != SLEEPING) return;

    pinMode(CAM_CS_PIN, INPUT_PULLDOWN);
    powerState = SLEEPING;

}

void CameraModule::captureToSD() {

    if(powerState == IDLE) wakeUp();

}

void CameraModule::captureAndSend() {

    if(powerState == IDLE) wakeUp();

    Inspi.getRadio().forbidListening();

    cam.clear_fifo_flag();
    cam.start_capture();
    capturing = true;

}

void CameraModule::readFifoBurst(bool is_header) {

    buff_i = 0;

    if(!is_header) length = cam.read_fifo_length();

    cam.CS_LOW();
    cam.set_fifo_burst();
    
    while(length--) {
        temp_last = temp;
        temp = SPI.transfer(0x00);

        if(is_header) {
            if(addToBuff(temp)) break;
        }else if(temp == 0xD8 && temp_last == 0xFF) {
            is_header = true;
            addToBuff(temp_last);
            addToBuff(temp);
        }

        if((temp == 0xD9 && temp_last == 0xFF) || length == 0) {
            bufferReady = true;
            imgCompleted = true;
            break;
        }
        delayMicroseconds(15);
    }
    cam.CS_HIGH();
}

bool CameraModule::addToBuff(int &val){

    buff[buff_i] = val;
    buff_i++;

    if(buff_i == BUFF_MAX) bufferReady = true;

    return bufferReady;
}

void CameraModule::processMsg(const uint8_t subFeature, uint8_t buff[], const int &buffSize) {

    if(subFeature == FT_CAM_CAPTURE) {
        captureAndSend();
    }else if(subFeature == FT_CAM_160x120) {
        cam.OV2640_set_JPEG_size(OV2640_160x120);
    }else if(subFeature == FT_CAM_320x240) {
        cam.OV2640_set_JPEG_size(OV2640_320x240);
    }else if(subFeature == FT_CAM_640x480) {
        cam.OV2640_set_JPEG_size(OV2640_640x480);
    }else if(subFeature == FT_CAM_800x600) {
        cam.OV2640_set_JPEG_size(OV2640_800x600);
    }else if(subFeature == FT_CAM_1024x768) {
        cam.OV2640_set_JPEG_size(OV2640_1024x768);
    }else if(subFeature == FT_CAM_1600x1200) {
        cam.OV2640_set_JPEG_size(OV2640_1600x1200);
    }

}