#include "global.h"
#include "LightsManager.h"
#include "GameState.h"
#include "RageTimer.h"
#include "arch/Lights/LightsDriver.h"
#include "RageUtil.h"
#include "GameInput.h"	// for GameController
#include "InputMapper.h"
#include "Game.h"
#include "PrefsManager.h"
#include "Actor.h"
#include "Preference.h"
#include "Foreach.h"
#include "GameManager.h"
#include "CommonMetrics.h"
#include "Style.h"

const RString DEFAULT_LIGHTS_DRIVER = "SystemMessage,Export";
static Preference<RString> g_sLightsDriver( "LightsDriver", "" ); // "" == DEFAULT_LIGHTS_DRIVER
Preference<float>	g_fLightsFalloffSeconds( "LightsFalloffSeconds", 0.1f );
Preference<float>	g_fLightsAheadSeconds( "LightsAheadSeconds", 0.05f );
static Preference<bool>	g_bBlinkGameplayButtonLightsOnNote( "BlinkGameplayButtonLightsOnNote", false );

static ThemeMetric<RString> GAME_BUTTONS_TO_SHOW( "LightsManager", "GameButtonsToShow" );

static const char *CabinetLightNames[] = {
	"MarqueeUpLeft",
	"MarqueeUpRight",
	"MarqueeLrLeft",
	"MarqueeLrRight",
	"BassLeft",
	"BassRight",
};
XToString( CabinetLight );
StringToX( CabinetLight );

static const char *LightsModeNames[] = {
	"Attract",
	"Joining",
	"MenuStartOnly",
	"MenuStartAndDirections",
	"Demonstration",
	"Gameplay",
	"Stage",
	"Cleared",
	"TestAutoCycle",
	"TestManualCycle",
};
XToString( LightsMode );
LuaXType( LightsMode );

static void GetUsedGameInputs( vector<GameInput> &vGameInputsOut )
{
	vGameInputsOut.clear();

	vector<RString> asGameButtons;
	split( GAME_BUTTONS_TO_SHOW.GetValue(), ",", asGameButtons );
	FOREACH_ENUM( GameController,  gc )
	{
		FOREACH_CONST( RString, asGameButtons, button )
		{
			GameButton gb = StringToGameButton( INPUTMAPPER->GetInputScheme(), *button );
			if( gb != GameButton_Invalid )
			{
				GameInput gi = GameInput( gc, gb );
				vGameInputsOut.push_back( gi );
			}
		}
	}

	set<GameInput> vGIs;
	vector<const Style*> vStyles;
	GAMEMAN->GetStylesForGame( GAMESTATE->m_pCurGame, vStyles );
	FOREACH( const Style*, vStyles, style )
	{
		bool bFound = find( CommonMetrics::STEPS_TYPES_TO_SHOW.GetValue().begin(), CommonMetrics::STEPS_TYPES_TO_SHOW.GetValue().end(), (*style)->m_StepsType ) != CommonMetrics::STEPS_TYPES_TO_SHOW.GetValue().end();
		if( !bFound )
			continue;
		FOREACH_PlayerNumber( pn )
		{
			for( int iCol=0; iCol<(*style)->m_iColsPerPlayer; ++iCol )
			{
				vector<GameInput> gi;
				(*style)->StyleInputToGameInput( iCol, pn, gi );
				for(size_t i= 0; i < gi.size(); ++i)
				{
					if(gi[i].IsValid())
					{
						vGIs.insert(gi[i]);
					}
				}
			}
		}
	}

	FOREACHS_CONST( GameInput, vGIs, gi )
		vGameInputsOut.push_back( *gi );
}

LightsManager*	LIGHTSMAN = NULL;	// global and accessible from anywhere in our program

LightsManager::LightsManager()
{
	ZERO( m_fSecsLeftInCabinetLightBlink );
	ZERO( m_fSecsLeftInGameButtonBlink );
	ZERO( m_fActorLights );
	ZERO( m_fSecsLeftInActorLightBlink );
	m_iQueuedCoinCounterPulses = 0;
	m_CoinCounterTimer.SetZero();

	m_LightsMode = LIGHTSMODE_JOINING;
	RString sDriver = g_sLightsDriver.Get();
	if( sDriver.empty() )
		sDriver = DEFAULT_LIGHTS_DRIVER;
	LightsDriver::Create( sDriver, m_vpDrivers );

	SetLightsMode( LIGHTSMODE_ATTRACT );
}

