//This is a MFRC552 reader class 

/*
 * Typical pin layout used:
 * --------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino          Arduino
 *             Reader/PCD   Uno           Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin              Pin
 * --------------------------------------------------------------------
 * RST/Reset   RST          A1            RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      A0            A0               A0
 * SPI MOSI    MOSI         11 / ICSP-4   ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   ICSP-3           15
 */

#include "Reader.h"

#include <SPI.h>
#include "myMFRC522.h"

#define RST_PIN		A1		 
#define SS_PIN		A0		
#define SCAN_INTERVAL 50 //Scan every 50ms

MFRC522 mfrc522(SS_PIN, RST_PIN);	
unsigned long LastTimeScanned = 0;

void ReaderClass::begin()
{	
	SPI.begin();
	delay(10);	
	mfrc522.PCD_Init();
	delay(10);
}


bool ReaderClass::DetactCard()
{
	if(millis()-LastTimeScanned >= SCAN_INTERVAL)
	{
		LastTimeScanned=millis();
		if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
		{
			SendCard(mfrc522.uid.uidByte,mfrc522.uid.size);
			return true;
		}
	}
	return false;
}

ReaderClass Reader;

