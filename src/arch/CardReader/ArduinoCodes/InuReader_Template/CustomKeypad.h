// CustomKeypad.h


#ifndef _CUSTOMKEYPAD_h
#define _CUSTOMKEYPAD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



class CustomKeypadClass
{
 protected:
 
 public:
	void DetactInput();
};

extern CustomKeypadClass CustomKeypad;

#endif

