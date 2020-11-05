#ifndef STORAGE_MODULE_H
#define STORAGE_MODULE_H

#include "api/PowerState.h"

const int SD_CS_PIN = 2;

class StorageModule {

    public:

        void begin();
        void sleep();
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;

};

#endif