LightsManager::~LightsManager()
{
	FOREACH( LightsDriver*, m_vpDrivers, iter )
		SAFE_DELETE( *iter );
	m_vpDrivers.clear();
}

// XXX: Allow themer to change these. (rewritten; who wrote original? -aj)
static const float g_fLightEffectRiseSeconds = 0.075f;
static const float g_fLightEffectFalloffSeconds = 0.35f;
static const float g_fCoinPulseTime = 0.100f; 
void LightsManager::BlinkActorLight( CabinetLight cl )
{
	m_fSecsLeftInActorLightBlink[cl] = g_fLightEffectRiseSeconds;
}

float LightsManager::GetActorLightLatencySeconds() const
{
	return g_fLightEffectRiseSeconds;
}

void LightsManager::Update( float fDeltaTime )
{
	// Update actor effect lights.
	FOREACH_CabinetLight( cl )
	{
		float fTime = fDeltaTime;
		float &fDuration = m_fSecsLeftInActorLightBlink[cl];
		if( fDuration > 0 )
		{
			// The light has power left.  Brighten it.
			float fSeconds = min( fDuration, fTime );
			fDuration -= fSeconds;
			fTime -= fSeconds;
			fapproach( m_fActorLights[cl], 1, fSeconds / g_fLightEffectRiseSeconds );
		}

		if( fTime > 0 )
		{
			// The light is out of power.  Dim it.
			fapproach( m_fActorLights[cl], 0, fTime / g_fLightEffectFalloffSeconds );
		}

		Actor::SetBGMLight( cl, m_fActorLights[cl] );
	}

	if( !IsEnabled() )
		return;

	// update lights falloff
	{
		FOREACH_CabinetLight( cl )
			fapproach( m_fSecsLeftInCabinetLightBlink[cl], 0, fDeltaTime );
		FOREACH_ENUM( GameController,  gc )
			FOREACH_ENUM( GameButton,  gb )
				fapproach( m_fSecsLeftInGameButtonBlink[gc][gb], 0, fDeltaTime );
	}

	// Set new lights state cabinet lights
	{
		ZERO( m_LightsState.m_bCabinetLights );
		ZERO( m_LightsState.m_bGameButtonLights );
	}

	{
		m_LightsState.m_bCoinCounter = false;
		if( !m_CoinCounterTimer.IsZero() )
		{
			float fAgo = m_CoinCounterTimer.Ago();
			if( fAgo < g_fCoinPulseTime )
				m_LightsState.m_bCoinCounter = true;
			else if( fAgo >= g_fCoinPulseTime * 2 )
				m_CoinCounterTimer.SetZero();
		}
		else if( m_iQueuedCoinCounterPulses )
		{
			m_CoinCounterTimer.Touch();
			--m_iQueuedCoinCounterPulses;
		}
	}

	if( m_LightsMode == LIGHTSMODE_TEST_AUTO_CYCLE )
	{
		m_fTestAutoCycleCurrentIndex += fDeltaTime;
		m_fTestAutoCycleCurrentIndex = fmodf( m_fTestAutoCycleCurrentIndex, NUM_CabinetLight*100 );
	}

	switch( m_LightsMode )
	{
		DEFAULT_FAIL( m_LightsMode );

		case LIGHTSMODE_ATTRACT:
		{
			int iSec = (int)RageTimer::GetTimeSinceStartFast();
			int iTopIndex = iSec % 4;

			// Aldo: Disabled this line, apparently it was a forgotten initialization
			//CabinetLight cl = CabinetLight_Invalid;

			switch( iTopIndex )
			{
				DEFAULT_FAIL( iTopIndex );
				case 0:	m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT]  = true;	break;
				case 1:	m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT] = true;	break;
				case 2:	m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT] = true;	break;
				case 3:	m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT]  = true;	break;
			}

			if( iTopIndex == 0 )
			{
				m_LightsState.m_bCabinetLights[LIGHT_BASS_LEFT] = true;
				m_LightsState.m_bCabinetLights[LIGHT_BASS_RIGHT] = true;
			}

			break;
		}
		case LIGHTSMODE_MENU_START_ONLY:
		case LIGHTSMODE_MENU_START_AND_DIRECTIONS:
		case LIGHTSMODE_JOINING:
		{
			static int iLight;

			// if we've crossed a beat boundary, advance the light index
			{
				static float fLastBeat;
				float fLightSongBeat = GAMESTATE->m_Position.m_fLightSongBeat;

				if( fracf(fLightSongBeat) < fracf(fLastBeat) )
				{
					++iLight;
					wrap( iLight, 4 );
				}

				fLastBeat = fLightSongBeat;
			}

			CabinetLight cl = CabinetLight_Invalid;

			switch( iLight )
			{
				DEFAULT_FAIL( iLight );
				case 0:	cl = LIGHT_MARQUEE_UP_LEFT;	break;
				case 1:	cl = LIGHT_MARQUEE_LR_RIGHT;	break;
				case 2:	cl = LIGHT_MARQUEE_UP_RIGHT;	break;
				case 3:	cl = LIGHT_MARQUEE_LR_LEFT;	break;
			}

			m_LightsState.m_bCabinetLights[cl] = true;

			break;
		}

		case LIGHTSMODE_DEMONSTRATION:
		case LIGHTSMODE_GAMEPLAY:
		{
			FOREACH_CabinetLight( cl )
				m_LightsState.m_bCabinetLights[cl] = m_fSecsLeftInCabinetLightBlink[cl] > 0;

			break;
		}

		case LIGHTSMODE_STAGE:
		case LIGHTSMODE_ALL_CLEARED:
		{
			FOREACH_CabinetLight( cl )
				m_LightsState.m_bCabinetLights[cl] = true;

			break;
		}

		case LIGHTSMODE_TEST_AUTO_CYCLE:
		{
			int iSec = GetTestAutoCycleCurrentIndex();

			CabinetLight cl = CabinetLight(iSec % NUM_CabinetLight);
			m_LightsState.m_bCabinetLights[cl] = true;

			break;
		}

		case LIGHTSMODE_TEST_MANUAL_CYCLE:
		{
			CabinetLight cl = m_clTestManualCycleCurrent;
			m_LightsState.m_bCabinetLights[cl] = true;

			break;
		}
	}


	// Update game controller lights
	switch( m_LightsMode )
	{
		DEFAULT_FAIL( m_LightsMode );

		case LIGHTSMODE_ALL_CLEARED:
		case LIGHTSMODE_STAGE:
		case LIGHTSMODE_JOINING:
		{
			FOREACH_ENUM( GameController, gc )
			{
				if( GAMESTATE->m_bSideIsJoined[gc] )
				{
					FOREACH_ENUM( GameButton, gb )
						m_LightsState.m_bGameButtonLights[gc][gb] = true;
				}
			}

			break;
		}

		case LIGHTSMODE_MENU_START_ONLY:
		case LIGHTSMODE_MENU_START_AND_DIRECTIONS:
		{
			float fLightSongBeat = GAMESTATE->m_Position.m_fLightSongBeat;

			/* Blink menu lights on the first half of the beat */
			if( fracf(fLightSongBeat) <= 0.5f )
			{
				FOREACH_PlayerNumber( pn )
				{
					if( !GAMESTATE->m_bSideIsJoined[pn] )
						continue;

					m_LightsState.m_bGameButtonLights[pn][GAME_BUTTON_START] = true;

					if( m_LightsMode == LIGHTSMODE_MENU_START_AND_DIRECTIONS )
					{
						m_LightsState.m_bGameButtonLights[pn][GAME_BUTTON_MENULEFT] = true;
						m_LightsState.m_bGameButtonLights[pn][GAME_BUTTON_MENURIGHT] = true;
					}
				}
			}

			// fall through to blink on button presses
		}

		case LIGHTSMODE_DEMONSTRATION:
		case LIGHTSMODE_GAMEPLAY:
		{
			bool bGameplay = (m_LightsMode == LIGHTSMODE_DEMONSTRATION) || (m_LightsMode == LIGHTSMODE_GAMEPLAY);

			// Blink on notes during gameplay.
			if( bGameplay && g_bBlinkGameplayButtonLightsOnNote )
			{
				FOREACH_ENUM( GameController,  gc )
				{
					FOREACH_ENUM( GameButton,  gb )
					{
						m_LightsState.m_bGameButtonLights[gc][gb] = m_fSecsLeftInGameButtonBlink[gc][gb] > 0 ;
					}
				}
				break;
			}

			// fall through to blink on button presses
		}

		case LIGHTSMODE_ATTRACT:
		{
			// Blink on button presses.
			FOREACH_ENUM( GameController,  gc )
			{
				FOREACH_GameButton_Custom( gb )
				{
					bool bOn = INPUTMAPPER->IsBeingPressed( GameInput(gc,gb) );
					m_LightsState.m_bGameButtonLights[gc][gb] = bOn;
				}
			}

			break;
		}

		case LIGHTSMODE_TEST_AUTO_CYCLE:
		{
			int index = GetTestAutoCycleCurrentIndex();

			vector<GameInput> vGI;
			GetUsedGameInputs( vGI );
			wrap( index, vGI.size() );

			ZERO( m_LightsState.m_bGameButtonLights );

			GameController gc = vGI[index].controller;
			GameButton gb = vGI[index].button;
			m_LightsState.m_bGameButtonLights[gc][gb] = true;

			break;
		}

		case LIGHTSMODE_TEST_MANUAL_CYCLE:
		{
			ZERO( m_LightsState.m_bGameButtonLights );

			vector<GameInput> vGI;
			GetUsedGameInputs( vGI );

			if( m_iControllerTestManualCycleCurrent != -1 )
			{
				GameController gc = vGI[m_iControllerTestManualCycleCurrent].controller;
				GameButton gb = vGI[m_iControllerTestManualCycleCurrent].button;
				m_LightsState.m_bGameButtonLights[gc][gb] = true;
			}

			break;
		}
	}

	// If not joined, has enough credits, and not too late to join, then
	// blink the menu buttons rapidly so they'll press Start
	{
		int iBeat = (int)(GAMESTATE->m_Position.m_fLightSongBeat*4);
		bool bBlinkOn = (iBeat%2)==0;
		FOREACH_PlayerNumber( pn )
		{
			if( !GAMESTATE->m_bSideIsJoined[pn] && GAMESTATE->PlayersCanJoin() && GAMESTATE->EnoughCreditsToJoin() )
				m_LightsState.m_bGameButtonLights[pn][GAME_BUTTON_START] = bBlinkOn;
		}
	}

	//RGB support -- every 1/60th of a second update the RGB lights because of the way we generate patterns
	//TODO: Only update when the LightsDriver is an RGB one.
	/*if (m_RGBLightTimer.Ago() >= 0.0166667f)
	{
		updateRGBLights();
		m_RGBLightTimer.Touch();
	}*/

	

	// apply new light values we set above
	FOREACH( LightsDriver*, m_vpDrivers, iter )
		(*iter)->Set( &m_LightsState );
}

