#include "Reader.h"

//This is a custom reader class for HSJ632TR

#define	READER_PACKET_LENGTH 5
#define CARDID_LENGTH 4


void ReaderClass::begin()
{	
	Serial1.begin(19200);
	delay(10);
}


bool ReaderClass::DetactCard()
{	
	if (Serial1.available() >= READER_PACKET_LENGTH)
	{
		byte Length=Serial1.available();
		
		byte *RxData = new byte[Length];
		
		for(byte b=0;b<Length;b++)
		{
			//read all data in buffer
			RxData[b]=Serial1.read();
		}
		
		//Check through the data, find correct card ID
		for (byte Index = 0; Index < Length - READER_PACKET_LENGTH+1; Index++)
		{
		
			byte CRC = (byte)(RxData[Index] ^ RxData[Index+1] ^ RxData[Index+2] ^ RxData[Index+3]);

			if (RxData[Index + 4] == CRC)
			{
				//correct card ID found , send it to serial
				byte* CardID = new byte[CARDID_LENGTH];
				for(byte i=0;i<CARDID_LENGTH;i++)
				{
					CardID[i]=RxData[Index+i];
				}
				SendCard(CardID,CARDID_LENGTH);
				delete[] CardID;
				break;
			}

		   
		}


	   delete[] RxData;
	}

}

ReaderClass Reader;

