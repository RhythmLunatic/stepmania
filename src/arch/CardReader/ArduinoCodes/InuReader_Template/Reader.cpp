#include "Reader.h"

//This is a Template reader class 

void ReaderClass::begin()
{	
	//do initial here like Serial1.begin()...
	delay(10);
}


bool ReaderClass::DetactCard()
{
	//Read card ,then call SendCard(CardID,CardID_Length)
	
	//For example
	//
	//if (ReceiveCard)
	//{
	//	byte CardID[8]={1,2,3,4,5,6,7,8};
	//	SendCard(CardID,8);
	//}

}

ReaderClass Reader;

