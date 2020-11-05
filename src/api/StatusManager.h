#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include "IUtils.h"

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