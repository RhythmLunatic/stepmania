#ifndef STEPMANIA_SCREENWITHOPTIONSLIST_H
#define STEPMANIA_SCREENWITHOPTIONSLIST_H

#include "ScreenWithMenuElements.h"
#include "GameInput.h"
#include "OptionsList.h"

class ScreenWithOptionsList : public ScreenWithMenuElements {
public:
    //virtual ~ScreenWithOptionsList();
    virtual void Init();
    virtual void BeginScreen();

    //virtual void Update( float fDeltaTime );
    virtual bool Input( const InputEventPlus &input );
    //virtual void HandleMessage( const Message &msg );
    //virtual void HandleScreenMessage( const ScreenMessage SM );

    void OpenOptionsList( PlayerNumber pn );
    bool CanOpenOptionsList( PlayerNumber pn );

    // Lua
    virtual void PushSelf( lua_State *L );
protected:
    OptionsList		m_OptionsList[NUM_PLAYERS];
    bool			m_bAcceptSelectRelease[NUM_PLAYERS]; //Not sure what this is, to be honest.
};


#endif //STEPMANIA_SCREENWITHOPTIONSLIST_H
