#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

//#include "libraries/L_ArduCAM.h"
#include "ArduCAM.h"
#include "api/PowerState.h"
#include "api/IUtils.h"
#include "api/TaskList.h"

const int CAM_CS_PIN = 38;
const int BUFF_MAX = 200;

const uint8_t FT_CAM_CAPTURE = 0x00;
const uint8_t FT_CAM_160x120 = 0x01;
const uint8_t FT_CAM_320x240 = 0x02;
const uint8_t FT_CAM_640x480 = 0x03;
const uint8_t FT_CAM_800x600 = 0x04;
const uint8_t FT_CAM_1024x768 = 0x05;
const uint8_t FT_CAM_1280x1024 = 0x06;
const uint8_t FT_CAM_1600x1200 = 0x07;

class CameraModule {

    public:

        void begin();
        void update();
        void sleep();
        void captureToSD();
        void captureAndSend();
        void incomingPacket(IPacket *packet);
        void checkMsg();
        PowerState &getState(){return powerState;}
        EventList<void (*)()> &getImgCompleteEvent() {return imageCompleteEvent;}

    private:
        PowerState powerState;
        ArduCAM cam = ArduCAM(OV2640, CAM_CS_PIN);
        void readFifoBurst(bool resume);
        bool addToBuff(int &val);
        bool capturing = false;
        bool bufferReady = false;
        bool imgCompleted = true;
        bool sdCard = false;
        int buff_i = 0;
        int temp = 0, temp_last = 0, length = 0;
        uint8_t buff[BUFF_MAX];
        IPacket *packet;
        EventList<void (*)()> imageCompleteEvent;
        int resolution = OV2640_160x120;

};

#endif