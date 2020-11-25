#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include "IUtils.h"

const uint8_t FT_STATUS_MAIN = 0x00;
const uint8_t FT_STATUS_WEATHER = 0x01;
const uint8_t FT_STATUS_BATT = 0x02;

class StatusManager {

    public:

        void update();

        void sendStatusMain();

        void sendStatusWeather();

        void sendStatusBatt();

        void incomingPacket(IPacket *packet);

        void checkMsg();

    private:

        IPacket *packet = 0;

};

#endif