void LightsManager::updateRGBLights()
{
	//This can be massivly improved here by using different patterns based on context
	//but for now, we mimic the DDR HD cabinet P3IO HDXB driver's implementation
	//This requires OITG style lights otherwise white will be the only pattern generated
	if (true)//stub -- only want to process this 60 times a second at most otherwise lights will be all white
	{
		rgb_red_bottom_count=m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT] ? upperCapAt(0x7F, rgb_red_bottom_count++) : lowerCapAt(0x00, rgb_red_bottom_count -= 2);
		rgb_blue_bottom_count=m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT] ? upperCapAt(0x7F, rgb_blue_bottom_count++) : lowerCapAt(0x00, rgb_blue_bottom_count -= 2);
		rgb_red_top_count=m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT] ? upperCapAt(0x7F, rgb_red_top_count++) : lowerCapAt(0x00, rgb_red_top_count -= 2);
		rgb_blue_top_count=m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT] ? upperCapAt(0x7F, rgb_blue_top_count++) : lowerCapAt(0x00, rgb_blue_top_count -= 2);
		rgb_neon_count_left=m_LightsState.m_bCabinetLights[LIGHT_BASS_LEFT] ? upperCapAt(0x7F, rgb_neon_count_left += 4) : lowerCapAt(0x00, rgb_neon_count_left -= 4);
		rgb_neon_count_right=m_LightsState.m_bCabinetLights[LIGHT_BASS_RIGHT] ? upperCapAt(0x7F, rgb_neon_count_right += 4) : lowerCapAt(0x00, rgb_neon_count_right -= 4);
		
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_UP_LEFT].g = rgb_neon_count_left & 0xFF;//G
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_UP_LEFT].r = rgb_red_top_count & 0xFF;//R
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_UP_LEFT].b = rgb_blue_top_count & 0xFF;//B

		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_UP_RIGHT].g = rgb_neon_count_right & 0xFF;//G
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_UP_RIGHT].r = rgb_red_top_count & 0xFF;//R
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_UP_RIGHT].b = rgb_blue_top_count & 0xFF;//B

		//these are actually the "neons" on ddr hd, we will replicate the below too
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_LR_LEFT].g = rgb_neon_count_left & 0xFF;//G
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_LR_LEFT].r = rgb_red_bottom_count & 0xFF;//R
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_LR_LEFT].b = rgb_blue_bottom_count & 0xFF;//B

		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_LR_RIGHT].g = rgb_neon_count_right & 0xFF;//G
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_LR_RIGHT].r = rgb_red_bottom_count & 0xFF;//R
		m_LightsState.m_rgbCabinetLights[LIGHT_MARQUEE_LR_RIGHT].b = rgb_blue_bottom_count & 0xFF;//B

		m_LightsState.m_rgbCabinetLights[LIGHT_BASS_LEFT].g = rgb_neon_count_left & 0xFF;//G
		m_LightsState.m_rgbCabinetLights[LIGHT_BASS_LEFT].r = rgb_red_bottom_count & 0xFF;//R
		m_LightsState.m_rgbCabinetLights[LIGHT_BASS_LEFT].b = rgb_blue_bottom_count & 0xFF;//B

		m_LightsState.m_rgbCabinetLights[LIGHT_BASS_RIGHT].g = rgb_neon_count_right & 0xFF;//G
		m_LightsState.m_rgbCabinetLights[LIGHT_BASS_RIGHT].r = rgb_red_bottom_count & 0xFF;//R
		m_LightsState.m_rgbCabinetLights[LIGHT_BASS_RIGHT].b = rgb_blue_bottom_count & 0xFF;//B

	}


	//spire code
	bool p1_start = false;
	bool p2_start = false;
	bool ul = false;
	bool ur = false;
	bool ll = false;
	bool lr = false;
	bool neon = false;
	bool neon_switch = false;

	if (m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT]) ul = true;
	if (m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT]) ur = true;
	if (m_LightsState.m_bCabinetLights[LIGHT_BASS_LEFT]) neon = true;
	if (m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT]) lr = true;
	if (m_LightsState.m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT]) ll = true;
	if (m_LightsState.m_bCabinetLights[LIGHT_BASS_RIGHT]) neon = true;

	if (m_LightsState.m_bGameButtonLights[GameController_1][GAME_BUTTON_START]) p1_start = true;
	if (m_LightsState.m_bGameButtonLights[GameController_2][GAME_BUTTON_START]) p2_start = true;

	if (neon != rgb_pNeon)
	{
		neon_switch = true;
		rgb_neon_switch_count %= 3;
		rgb_pNeon = neon;
		rgb_randBase = (rand() % 3) + rgb_neon_switch_count;
	}

	//init satelllite lights to off

	setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_X);
	setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_X);
	setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_X);
	setRGBLightToSolid(m_LightsState.m_rgbSpires[3], RGB_X);
	setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_X);
	setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_X);
	setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_X);


	if (neon)
	{

		//left

		if (ul && ll)
		{//all white
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_W);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_W);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_W);
		}
		if (ul && !ll)
		{//red
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_R);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_R);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_R);
		}
		if (!ul && !ll)
		{//blue
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_B);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_B);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_B);
		}
		if (!ul && ll)
		{//green
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_G);
		}



		//right
		if (ur && lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_W);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_W);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_W);
		}
		if (ur && !lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_R);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_R);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_R);
		}
		if (!ur && !lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_B);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_B);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_B);
		}
		if (!ur && lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_G);
		}


	}
	else
	{
		//left
		if (ul && ll)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], ((0 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], ((1 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], ((2 + rgb_randBase) % 3) + 1);
		}
		if (ul && !ll)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], ((1 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], ((2 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], ((3 + rgb_randBase) % 3) + 1);
		}
		if (!ul && ll)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], ((2 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], ((3 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], ((4 + rgb_randBase) % 3) + 1);
		}


		//right


		if (ur && lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], ((0 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], ((1 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[7], ((2 + rgb_randBase) % 3) + 1);
		}
		if (ur && !lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], ((1 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], ((2 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[7], ((3 + rgb_randBase) % 3) + 1);
		}
		if (!ur && lr)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], ((2 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], ((3 + rgb_randBase) % 3) + 1);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[7], ((4 + rgb_randBase) % 3) + 1);
		}
	}

	if (p1_start)
	{
		setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_B);
		setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_B);
		setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_B);
		setRGBLightToSolid(m_LightsState.m_rgbSpires[3], RGB_R);

		if (!p2_start)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_G);
		}
	}



	if (p2_start)
	{
		setRGBLightToSolid(m_LightsState.m_rgbSpires[4], RGB_B);
		setRGBLightToSolid(m_LightsState.m_rgbSpires[5], RGB_B);
		setRGBLightToSolid(m_LightsState.m_rgbSpires[6], RGB_B);
		setRGBLightToSolid(m_LightsState.m_rgbSpires[3], RGB_B);

		if (!p1_start)
		{
			setRGBLightToSolid(m_LightsState.m_rgbSpires[0], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[1], RGB_G);
			setRGBLightToSolid(m_LightsState.m_rgbSpires[2], RGB_G);
		}
	}

	if (p1_start&&p2_start)
	{
		setRGBLightToSolid(m_LightsState.m_rgbSpires[3], RGB_G);
	}

}

