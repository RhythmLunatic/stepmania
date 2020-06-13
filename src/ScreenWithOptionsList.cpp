#include "global.h"
#include "ScreenWithOptionsList.h"
#include "OptionsList.h"
#include "ScreenManager.h"
#include "InputEventPlus.h"
//#include "CommonMetrics.h"

REGISTER_SCREEN_CLASS( ScreenWithOptionsList );

void ScreenWithOptionsList::Init()
{
    //ScreenWithMenuElements Init must be before you start adding actors.
    ScreenWithMenuElements::Init();

    FOREACH_PlayerNumber(p)
    {
        m_OptionsList[p].SetName( "OptionsList" + PlayerNumberToString(p) );
        m_OptionsList[p].Load( "OptionsList", p );
        m_OptionsList[p].SetDrawOrder( 100 );
        ActorUtil::LoadAllCommands( m_OptionsList[p], m_sName );
        this->AddChild( &m_OptionsList[p] );
    }
    m_OptionsList[PLAYER_1].Link( &m_OptionsList[PLAYER_2] );
    m_OptionsList[PLAYER_2].Link( &m_OptionsList[PLAYER_1] );

}

void ScreenWithOptionsList::BeginScreen() {
    FOREACH_PlayerNumber( pn )
        m_OptionsList[pn].Reset();

    ScreenWithMenuElements::BeginScreen();
}

bool ScreenWithOptionsList::Input(const InputEventPlus &input) {
    PlayerNumber pn = input.pn;
    if( pn != PLAYER_INVALID )
    {
        if( m_OptionsList[pn].IsOpened() )
        {
            return m_OptionsList[pn].Input( input );
        }
        else
        {
            if( input.type == IET_RELEASE  &&  input.MenuI == GAME_BUTTON_SELECT && m_bAcceptSelectRelease[pn] )
                m_OptionsList[pn].Open();
        }
    }
    return ScreenWithMenuElements::Input( input );
}

void ScreenWithOptionsList::OpenOptionsList(PlayerNumber pn) {
    if( pn != PLAYER_INVALID )
    {
        m_OptionsList[pn].Open();
    }
}

/* One might think this is a pointless function to have since you'd always be able to open
 * the OptionsList, but this can be used to check if it's already open.
 * In the interest of keeping it easy to use with screens that originally used ScreenSelectMusic,
 * I won't rename it to IsOptionsListOpened.
 */
bool ScreenWithOptionsList::CanOpenOptionsList(PlayerNumber pn)
{
    if(pn >= NUM_PLAYERS)
    {
        return false;
    }
    return !m_OptionsList[pn].IsOpened();
}

// lua start
#include "LuaBinding.h"

class LunaScreenWithOptionsList: public Luna<ScreenWithOptionsList>
{
public:
    static int OpenOptionsList( T* p, lua_State *L )
    {
        PlayerNumber pn = Enum::Check<PlayerNumber>(L, 1);
        p->OpenOptionsList(pn);

        /*if(p->CanOpenOptionsList(pn))
        {
            p->OpenOptionsList(pn);
        }*/
        COMMON_RETURN_SELF;
    }
    static int CanOpenOptionsList( T* p, lua_State *L )
    {
        PlayerNumber pn = Enum::Check<PlayerNumber>(L, 1);
        lua_pushboolean(L, p->CanOpenOptionsList(pn));
        return 1;
    }

    LunaScreenWithOptionsList()
    {
        ADD_METHOD( OpenOptionsList );
        ADD_METHOD( CanOpenOptionsList );
    }
};

LUA_REGISTER_DERIVED_CLASS( ScreenWithOptionsList, ScreenWithMenuElements )

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
