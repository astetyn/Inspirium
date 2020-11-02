#include "api/CameraModule.h"

#include "api/API.h"

#include "Arduino.h"
#include "SPI.h"

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
        powerState = PowerState::UNAVAILABLE;
        API.getLights().showColor(255, 0, 0);
        return;
    }
    
    //Check if the camera module type is OV2640
    cam.wrSensorReg8_8(0xff, 0x01);
    cam.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    cam.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
        powerState = PowerState::UNAVAILABLE;
        API.getLights().showColor(255, 0, 0);
        return;
    }

    cam.set_format(JPEG);
    cam.InitCAM();
    cam.OV2640_set_JPEG_size(OV2640_160x120);
    delay(200);
    cam.clear_fifo_flag();

    packet = 0;

    powerState = PowerState::ACTIVE;

}

void CameraModule::update() {

    checkMsg();

    if(powerState != PowerState::ACTIVE) return;

    // Buffer sending
    if(bufferReady && !API.getRadio().isSending()) {
        if(imgCompleted) {
            API.getRadio().send(ACK_END, buff, buff_i);
        }else {
            API.getRadio().send(ACK_CONTINUE, buff, buff_i);
        }
        bufferReady = false;
    }

    // Final image complete end sequence
    if(length != 0 && imgCompleted && !API.getRadio().isSending()) {

        API.getRadio().allowListening();
        length = 0;

        for(int i = 0; i < imageCompleteEvent.size(); i++) {
            imageCompleteEvent.getAt(i)();
        }
    }

    // Read checker
    if(!imgCompleted && !API.getRadio().isSending()) {
        if(length == 0) {
            readFifoBurst(false);
        }else{
            readFifoBurst(true);
        }
    }

    // Capture complete
    if(capturing && cam.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        capturing = false;
        imgCompleted = false;
        length = 0;
    }

}

void CameraModule::sleep() {

    cam.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);

    pinMode(CAM_CS_PIN, INPUT_PULLDOWN);
    powerState = PowerState::SLEEPING;

}

void CameraModule::captureToSD() {

}

void CameraModule::captureAndSend() {

    API.getRadio().forbidListening();

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

void CameraModule::incomingPacket(IPacket *packet) {

    this->packet = packet;

}

void CameraModule::checkMsg() {

    if(packet == 0) return;

    if(packet->subFeature == FT_CAM_CAPTURE) {
        API.getEnviro().getRTC().disableAlarm();
        API.getPower().enable3V3();
        delay(500);
        begin();
        API.getRadio().begin();
        captureAndSend();
    }

    delete packet;
    packet = 0;

}