void LightsManager::setRGBLightToSolid(RGBLight l, uint8_t lightColor)
{
	switch (lightColor)
	{
	case RGB_R:
		l.r = 0x7F;
		l.g =0x00;
		l.b = 0x00;
		break;
	case RGB_G:
		l.r = 0x00;
		l.g = 0x7F;
		l.b = 0x00;
		break;
	case RGB_B:
		l.r = 0x00;
		l.g = 0x00;
		l.b = 0x7f;
		break;
	case RGB_W:
		l.r = 0x7F;
		l.g = 0x7F;
		l.b = 0x7f;
		break;
	default:
		l.r = 0x00;
		l.g = 0x00;
		l.b = 0x00;
		break;
	}
}



int16_t LightsManager::upperCapAt(int16_t cap, int16_t var)
{
	if (var > cap) return cap;
	return var;
}

int16_t LightsManager::lowerCapAt(int16_t cap, int16_t var)
{
	if (var < cap) return cap;
	return var;
}


void LightsManager::BlinkCabinetLight( CabinetLight cl )
{
	m_fSecsLeftInCabinetLightBlink[cl] = g_fLightsFalloffSeconds;
}

void LightsManager::BlinkGameButton( GameInput gi )
{
	m_fSecsLeftInGameButtonBlink[gi.controller][gi.button] = g_fLightsFalloffSeconds;
}

