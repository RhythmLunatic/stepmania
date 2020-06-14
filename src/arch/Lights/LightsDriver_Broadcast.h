#ifndef STEPMANIA_LIGHTSDRIVER_BROADCAST_H
#define STEPMANIA_LIGHTSDRIVER_BROADCAST_H

#include "LightsDriver.h"

class LightsDriver_Broadcast : public LightsDriver
{
public:
    LightsDriver_Broadcast() = default;
    virtual ~LightsDriver_Broadcast() = default;

    virtual void Set( const LightsState *ls );
};

#endif //STEPMANIA_LIGHTSDRIVER_BROADCAST_H
