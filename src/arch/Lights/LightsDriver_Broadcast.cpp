// It's like LightsDriver_SystemMessage but it skips the SystemMessage part.

#include "global.h"
#include "LightsDriver_Broadcast.h"
#include "ScreenManager.h"
#include "InputMapper.h"
#include "PrefsManager.h"
#include "MessageManager.h"

REGISTER_LIGHTS_DRIVER_CLASS(Broadcast);


void LightsDriver_Broadcast::Set( const LightsState *ls )
{
    if (!PREFSMAN || !LIGHTSMAN || !SCREENMAN)
    {
        return;
    }

    if( !PREFSMAN->m_bDebugLights )
        return;

    Message msg("LightsDebug");
    msg.SetParam("LightsMode",LightsModeToString(LIGHTSMAN->GetLightsMode()));

    std::vector<bool> cabinetLights;
    FOREACH_CabinetLight(cl)
    {
        cabinetLights.push_back(ls->m_bCabinetLights[cl]);
    }
    msg.SetParam("Cabinet",cabinetLights);

    FOREACH_ENUM( GameController,  gc )
    {
        std::vector<bool> playerLights;
        FOREACH_ENUM( GameButton,  gb )
        {
            playerLights.push_back(ls->m_bGameButtonLights[gc][gb]);
        }
        msg.SetParam(ssprintf("Controller%d",gc+1),playerLights);
    }
    MESSAGEMAN->Broadcast(msg);
}

/*
 * (c) 2020 Rhythm Lunatic
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */