#ifndef LightsDriver_PIUIOKey_H
#define LightsDriver_PIUIOKey_H

#include "arch/Lights/LightsDriver.h"

#define WINVER 0x0500

class LightsDriver_PIUIOKey : public LightsDriver
{
public:
	LightsDriver_PIUIOKey();
	virtual ~LightsDriver_PIUIOKey();

	virtual void Set( const LightsState *ls );
//private:
//	int fd;
};

#endif
