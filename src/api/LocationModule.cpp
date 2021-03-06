#include "api/LocationModule.h"

#include "api/API.h"

#include "Arduino.h"

#define _GPRMCterm "GPRMC"
#define _GPGGAterm "GPGGA"
#define _GNRMCterm "GNRMC"
#define _GNGGAterm "GNGGA"

void LocationModule::begin() {

    Serial1.begin(9600);

    powerState = PowerState::ACTIVE;
}

void LocationModule::update() {

    if(powerState != PowerState::ACTIVE) return;

    while(Serial1.available() > 0){
        gnss.encode(Serial1.read());
    }
    /*SerialUSB.println(gnss.satellites.value());
    SerialUSB.println(gnss.location.lat());
    SerialUSB.println(gnss.location.lng());
    SerialUSB.println(gnss.altitude.meters());
    SerialUSB.println(gnss.satellites.value());
    SerialUSB.println(gnss.satellites.value());*/
}

void LocationModule::sleep() {

    uint8_t sleepArr [16] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x10, 0x27, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x84, 0xCC};

    Serial1.write(sleepArr, 16);

    Serial1.end();

    // RX+TX PINS NEO
    pinMode(0, INPUT_PULLDOWN);
    pinMode(1, INPUT_PULLDOWN);

    powerState = PowerState::SLEEPING;

}

GNSSEncoder::GNSSEncoder()
  :  parity(0)
  ,  isChecksumTerm(false)
  ,  curSentenceType(GPS_SENTENCE_OTHER)
  ,  curTermNumber(0)
  ,  curTermOffset(0)
  ,  sentenceHasFix(false)
  ,  encodedCharCount(0)
  ,  sentencesWithFixCount(0)
  ,  failedChecksumCount(0)
  ,  passedChecksumCount(0) {
  term[0] = '\0';
}

//
// public methods
//

bool GNSSEncoder::encode(char c) {

    encodedCharCount++;

    switch(c) {
    case ',': // term terminators
        parity ^= (uint8_t)c;
    case '\r':
    case '\n':
    case '*': {
        bool isValidSentence = false;
        if (curTermOffset < sizeof(term)) {
            term[curTermOffset] = 0;
            isValidSentence = endOfTermHandler();
        }
        ++curTermNumber;
        curTermOffset = 0;
        isChecksumTerm = c == '*';
        return isValidSentence;
    }break;

    case '$': // sentence begin
        curTermNumber = curTermOffset = 0;
        parity = 0;
        curSentenceType = GPS_SENTENCE_OTHER;
        isChecksumTerm = false;
        sentenceHasFix = false;
        return false;

    default: // ordinary characters
        if (curTermOffset < sizeof(term) - 1)
        term[curTermOffset++] = c;
        if (!isChecksumTerm)
        parity ^= c;
        return false;
    }
    return false;
}

//
// internal utilities
//
int GNSSEncoder::fromHex(char a) {

    if(a >= 'A' && a <= 'F'){
        return a - 'A' + 10;
    }else if(a >= 'a' && a <= 'f') {
        return a - 'a' + 10;
    }else{
        return a - '0';
    }

}

// static
// Parse a (potentially negative) number with up to 2 decimal digits -xxxx.yy
int GNSSEncoder::parseDecimal(const char *term) {
    bool negative = *term == '-';
    if(negative) ++term;
    int ret = 100 * (int)atol(term);
    while(isdigit(*term)) { 
        term++;
    }
    if(*term == '.' && isdigit(term[1])) {
        ret += 10 * (term[1] - '0');
        if(isdigit(term[2])) {
            ret += term[2] - '0';
        }
    }
    return negative ? -ret : ret;
}

// static
// Parse degrees in that funny NMEA format DDMM.MMMM
void GNSSEncoder::parseDegrees(const char *term, RawDegrees &deg) {

    int leftOfDecimal = (int)atol(term);
    int minutes = (int)(leftOfDecimal % 100);
    int multiplier = 10000000UL;
    int tenMillionthsOfMinutes = minutes * multiplier;

    deg.deg = (int)(leftOfDecimal / 100);

    while(isdigit(*term)) {
        term++;
    }

    if(*term == '.') {
        while(isdigit(*++term)) {
        multiplier /= 10;
        tenMillionthsOfMinutes += (*term - '0') * multiplier;
        }
    }

    deg.billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
    deg.negative = false;
}

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

