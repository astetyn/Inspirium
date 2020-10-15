#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

//#include "libraries/L_ArduCAM.h"
#include "ArduCAM.h"
#include "PowerState.h"
#include "stdint.h"

static const int CAM_CS_PIN = 38;
static const int BUFF_MAX = 64;

class CameraModule {

    public:

        void begin();
        void update();
        void idle();
        void wakeUp();
        void sleep();
        void captureToSD();
        void captureAndSend();
        PowerState &getState(){return powerState;}

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

};

#endif