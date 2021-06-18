#ifndef LightsManager_H
#define LightsManager_H

#include "PlayerNumber.h"
#include "GameInput.h"
#include "EnumHelper.h"
#include "Preference.h"
#include "RageTimer.h"

extern Preference<float>	g_fLightsFalloffSeconds;
extern Preference<float>	g_fLightsAheadSeconds;

enum CabinetLight
{
	LIGHT_MARQUEE_UP_LEFT,
	LIGHT_MARQUEE_UP_RIGHT,
	LIGHT_MARQUEE_LR_LEFT,
	LIGHT_MARQUEE_LR_RIGHT,
	LIGHT_BASS_LEFT,
	LIGHT_BASS_RIGHT,
	NUM_CabinetLight,
	CabinetLight_Invalid
};
/** @brief Loop through each CabinetLight on the machine. */
#define FOREACH_CabinetLight( i ) FOREACH_ENUM( CabinetLight, i )
const RString& CabinetLightToString( CabinetLight cl );
CabinetLight StringToCabinetLight( const RString& s);

enum LightsMode
{
	LIGHTSMODE_ATTRACT,
	LIGHTSMODE_JOINING,
	LIGHTSMODE_MENU_START_ONLY,
	LIGHTSMODE_MENU_START_AND_DIRECTIONS,
	LIGHTSMODE_DEMONSTRATION,
	LIGHTSMODE_GAMEPLAY,
	LIGHTSMODE_STAGE,
	LIGHTSMODE_ALL_CLEARED,
	LIGHTSMODE_TEST_AUTO_CYCLE,
	LIGHTSMODE_TEST_MANUAL_CYCLE,
	NUM_LightsMode,
	LightsMode_Invalid
};
const RString& LightsModeToString( LightsMode lm );
LuaDeclareType( LightsMode );

struct RGBLight
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct LightsState
{
	bool m_bCabinetLights[NUM_CabinetLight];
	bool m_bGameButtonLights[NUM_GameController][NUM_GameButton];
	RGBLight m_rgbCabinetLights[NUM_CabinetLight];

	//note these use an MCU for patterns and intensity as well... for now... just solid colors
	RGBLight m_rgbSpires[7];//3 on left, one under the monitor, 3 on right, in order of left to right (p1->p2)

	// This isn't actually a light, but it's typically implemented in the same way.
	bool m_bCoinCounter;
};

class LightsDriver;
/** @brief Control lights. */
class LightsManager
{
public:
	LightsManager();
	~LightsManager();

	void Update( float fDeltaTime );
	bool IsEnabled() const;

	void BlinkCabinetLight( CabinetLight cl );
	void BlinkGameButton( GameInput gi );
	void BlinkActorLight( CabinetLight cl );
	void TurnOffAllLights();
	void PulseCoinCounter() { ++m_iQueuedCoinCounterPulses; }
	float GetActorLightLatencySeconds() const;

	void SetLightsMode( LightsMode lm );
	LightsMode GetLightsMode();

	void PrevTestCabinetLight()		{ ChangeTestCabinetLight(-1); }
	void NextTestCabinetLight()		{ ChangeTestCabinetLight(+1); }
	void PrevTestGameButtonLight()	{ ChangeTestGameButtonLight(-1); }
	void NextTestGameButtonLight()	{ ChangeTestGameButtonLight(+1); }

	CabinetLight	GetFirstLitCabinetLight();
	GameInput	GetFirstLitGameButtonLight();

	bool lightSpiresEnabled;
    float m_LightSpirePercentage[NUM_PLAYERS];
    bool b_LightsAreRainbow[NUM_PLAYERS]; //I don't know where to put this

private:
	void ChangeTestCabinetLight( int iDir );
	void ChangeTestGameButtonLight( int iDir );
	static int16_t upperCapAt(int16_t cap, int16_t var);
	static int16_t lowerCapAt(int16_t cap, int16_t var);
	void setRGBLightToSolid(RGBLight l, uint8_t lightColor);
	void updateRGBLights();

	float m_fSecsLeftInCabinetLightBlink[NUM_CabinetLight];
	float m_fSecsLeftInGameButtonBlink[NUM_GameController][NUM_GameButton];
	float m_fActorLights[NUM_CabinetLight];	// current "power" of each actor light
	float m_fSecsLeftInActorLightBlink[NUM_CabinetLight];	// duration to "power" an actor light

	vector<LightsDriver*> m_vpDrivers;
	LightsMode m_LightsMode;
	LightsState m_LightsState;

	int m_iQueuedCoinCounterPulses;
	RageTimer m_CoinCounterTimer;
	RageTimer m_RGBLightTimer;

	int GetTestAutoCycleCurrentIndex() { return (int)m_fTestAutoCycleCurrentIndex; }

	float			m_fTestAutoCycleCurrentIndex;
	CabinetLight	m_clTestManualCycleCurrent;
	int				m_iControllerTestManualCycleCurrent;
	int16_t rgb_red_top_count;
	int16_t rgb_red_bottom_count;
	int16_t rgb_blue_top_count;
	int16_t rgb_blue_bottom_count;
	int16_t rgb_neon_count_left;
	int16_t rgb_neon_count_right;
	bool rgb_pNeon = false;
	int rgb_neon_switch_count = 0;
	int rgb_randBase = 0;


	#define RGB_X 0 //nothing
	#define RGB_R 1 //red
	#define RGB_G 2 //green
	#define RGB_B 3 //blue
	#define RGB_W 4 //white
};

extern LightsManager*	LIGHTSMAN;	// global and accessible from anywhere in our program

#endif

/*
 * (c) 2003-2004 Chris Danford
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
