#include "myMFRC522.h"
#include "Keyboard.h"
#include "Keypad.h"
#include "Key.h"
// all includes above are library, DO NOT edit them

#include "Reader.h"
#include "CustomKeypad.h"

#define MAX_CARDID_LENGTH 8


void setup(){
  delay(100);
  
  Keyboard.begin();
  Serial.begin(9600);
  
  delay(100);
  Reader.begin();
}
  
void loop(){
	Reader.DetactCard();
	CustomKeypad.DetactInput();
}

//Call this function to send card ID
void SendCard(byte* CardID,byte Length)
{
	if(Length>MAX_CARDID_LENGTH)
	Length=MAX_CARDID_LENGTH;

	byte CRC=0;
	byte TxData[MAX_CARDID_LENGTH] ={0};
	
	for(byte b=0;b<Length;b++)
	{
		//copy data
		TxData[b] = CardID[b];
	}
	
	//Send card ID 
	Serial.write(0xaa);
	CRC+=0xaa;
	Serial.write(0xaa);
	CRC+=0xaa;
	
	for(byte b=0;b<MAX_CARDID_LENGTH;b++)
	{
		CRC+=TxData[b];
		Serial.write(TxData[b]);
	}
	
	Serial.write(~CRC);

}
