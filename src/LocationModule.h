#ifndef LOCATION_MODULE_H
#define LOCATION_MODULE_H

#include "stdint.h"
#include "PowerState.h"
#include "delay.h"

#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MAX_FIELD_SIZE 15

#define INT_MAX 2147483647

//naposledy som mazal slova "public" ak by to nefungovalo

struct RawDegrees {
    int deg;
    int billionths;
    bool negative;
    RawDegrees() : deg(0), billionths(0), negative(false){}
};

struct TinyGPSLocation {

    friend class GNSSEncoder;

    bool isValid() const    { return valid; }
    bool isUpdated() const  { return updated; }
    int age() const    { return valid ? millis() - lastCommitTime : (int)INT_MAX; }
    const RawDegrees &rawLat()     { updated = false; return rawLatData; }
    const RawDegrees &rawLng()     { updated = false; return rawLngData; }
    double lat();
    double lng();

    TinyGPSLocation() : valid(false), updated(false){}

    private:
        bool valid, updated;
        RawDegrees rawLatData, rawLngData, rawNewLatData, rawNewLngData;
        int lastCommitTime;
        void commit();
        void setLatitude(const char *term);
        void setLongitude(const char *term);
};

struct TinyGPSDate {

    friend class GNSSEncoder;

    bool isValid() const       { return valid; }
    bool isUpdated() const     { return updated; }
    int age() const       { return valid ? millis() - lastCommitTime : (int)INT_MAX; }

    int value()           { updated = false; return date; }
    int year();
    int month();
    int day();

    TinyGPSDate() : valid(false), updated(false), date(0){}

    private:
        bool valid, updated;
        int date, newDate;
        int lastCommitTime;
        void commit();
        void setDate(const char *term);
};

struct TinyGPSTime {

    friend class GNSSEncoder;

    bool isValid() const       { return valid; }
    bool isUpdated() const     { return updated; }
    int age() const       { return valid ? millis() - lastCommitTime : (int)INT_MAX; }

    int value()           { updated = false; return time; }
    int hour();
    int minute();
    int second();

    TinyGPSTime() : valid(false), updated(false), time(0){}

    private:
        bool valid, updated;
        int time, newTime;
        int lastCommitTime;
        void commit();
        void setTime(const char *term);
};

struct TinyGPSDecimal {

    friend class GNSSEncoder;

    bool isValid() const    { return valid; }
    bool isUpdated() const  { return updated; }
    int age() const    { return valid ? millis() - lastCommitTime : (int)INT_MAX; }
    int value()         { updated = false; return val; }

    TinyGPSDecimal() : valid(false), updated(false), val(0)
    {}

    private:
        bool valid, updated;
        int lastCommitTime;
        int val, newval;
        void commit();
        void set(const char *term);
};

struct TinyGPSInteger {

    friend class GNSSEncoder;

    bool isValid() const    { return valid; }
    bool isUpdated() const  { return updated; }
    int age() const    { return valid ? millis() - lastCommitTime : (int)INT_MAX; }
    int value()        { updated = false; return val; }

    TinyGPSInteger() : valid(false), updated(false), val(0)
    {}

    private:
        bool valid, updated;
        int lastCommitTime;
        int val, newval;
        void commit();
        void set(const char *term);
};

struct TinyGPSSpeed : TinyGPSDecimal {
   double kmph()     { return _GPS_KMPH_PER_KNOT * value() / 100.0; }
};

struct TinyGPSCourse : public TinyGPSDecimal {
   double deg()      { return value() / 100.0; }
};

struct TinyGPSAltitude : TinyGPSDecimal {
   double get()       { return value() / 100.0; }
};

struct TinyGPSHDOP : TinyGPSDecimal {
   double hdop() { return value() / 100.0; }
};

class GNSSEncoder {

    public:

        GNSSEncoder();
        bool encode(char c); // process one character received from GPS
        GNSSEncoder &operator << (char c) {encode(c); return *this;}

        TinyGPSLocation location;
        TinyGPSDate date;
        TinyGPSTime time;
        TinyGPSSpeed speed;
        TinyGPSCourse course;
        TinyGPSAltitude altitude;
        TinyGPSInteger satellites;
        TinyGPSHDOP hdop;

        static double distanceBetween(double lat1, double long1, double lat2, double long2);
        static double courseTo(double lat1, double long1, double lat2, double long2);

        static int parseDecimal(const char *term);
        static void parseDegrees(const char *term, RawDegrees &deg);

        int charsProcessed()   const { return encodedCharCount; }
        int sentencesWithFix() const { return sentencesWithFixCount; }
        int failedChecksum()   const { return failedChecksumCount; }
        int passedChecksum()   const { return passedChecksumCount; }

    private:

        enum {GPS_SENTENCE_GPGGA, GPS_SENTENCE_GPRMC, GPS_SENTENCE_OTHER};

        // parsing state variables
        uint8_t parity;
        bool isChecksumTerm;
        char term[_GPS_MAX_FIELD_SIZE];
        uint8_t curSentenceType;
        uint8_t curTermNumber;
        uint8_t curTermOffset;
        bool sentenceHasFix;

        // statistics
        int encodedCharCount;
        int sentencesWithFixCount;
        int failedChecksumCount;
        int passedChecksumCount;

        // internal utilities
        int fromHex(char a);
        bool endOfTermHandler();
};

class LocationModule {

    public:

        void begin();
        void update();
        void idle();
        void wakeUp();
        void sleep();
        double getLat() {return gnss.location.lat();}
        double getLng() {return gnss.location.lng();}
        double getAlt() {return gnss.altitude.get();}
        PowerState &getState(){return powerState;}

    private:
        PowerState powerState;
        GNSSEncoder gnss = GNSSEncoder();

};

#endif