void LightsManager::SetLightsMode( LightsMode lm )
{
	m_LightsMode = lm;
	m_fTestAutoCycleCurrentIndex = 0;
	m_clTestManualCycleCurrent = CabinetLight_Invalid;
	m_iControllerTestManualCycleCurrent = -1;
}

LightsMode LightsManager::GetLightsMode()
{
	return m_LightsMode;
}

void LightsManager::ChangeTestCabinetLight( int iDir )
{
	m_iControllerTestManualCycleCurrent = -1;

	enum_add( m_clTestManualCycleCurrent, iDir );
	wrap( *ConvertValue<int>(&m_clTestManualCycleCurrent), NUM_CabinetLight );
}

void LightsManager::ChangeTestGameButtonLight( int iDir )
{
	m_clTestManualCycleCurrent = CabinetLight_Invalid;

	vector<GameInput> vGI;
	GetUsedGameInputs( vGI );

	m_iControllerTestManualCycleCurrent += iDir;
	wrap( m_iControllerTestManualCycleCurrent, vGI.size() );
}

CabinetLight LightsManager::GetFirstLitCabinetLight()
{
	FOREACH_CabinetLight( cl )
	{
		if( m_LightsState.m_bCabinetLights[cl] )
			return cl;
	}
	return CabinetLight_Invalid;
}

GameInput LightsManager::GetFirstLitGameButtonLight()
{
	FOREACH_ENUM( GameController, gc )
	{
		FOREACH_ENUM( GameButton, gb )
		{
			if( m_LightsState.m_bGameButtonLights[gc][gb] )
				return GameInput( gc, gb );
		}
	}
	return GameInput();
}

bool LightsManager::IsEnabled() const
{
	return m_vpDrivers.size() >= 1 || PREFSMAN->m_bDebugLights;
}

void LightsManager::TurnOffAllLights()
{
	FOREACH( LightsDriver*, m_vpDrivers, iter )
		(*iter)->Reset();
}

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
