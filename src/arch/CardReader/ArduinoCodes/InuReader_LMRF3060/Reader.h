// Reader.h

//This is a custom reader class for LMRF3060

#ifndef _Reader_h
#define _Reader_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MAX_CARDID_LENGTH 8
#define SerialBufferLength 64
#include "Queue.h"

class ReaderClass
{
 protected:
	CircularQueue<byte> SerialQueue;
	bool ReadingCard=false;
 public:
	ReaderClass(): SerialQueue(SerialBufferLength){}
	void begin();
	bool DetactCard();
	
};

extern ReaderClass Reader;
//Call this function to send card ID
extern void SendCard(byte* CardID,byte Length);

#endif

