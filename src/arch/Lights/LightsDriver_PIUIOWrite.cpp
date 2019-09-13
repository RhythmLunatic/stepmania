#include "global.h"
#include "LightsDriver_PIUIOWrite.h"
#include "windows.h"
#include "RageUtil.h"
#include "GameState.h"
#include "Game.h"
#include "RageLog.h"

#include <iostream>
#include <fstream>

REGISTER_LIGHTS_DRIVER_CLASS(PIUIOWrite);

LightsDriver_PIUIOWrite::LightsDriver_PIUIOWrite()
{
}

LightsDriver_PIUIOWrite::~LightsDriver_PIUIOWrite()
{
}

void LightsDriver_PIUIOWrite::Set( const LightsState *ls )
{
	// OutputDebugString("Setting lights...\r\n");
	// unsigned int addr[15] = { 0x52, 0x54, 0x59, 0x55, 0x49, 0x46, 0x47, 0x4a, 0x4b, 0x4c, 0x56, 0x42, 0x4e, 0x4d };
	static bool oldbuf[15] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	bool buf[15] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	// 0b1000000000000000
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT]) buf[10] = true;
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT]) buf[11] = true;
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT]) buf[12] = true;
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT]) buf[13] = true;
	if (ls->m_bCabinetLights[LIGHT_BASS_LEFT] || ls->m_bCabinetLights[LIGHT_BASS_RIGHT]) buf[14] = true;

	RString sInput = GAMESTATE->GetCurrentGame()->m_InputScheme.m_szName;
	if (sInput.EqualsNoCase("dance")) {
		if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_UP]) buf[0] = true;
		if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_DOWN]) buf[1] = true;
		if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_LEFT]) buf[2] = true;
		if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_RIGHT]) buf[3] = true;

		if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_UP]) buf[5] = true;
		if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_DOWN]) buf[6] = true;
		if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_LEFT]) buf[7] = true;
		if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_RIGHT]) buf[8] = true;
	}
	else if (sInput.EqualsNoCase("pump")) {
		if (ls->m_bGameButtonLights[GameController_1][PUMP_BUTTON_UPLEFT]) buf[0] = true;
		if (ls->m_bGameButtonLights[GameController_1][PUMP_BUTTON_UPRIGHT]) buf[1] = true;
		if (ls->m_bGameButtonLights[GameController_1][PUMP_BUTTON_CENTER]) buf[2] = true;
		if (ls->m_bGameButtonLights[GameController_1][PUMP_BUTTON_DOWNLEFT]) buf[3] = true;
		if (ls->m_bGameButtonLights[GameController_1][PUMP_BUTTON_DOWNRIGHT]) buf[4] = true;

		if (ls->m_bGameButtonLights[GameController_2][PUMP_BUTTON_UPLEFT]) buf[5] = true;
		if (ls->m_bGameButtonLights[GameController_2][PUMP_BUTTON_UPRIGHT]) buf[6] = true;
		if (ls->m_bGameButtonLights[GameController_2][PUMP_BUTTON_CENTER]) buf[7] = true;
		if (ls->m_bGameButtonLights[GameController_2][PUMP_BUTTON_DOWNLEFT]) buf[8] = true;
		if (ls->m_bGameButtonLights[GameController_2][PUMP_BUTTON_DOWNRIGHT]) buf[9] = true;
	}
	if (!memcmp(buf, oldbuf, sizeof buf))
		return;
	memcpy(oldbuf, buf, sizeof buf);

	ofstream myfile;
	myfile.open("lights.txt", ios::out | ios::trunc);
	for (int i = 0; i < 15; i++) {
		myfile << buf[i] << "\n";
	}
	myfile.close();
}

/*
 * (c) 2012-2013 StepMania team
 * (c) 2018 BedrockSolid
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
