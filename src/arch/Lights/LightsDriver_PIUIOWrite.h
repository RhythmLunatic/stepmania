#ifndef LightsDriver_PIUIOWrite_H
#define LightsDriver_PIUIOWrite_H

#include "arch/Lights/LightsDriver.h"

#define WINVER 0x0500

class LightsDriver_PIUIOWrite : public LightsDriver
{
public:
	LightsDriver_PIUIOWrite();
	virtual ~LightsDriver_PIUIOWrite();

	virtual void Set( const LightsState *ls );
//private:
//	int fd;
};

#endif
