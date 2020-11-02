#ifndef MANUAL_MODE_H
#define MANUAL_MODE_H

#include "api/IUtils.h"

static const int LISTEN_WINDOW_TIME = 20; // in s
static const int LISTEN_SYNC_TIME = 60; // in s
static const int SLEEP_TIME = 120; // in s

class ManualMode {

    public:
        
        void start();

        void stop();

        void update();

        static void onImageCompleted();

        static void onPreReceive(IPacket *packet);

        static void onPartyEnd();

        static void syncExpiredISR();

        static void extendTimeout(int hours, int minutes, int seconds);

        void syncExpired();

        void sleepEnd();

    private:

        static bool synced;

};

#endif