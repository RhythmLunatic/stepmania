//
// Created by gaming on 6/16/21.
//

#ifndef STEPMANIA_LIGHTSDRIVER_WS2812B_H
#define STEPMANIA_LIGHTSDRIVER_WS2812B_H
#include "LightsDriver.h"
#include "arch/COM/serial.h"
#include "Preference.h"


class LightsDriver_WS2812B : public LightsDriver
{
public:
    LightsDriver_WS2812B();
    virtual ~LightsDriver_WS2812B() = default;

    virtual void Set( const LightsState *ls);
private:
    static serial::Serial arduino;
    uint8_t lightBuffer[6];

};


#endif //STEPMANIA_LIGHTSDRIVER_WS2812B_H
