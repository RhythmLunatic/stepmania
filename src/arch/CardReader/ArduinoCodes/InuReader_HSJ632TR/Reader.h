// Reader.h


//This is a custom reader class for HSJ632TR

#ifndef _Reader_h
#define _Reader_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



class ReaderClass
{
 protected:
 
 public:
	bool DetactCard();
	void begin();
};

extern ReaderClass Reader;
//Call this function to send card ID
extern void SendCard(byte* CardID,byte Length);

#endif

