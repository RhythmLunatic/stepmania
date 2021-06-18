//
// Created by gaming on 6/16/21.
//

#ifndef STEPMANIA_LIGHTSDRIVER_WS2812B_H
#define STEPMANIA_LIGHTSDRIVER_WS2812B_H
#include "LightsDriver.h"
#include "arch/COM/serial.h"
#include "Preference.h"

enum ArduinoLightModes
{
    LM_NONE,
    LM_TRAVEL, //Travel along the LED strip, percentage is how far along to travel
    LM_REGULAR, //Regular, fill from the bottom
    LM_REVERSED, //Fill from the top
    LM_RAINBOW, //Regular + Ignore RGB values and use a rainbow effect
    LM_BLINKRAPIDLY, //Blink the spire rapidly (It was easier to implement on the arduino side)
    LM_EVERYOTHER, //Light up every other LED (evens)
    LM_EVERYOTHER2, //Light up every other LED (odds)
    LM_MANUAL //There's no way of knowing how many LEDs the arduino has...
};

class LightsDriver_WS2812B : public LightsDriver
{
public:
    LightsDriver_WS2812B();
    virtual ~LightsDriver_WS2812B() = default;

    virtual void Set( const LightsState *ls);
private:
    static serial::Serial arduino;
    uint8_t lightBuffer[6];
    unsigned char curRGBVal = 0;

};


#endif //STEPMANIA_LIGHTSDRIVER_WS2812B_H
