//
// Created by gaming on 6/16/21.
//

#include "global.h"
#include "LightsDriver_WS2812B.h"
#include "ScreenManager.h"
#include "PrefsManager.h"
#include "RageLog.h"
#include "GameState.h"

REGISTER_LIGHTS_DRIVER_CLASS(WS2812B);

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(_WINDOWS)
static Preference<RString> m_ARDUINO_COM_PORT("ArduinoComPort", "COM1"); //default on windows
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
        if (arduino.available())
        {
            arduino.open();
        }
        else
        {
            LOG->Warn("The arduino is not connected. WS2812B lights cannot start.");
        }
    }
}

void LightsDriver_WS2812B::Set( const LightsState *ls)
{
    /*
     * we don't need the lights state at all because we don't use it like SM lights,
     * but since Set() is called every time the main thread calls update() it's good
     * way to update the LEDs.
     */
    if (!arduino.isOpen())
    {
        LOG->Trace("Arduino is not open!!");
        return;
        //arduino.open();
    }
    lightBuffer[0]=LM_REGULAR;
    if (LIGHTSMAN->GetLightsMode() == LIGHTSMODE_GAMEPLAY)
    {

        lightBuffer[2]=1; //r
        lightBuffer[3]=1; //g
        lightBuffer[4]=1; //b

        if (GAMESTATE->GetNumSidesJoined() > 1)
        {
            for (int i=0;i<NUM_PLAYERS;i++)
            {
                //It was actually faster just to have the arduino handle rainbow lights... So that's mode 3.
                lightBuffer[0]=LIGHTSMAN->b_LightsAreRainbow[i] ? LM_RAINBOW : LM_REGULAR;
                lightBuffer[1]=1+i; //player
                lightBuffer[5]=clamp((int)(LIGHTSMAN->m_LightSpirePercentage[i]*255),1,255);

                arduino.write(lightBuffer,6);

                //LOG->Trace("%i",lightBuffer[5]);
            }
        }
        else
        {
            lightBuffer[0]=LIGHTSMAN->b_LightsAreRainbow[GAMESTATE->GetMasterPlayerNumber()] ? LM_RAINBOW : LM_REGULAR;
            lightBuffer[5]=clamp((int)(LIGHTSMAN->m_LightSpirePercentage[GAMESTATE->GetMasterPlayerNumber()]*255),1,255);
            //LOG->Trace("%i",lightBuffer[5]);
            //LOG->Trace("%i %i %i",lightBuffer[2],lightBuffer[3],lightBuffer[4]);
            for (int i=0;i<NUM_PLAYERS;i++)
            {
                lightBuffer[1]=1+i; //player
                arduino.write(lightBuffer,6);
            }
        }


    }
    else
    {
        if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT] && ls->m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT])
            lightBuffer[5]=255;
        else if(ls->m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT])
        {
            lightBuffer[0]=LM_REVERSED;
            lightBuffer[5]=128;
        }
        else if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT])
        {
            lightBuffer[5]=128;
        }
        else
        {
            lightBuffer[5]=0;
        }
        lightBuffer[1]=1; //Left spire
        arduino.write(lightBuffer,6);


        if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT] && ls->m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT])
            lightBuffer[5]=255;
        else if(ls->m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT])
        {
            lightBuffer[0]=LM_REVERSED;
            lightBuffer[5]=128;
        }
        else if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT])
        {
            lightBuffer[5]=128;
        }
        else
        {
            lightBuffer[5]=0;
        }
        lightBuffer[1]=2; //Right spire
        arduino.write(lightBuffer,6);
    }
}

/*  (c) 2021 Rhythm Lunatic.

    This file is part of StepAMWorks.

    StepAMWorks is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    StepAMWorks is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with StepAMWorks.  If not, see <https://www.gnu.org/licenses/>.
*/