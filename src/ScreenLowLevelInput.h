/* ScreenLowLevelInput - Lower level input handling for when AddInputCallback does not work. */

#ifndef STEPMANIA_SCREENLOWLEVELINPUT_H
#define STEPMANIA_SCREENLOWLEVELINPUT_H

#include "ScreenWithMenuElements.h"

class ScreenLowLevelInput : public ScreenWithMenuElements
{
public:
    ScreenLowLevelInput();
    virtual void Update( float fDeltaTime );
    void SetInputCallbackFromStack(lua_State* L);

    //lua binding
    virtual void PushSelf( lua_State *L );
private:
    LuaReference inputCallback;
};


#endif //STEPMANIA_SCREENLOWLEVELINPUT_H

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