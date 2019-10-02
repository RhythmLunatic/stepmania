/*
 * Created by Rhythm Lunatic on 9/29/19.
 * My arduino is connected to one of those RC522 thingies and flashed with a firmware
 * that writes the card ID in bytes to the serial port.
 * The first two bytes are just 'aaaa' so they're ignored. Rest is the card ID.
*/
//#include "RageTimer.h"
#include "global.h"
#include "CardReader_Arduino.h"
#include <fcntl.h>
#include <unistd.h>
//#include <iomanip>
#include <sstream>
//#include <iostream.h>
#include "RageLog.h"
#include "MessageManager.h"
#include "PlayerNumber.h"

//Make it global and accessible everywhere.
CardReader_Arduino* CARDREADER = nullptr;

CardReader_Arduino::CardReader_Arduino()
{
    fd = open("/dev/ttyUSB0", O_RDONLY);
    if (fd == -1)
    {
        //RageException::Throw("open(\"/dev/ttyUSB0\"): %s", strerror(errno));
        LOG->Warn("COM port not found, card reader will not function.");
    }
    buffer = new char [11];
    m_LastTimeScanned = 0; //Init
    LOG->Trace("Card reader opened.");
}

void CardReader_Arduino::Update() {
    //Don't check card if card reader isn't connected..
    if (fd == -1)
        return;

    while (true)
    {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

        struct timeval zero = {0,0};
        //Yeah so turns out this breaks Aime cards because polling the serial port like this is stupid
        if ( select(fd+1, &fdset, NULL, NULL, &zero) <= 0 )
            return;


        //You actually have to clear the serial port no matter what
        read(fd, buffer, 11);

        //Not sure if I should be using a RageTimer type instead.
        int scanTime = (int)RageTimer::GetTimeSinceStart();
        if (m_LastTimeScanned+5 > scanTime )
        {
            //LOG->Warn("[%i][%i] Scan times were within 5 seconds of each other, ignoring them.",m_LastTimeScanned, scanTime);
            return;
        }
        else
        {
            m_LastTimeScanned = scanTime;
            stringstream hexStr;
            for (int i = 2; i < 11; i++)
            {
                int b = (int)buffer[i];
                if (b < 0)
                    b = 256+b;
                hexStr << std::hex << b;
            }
            LOG->Warn("[%i] Card scanned: %s",scanTime, hexStr.str().c_str());
            Message msg("CardScanned");
            msg.SetParam("Player", PLAYER_1);
            //Ironically you can't put a char array in a message.
            //The message command never sends if you don't wrap it in RString().
            msg.SetParam("card",RString(hexStr.str()));
            MESSAGEMAN->Broadcast(msg);
        }

    }

}