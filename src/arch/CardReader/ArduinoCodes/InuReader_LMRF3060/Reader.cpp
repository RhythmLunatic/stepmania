#include "Reader.h"

//This is a custom reader class for LMRF3060

char NormalEcho[] = {0x44,0x52,0x20,0x56,0x31,0x2E,0x31,0x20,0x3E,0x0D,0x0A,'\0'};
char LineFeed[] = {0x0D,0x0A,'\0'};
char GetCardIDMessage[] = {0x49,0x44,0x3A,0x0D,0x0A,'\0'};


void ReaderClass::begin()
{	
	Serial1.begin(115200);
	//Send 'B' to LMRF3060 will mute the speaker on it.
	//delay(10);
	//Serial1.write('B');
	//Serial1.print(LineFeed);
	//delay(500);
	
	//Send 'A' to LMRF3060 will enter auto read mode.
	Serial1.write('A');
	Serial1.print(LineFeed);
	delay(500);
	
}

bool ReaderClass::DetactCard()
{	
	while(Serial1.available())
	{
		if(SerialQueue.isFull())
			SerialQueue.Dequeue();
		SerialQueue.Enqueue(Serial1.read());
	}
	
	if(ReadingCard)
	{
		//Find LineFeed 0x0D 0x0A
		int CardIDLength = SerialQueue.IndexOf((byte*)LineFeed,strlen(LineFeed));	
		//Don't know why but sometimes LMRF3060 return 1 byte card ID (2 bytes in ASCII format),which is obviously wrong.
		//Often occours when card is too far away from LMRF3060.
		//So limit it to minimum length of 8 bytes (Minimum of MIFARE standard) plus 1 byte new line
		//ISO15693 card seems to return 20 byte card ID plus 1 byte new line,so set maximum to 21bytes
		if (CardIDLength >= 9 && CardIDLength<=21)
		{
			byte* CardID_ASCII=new byte[CardIDLength];
			for(byte b=0;b<CardIDLength;b++)
			{
				CardID_ASCII[b]=SerialQueue.Pop();				
			}
			
			if(CardID_ASCII[CardIDLength-1]==0x0A)
			{
				//Convert ASCII to byte value
				
				byte CardIDLength_Byte=CardIDLength>>1;// equals to div 2
				byte CardID_Byte[MAX_CARDID_LENGTH]{0};	
				
				//Because maximum card ID Length is 8 bytes , abandon some bytes larger than that.
				//Eamuse seems to return 10 bytes ID ,but first 2 bytes is 0x00 ,so abandon from begining.
				byte offset = (CardIDLength_Byte>MAX_CARDID_LENGTH ? CardIDLength_Byte - MAX_CARDID_LENGTH : 0)<<1 ;//equals to mul 2
				byte* CardID_ASCII_offset=CardID_ASCII+offset;
				
				if(CardIDLength_Byte > MAX_CARDID_LENGTH)
					CardIDLength_Byte=MAX_CARDID_LENGTH;
				
				for (byte b=0; b< CardIDLength_Byte; b++)
				{
					byte High4bit=CardID_ASCII_offset[b*2],Low4bit=CardID_ASCII_offset[b*2+1];
					
					High4bit = (High4bit >= 'A') ? High4bit - 'A' + 10 : High4bit - '0';
					Low4bit = (Low4bit >= 'A') ? Low4bit - 'A' + 10 : Low4bit - '0';
					
					CardID_Byte[b] = (High4bit<<4) + (Low4bit);
				}
				
				SendCard(CardID_Byte,CardIDLength_Byte);
			}
			
			delete []CardID_ASCII; 
			ReadingCard=false;
			SerialQueue.Clear();
		}
		else if(CardIDLength>21)
		{
			//error
			ReadingCard=false;
			SerialQueue.Clear();
		}
			
	}
	else
	{
		//Keep searching new card
		int SearchResult = SerialQueue.IndexOf((byte*)GetCardIDMessage,strlen(GetCardIDMessage));
		if (SearchResult >0)
		{
			//Found card
			ReadingCard=true;
			for(byte b=0;b<SearchResult+strlen(GetCardIDMessage);b++)
				SerialQueue.Dequeue();
		}
	}
	
	
	return false;
}


ReaderClass Reader;

