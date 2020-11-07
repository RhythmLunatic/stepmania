#include "global.h"
#include "ScreenLowLevelInput.h"
#include "ScreenManager.h"
#include "RageLog.h"
#include "InputMapper.h"
#include "ThemeManager.h"
//#include "ScreenDimensions.h"
//#include "PrefsManager.h"
#include "RageInput.h"
//#include "InputEventPlus.h"
//#include "LocalizedString.h"

REGISTER_SCREEN_CLASS( ScreenLowLevelInput );
ScreenLowLevelInput::ScreenLowLevelInput()
{
    inputCallback = LuaReference();
}

void ScreenLowLevelInput::Update( float fDeltaTime)
{
    if (inputCallback.IsSet() && !inputCallback.IsNil())
    {
        vector<DeviceInput> DeviceInputs;

        //vector of currently held mapped inputs. For example it would be ["UpLeft","UpRight","DownLeft"] when three are held.
        vector<RString> mappedInputArray;
        //vector of the raw held inputs, mapped or unmapped. For example it would be ["q","w","p"].
        vector<RString> rawInputArray;

        INPUTFILTER->GetPressedButtons( DeviceInputs );
        FOREACH( DeviceInput, DeviceInputs, di )
        {
            //Skip if button isn't down, of course
            if( !di->bDown && di->level == 0.0f )
                continue;

            rawInputArray.push_back(INPUTMAN->GetDeviceSpecificInputString(*di));

            GameInput gi;
            if( INPUTMAPPER->DeviceToGame(*di,gi) )
            {
                RString sName = GameButtonToString( INPUTMAPPER->GetInputScheme(), gi.button );
                mappedInputArray.push_back(sName);
                /*sTemp += ssprintf(" - %s %d %s", CONTROLLER.GetValue().c_str(), gi.controller+1, sName.c_str() );

                if( !PREFSMAN->m_bOnlyDedicatedMenuButtons )
                {
                    GameButton mb = INPUTMAPPER->GetInputScheme()->GameButtonToMenuButton( gi.button );
                    if( mb != GameButton_Invalid && mb != gi.button )
                    {
                        RString sGameButtonString = GameButtonToLocalizedString( INPUTMAPPER->GetInputScheme(), mb );
                        sTemp += ssprintf( " - (%s %s)", sGameButtonString.c_str(), SECONDARY.GetValue().c_str() );
                    }
                }*/
            }

            /*RString sComment = INPUTFILTER->GetButtonComment( *di );
            if( sComment != "" )
                sTemp += " - " + sComment;

            asInputs.push_back( sTemp );*/
        }

        Lua* L= LUA->Get();

        //Push the input callback onto the stack
        inputCallback.PushSelf(L); //-3

        LuaHelpers::CreateTableFromArray<RString>( mappedInputArray, L ); //-2
        LuaHelpers::CreateTableFromArray<RString>( rawInputArray, L ); //-1

        RString error= "Error running input callback: ";
        //bool LuaHelpers::RunScriptOnStack( Lua *L, RString &Error, int Args, int ReturnValues, bool ReportError )
        //it runs whatever is on the stack at position numargs-1, so -3 in this case
        LuaHelpers::RunScriptOnStack(L, error, 2, 1, true);
        //pop inputCallback from stack (arrays are already popped after being used for RunScriptOnStack)
        lua_pop(L, 1);

        LUA->Release(L);
    }
    //Always remember to update the parent class... Or the game will hang...
    ScreenWithMenuElements::Update(fDeltaTime);


}

void ScreenLowLevelInput::SetInputCallbackFromStack(lua_State* L)
{
    inputCallback = LuaReference(L);
}

// lua start
#include "LuaBinding.h"

/** @brief Allow Lua to have access to ScreenLowLevelInput. */
class LunaScreenLowLevelInput: public Luna<ScreenLowLevelInput>
{
public:

    static int SetInputCallback(T* p, lua_State* L)
    {
        if(!lua_isfunction(L, 1))
        {
            luaL_error(L, "Input callback must be a function.");
        }
        p->SetInputCallbackFromStack(L);
        COMMON_RETURN_SELF;
    }

    /*static int RemoveInputCallback(T* p, lua_State* L)
    {
        if(!lua_isfunction(L, 1))
        {
            luaL_error(L, "Input callback must be a function.");
        }
        p->RemoveInputCallback(L);
        COMMON_RETURN_SELF;
    }*/

    LunaScreenLowLevelInput()
    {
        ADD_METHOD( SetInputCallback );
        //ADD_METHOD( RemoveInputCallback );
    }
};

LUA_REGISTER_DERIVED_CLASS( ScreenLowLevelInput, ScreenWithMenuElements )

/*
 *  (c) 2018-2020 Rhythm Lunatic
 *
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */