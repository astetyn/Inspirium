#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

//#include "libraries/L_ArduCAM.h"
#include "ArduCAM.h"
#include "api/PowerState.h"
#include "api/IUtils.h"
#include "api/TaskList.h"

static const int CAM_CS_PIN = 38;
static const int BUFF_MAX = 64;
static const uint8_t FT_CAM_CAPTURE = 0x00;
static const uint8_t FT_CAM_160x120 = 0x01;
static const uint8_t FT_CAM_320x240 = 0x02;
static const uint8_t FT_CAM_640x480 = 0x03;
static const uint8_t FT_CAM_800x600 = 0x04;
static const uint8_t FT_CAM_1024x768 = 0x05;
static const uint8_t FT_CAM_1600x1200 = 0x06;

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

};

#endif