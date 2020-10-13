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
        void slowUpdate();
        void idle();
        void wakeUp();
        void sleep();
        void captureToSD();
        void captureAndSend();
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        ArduCAM cam = ArduCAM(OV2640, CAM_CS_PIN);
        void readFifoBurst();
        void addToBuff(uint8_t val);
        void completeBuff();
        bool capturing = false;
        bool sdCard = false;
        int buff_i = 2;
        uint8_t buff[BUFF_MAX];

};

#endif