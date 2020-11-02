#ifndef INSPIRIUM_H
#define INSPIRIUM_H

#include "ai/ManualMode.h"
#include "ai/AutoMode.h"

enum class Mode {

    MANUAL,
    AUTO

};

class InspiriumClass {

    public:
        
        void begin();

        void update();

        void switchToManual();

        void switchToAuto();

    private:

        Mode mode;
        ManualMode manualMode;
        AutoMode autoMode;

};

extern InspiriumClass Inspi;

#endif