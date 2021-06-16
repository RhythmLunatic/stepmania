//
// Created by gaming on 6/16/21.
//

#include "global.h"
#include "LightsDriver_WS2812B.h"
#include "ScreenManager.h"
#include "PrefsManager.h"
#include "RageLog.h"

REGISTER_LIGHTS_DRIVER_CLASS(WS2812B);

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(_WINDOWS)
static Preference<RString> m_EXTIO_COMPORT("ArduinoComPort", "COM1"); //default on windows
#else
static Preference<RString> m_ARDUINO_COM_PORT("ArduinoComPort", "/dev/ttyACM0"); //default on *nix?
#endif

//Because C++ makes sense right?
serial::Serial LightsDriver_WS2812B::arduino;

LightsDriver_WS2812B::LightsDriver_WS2812B() {
    /*for (int i =0;i<6;i++)
        lightBuffer[i]=0x0;*/
    lightBuffer[0]=2; //setLED type packet
    lightBuffer[1]=1; //player
    lightBuffer[2]=1; //r
    lightBuffer[3]=1; //g
    lightBuffer[4]=1; //b

    if(m_ARDUINO_COM_PORT.Get().length() > 1)
    {
        arduino.setPort(m_ARDUINO_COM_PORT.Get().c_str());
        arduino.setBaudrate(9600);
        static serial::Timeout timeout;
        timeout=serial::Timeout::simpleTimeout(1000);
        arduino.setTimeout(timeout);
        arduino.open();
    }
}
void LightsDriver_WS2812B::Set( const LightsState *ls)
{
    /*
     * we don't need the lights state at all because we don't use it like SM lights,
     * but since Set() is called every time the main thread calls update() it's good
     * way to update the LEDs.
     */

    if (LIGHTSMAN->GetLightsMode() == LIGHTSMODE_GAMEPLAY)
    {
        if (!arduino.isOpen())
        {
            LOG->Trace("Arduino is not open");
            return;
            //arduino.open();
        }

        lightBuffer[0]=2; //setLED type packet
        lightBuffer[1]=1; //player
        lightBuffer[2]=1; //r
        lightBuffer[3]=1; //g
        lightBuffer[4]=1; //b
        //COBS when?
        //LOG->Trace("%f",LIGHTSMAN->m_LightSpirePercentage[0]);
        lightBuffer[5]=clamp((int)(LIGHTSMAN->m_LightSpirePercentage[0]*255),1,255);
        //LOG->Trace("%i",lightBuffer[5]);
        arduino.write(lightBuffer,6);
    }
    else
    {
        lightBuffer[2]=10; //r
        lightBuffer[5]=255; //make it full
        arduino.write(lightBuffer,6);
    }
}