// Processes a just-completed term
// Returns true if new sentence has just passed checksum test and is validated
bool GNSSEncoder::endOfTermHandler() {
    // If it's the checksum term, and the checksum checks out, commit
    if(isChecksumTerm) {
        byte checksum = 16 * fromHex(term[0]) + fromHex(term[1]);
        if(checksum == parity) {
            passedChecksumCount++;
            if(sentenceHasFix) {
                ++sentencesWithFixCount;
            }

            switch(curSentenceType)
            {
            case GPS_SENTENCE_GPRMC:
                date.commit();
                time.commit();
                if (sentenceHasFix)
                {
                location.commit();
                speed.commit();
                course.commit();
                }
                break;
            case GPS_SENTENCE_GPGGA:
                time.commit();
                if (sentenceHasFix)
                {
                location.commit();
                altitude.commit();
                }
                satellites.commit();
                hdop.commit();
                break;
            }
            return true;
        }else {
            failedChecksumCount++;
        }
        return false;
    }

    // the first term determines the sentence type
    if (curTermNumber == 0) {
        if (!strcmp(term, _GPRMCterm) || !strcmp(term, _GNRMCterm))
        curSentenceType = GPS_SENTENCE_GPRMC;
        else if (!strcmp(term, _GPGGAterm) || !strcmp(term, _GNGGAterm))
        curSentenceType = GPS_SENTENCE_GPGGA;
        else
        curSentenceType = GPS_SENTENCE_OTHER;

        return false;
    }

    if (curSentenceType != GPS_SENTENCE_OTHER && term[0]) {
        switch(COMBINE(curSentenceType, curTermNumber)) {
            case COMBINE(GPS_SENTENCE_GPRMC, 1): // Time in both sentences
            case COMBINE(GPS_SENTENCE_GPGGA, 1):
            time.setTime(term);
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 2): // GPRMC validity
            sentenceHasFix = term[0] == 'A';
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 3): // Latitude
            case COMBINE(GPS_SENTENCE_GPGGA, 2):
            location.setLatitude(term);
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 4): // N/S
            case COMBINE(GPS_SENTENCE_GPGGA, 3):
            location.rawNewLatData.negative = term[0] == 'S';
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 5): // Longitude
            case COMBINE(GPS_SENTENCE_GPGGA, 4):
            location.setLongitude(term);
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 6): // E/W
            case COMBINE(GPS_SENTENCE_GPGGA, 5):
            location.rawNewLngData.negative = term[0] == 'W';
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 7): // Speed (GPRMC)
            speed.set(term);
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 8): // Course (GPRMC)
            course.set(term);
            break;
            case COMBINE(GPS_SENTENCE_GPRMC, 9): // Date (GPRMC)
            date.setDate(term);
            break;
            case COMBINE(GPS_SENTENCE_GPGGA, 6): // Fix data (GPGGA)
            sentenceHasFix = term[0] > '0';
            break;
            case COMBINE(GPS_SENTENCE_GPGGA, 7): // Satellites used (GPGGA)
            satellites.set(term);
            break;
            case COMBINE(GPS_SENTENCE_GPGGA, 8): // HDOP
            hdop.set(term);
            break;
            case COMBINE(GPS_SENTENCE_GPGGA, 9): // Altitude (GPGGA)
            altitude.set(term);
            break;
        }
    }

    return false;
}

/* static */
double GNSSEncoder::distanceBetween(double lat1, double long1, double lat2, double long2) {
    // returns distance in meters between two positions, both specified
    // as signed decimal-degrees latitude and longitude. Uses great-circle
    // distance computation for hypothetical sphere of radius 6372795 meters.
    // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
    // Courtesy of Maarten Lamers
    double delta = radians(long1-long2);
    double sdlong = sin(delta);
    double cdlong = cos(delta);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = sq(delta);
    delta += sq(clat2 * sdlong);
    delta = sqrt(delta);
    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);
    return delta * 6372795;
}

double GNSSEncoder::courseTo(double lat1, double long1, double lat2, double long2) {
    // returns course in degrees (North=0, West=270) from position 1 to position 2,
    // both specified as signed decimal-degrees latitude and longitude.
    // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
    // Courtesy of Maarten Lamers
    double dlon = radians(long2-long1);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double a1 = sin(dlon) * cos(lat2);
    double a2 = sin(lat1) * cos(lat2) * cos(dlon);
    a2 = cos(lat1) * sin(lat2) - a2;
    a2 = atan2(a1, a2);
    if (a2 < 0.0) {
        a2 += TWO_PI;
    }
    return degrees(a2);
}

void TinyGPSLocation::commit() {
    rawLatData = rawNewLatData;
    rawLngData = rawNewLngData;
    lastCommitTime = millis();
    valid = updated = true;
}

void TinyGPSLocation::setLatitude(const char *term) {
    GNSSEncoder::parseDegrees(term, rawNewLatData);
}

void TinyGPSLocation::setLongitude(const char *term) {
    GNSSEncoder::parseDegrees(term, rawNewLngData);
}

double TinyGPSLocation::lat() {
    updated = false;
    double ret = rawLatData.deg + rawLatData.billionths / 1000000000.0;
    return rawLatData.negative ? -ret : ret;
}

double TinyGPSLocation::lng() {
    updated = false;
    double ret = rawLngData.deg + rawLngData.billionths / 1000000000.0;
    return rawLngData.negative ? -ret : ret;
}

void TinyGPSDate::commit() {
    date = newDate;
    lastCommitTime = millis();
    valid = updated = true;
}

void TinyGPSTime::commit() {
    time = newTime;
    lastCommitTime = millis();
    valid = updated = true;
}

void TinyGPSTime::setTime(const char *term) {
    newTime = (int)GNSSEncoder::parseDecimal(term);
}

void TinyGPSDate::setDate(const char *term) {
    newDate = atol(term);
}

int TinyGPSDate::year() {
    updated = false;
    int year = date % 100;
    return year + 2000;
}

int TinyGPSDate::month() {
    updated = false;
    return (date / 100) % 100;
}

int TinyGPSDate::day() {
    updated = false;
    return date / 10000;
}

int TinyGPSTime::hour() {
    updated = false;
    return time / 1000000;
}

int TinyGPSTime::minute() {
    updated = false;
    return (time / 10000) % 100;
}

int TinyGPSTime::second() {
    updated = false;
    return (time / 100) % 100;
}

void TinyGPSDecimal::commit() {
    val = newval;
    lastCommitTime = millis();
    valid = updated = true;
}

void TinyGPSDecimal::set(const char *term) {
    newval = GNSSEncoder::parseDecimal(term);
}

void TinyGPSInteger::commit() {
    val = newval;
    lastCommitTime = millis();
    valid = updated = true;
}

void TinyGPSInteger::set(const char *term) {
    newval = atol(term);
}
