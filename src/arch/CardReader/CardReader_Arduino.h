//
// Created by arc on 9/29/19.
//

#ifndef STEPMANIA_CARDREADER_ARDUINO_H

#define STEPMANIA_CARDREADER_ARDUINO_H

class CardReader_Arduino
{
    int fd;
    char * buffer;

public:
    void Update();
    CardReader_Arduino();
};
//I know this should be changed to a generic reader eventually...
extern CardReader_Arduino* CARDREADER;

#endif //STEPMANIA_CARDREADER_ARDUINO_H



