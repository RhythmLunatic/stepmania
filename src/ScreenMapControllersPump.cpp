//
// The debugger would crash trying to debug this screen, so the only option was to put LOG->Trace() everywhere.
//

#include "global.h"

#include "ScreenMapControllersPump.h"

#include "ScreenMapControllers.h"
#include "ScreenManager.h"
#include "ScreenPrompt.h"
#include "RageLog.h"
#include "RageInput.h"
#include "InputMapper.h"
#include "ThemeManager.h"
#include "ScreenDimensions.h"
#include "InputEventPlus.h"
#include "LocalizedString.h"

AutoScreenMessage(SM_DoSaveAndExit);
#define BUTTONS_TO_MAP			THEME->GetMetric ( m_sName, "ButtonsToMap" )
static LocalizedString INVALID_BUTTON   ( "ScreenMapControllers", "InvalidButton" );
static LocalizedString SAVE_PROMPT("ScreenMapControllers", "SavePrompt");
#define MAPPED_TO_COMMAND(slot)	THEME->GetMetricA( m_sName, ssprintf("MappedToS%iCommand", slot+1) )

static const float g_fSecondsToWaitForInput = 0.05f;

// reserve the 3rd slot for hard-coded keys
static const int NUM_CHANGABLE_SLOTS = NUM_SHOWN_GAME_TO_DEVICE_SLOTS-1;

REGISTER_SCREEN_CLASS( ScreenMapControllersPump );

static LocalizedString PLAYER_SLOTS( "ScreenMapControllers", "%s slots" );
static LocalizedString SLOT_NAMES[3]= {
        LocalizedString("ScreenMapControllers", "Primary"),
        LocalizedString("ScreenMapControllers", "Secondary"),
        LocalizedString("ScreenMapControllers", "Default")
};
static LocalizedString KEYNAME("ScreenMapControllers", "KeyName");

ScreenMapControllersPump::ScreenMapControllersPump()
{
    LOG->Trace("run class constructor!");
    m_InSetListMode= false;
    m_ChangeOccurred= false;
    this->SubscribeToMessage( Message_AutoJoyMappingApplied );
}
ScreenMapControllersPump::~ScreenMapControllersPump()
{
    for(size_t i= 0; i < m_Line.size(); ++i)
    {
        SAFE_DELETE(m_Line[i]);
    }
}

void ScreenMapControllersPump::Init()
{
    USERSCROLLER_TRANSFORM.Load( m_sName, "UserScrollerTransformFunction" );

    LOG->Trace("Init function starting!");
    //ScreenMapControllersPump::Init();
    LOG->Trace("Init base screen!");
    ScreenWithMenuElements::Init();
    m_soundChange.Load( THEME->GetPathS(m_sName,"change"), true );
    m_soundDelete.Load( THEME->GetPathS(m_sName,"delete"), true );

    m_textDevices.LoadFromFont( THEME->GetPathF(m_sName,"devices") );
    m_textDevices.SetName( "Devices" );
    LOAD_ALL_COMMANDS_AND_SET_XY( m_textDevices );
    this->AddChild( &m_textDevices );

    LOG->Trace("Fill button mappings!");
    RString sButtons = BUTTONS_TO_MAP;
    if( sButtons.empty() )
    {
        /* Map all buttons for this game. */
        FOREACH_GameButtonInScheme( INPUTMAPPER->GetInputScheme(), gb )
        {
            KeyToMap k;
            k.m_GameButton = gb;
            m_KeysToMap.push_back( k );
        }
    }
    else
    {
        /* Map the specified buttons. */
        vector<RString> asBits;
        split( sButtons, ",", asBits );
        for( unsigned i=0; i<asBits.size(); ++i )
        {
            KeyToMap k;
            k.m_GameButton = StringToGameButton( INPUTMAPPER->GetInputScheme(), asBits[i] );
            m_KeysToMap.push_back( k );
        }
    }
    LOG->Trace("Init m_Line vector");

    // normal rows
    /*for( unsigned b=0; b<m_KeysToMap.size(); b++ )
    {
        m_Line.push_back(new ActorFrame);
        KeyToMap *pKey = &m_KeysToMap[b];


        //GameController c = GameController_1;
        pKey->m_textMappedTo = new BitmapText;
        pKey->m_textMappedTo->SetName( "MappedTo" );
        pKey->m_textMappedTo->LoadFromFont( THEME->GetPathF(m_sName,"entry") );
        pKey->m_textMappedTo->RunCommands( MAPPED_TO_COMMAND(GameController_1) );
        ActorUtil::LoadAllCommands( *pKey->m_textMappedTo, m_sName );
        m_Line.back()->AddChild( pKey->m_textMappedTo );

        m_Line.back()->DeleteChildrenWhenDone();
        m_Line.back()->SetName( "Line" );
        ActorUtil::LoadAllCommands(m_Line.back(), m_sName);
        m_LineScroller.AddChild(m_Line.back());
    }*/
    m_UserScrollerActors.resize(m_KeysToMap.size());
    for( unsigned b=0; b<m_KeysToMap.size(); b++ ) //For each key to map, generate a row...
    {
        m_Line.push_back(new ActorFrame); //From this point m_Line.back() refers to the ActorFrame that was just created
        KeyToMap *pKey = &m_KeysToMap[b];

        //No, not the primary and secondary headers. Primary meaning the button's primary function.
        {
            BitmapText *pName = new BitmapText;
            pName->SetName( "Primary" );
            pName->LoadFromFont( THEME->GetPathF(m_sName,"title") );
            RString sText = GameButtonToLocalizedString( INPUTMAPPER->GetInputScheme(), pKey->m_GameButton );
            pName->SetText( sText );
            ActorUtil::LoadAllCommands( *pName, m_sName );
            m_Line.back()->AddChild( pName );
        }
        //Usually blank, it's the text under the game pad buttons that shows their secondary function
        {
            BitmapText *pSecondary = new BitmapText;
            pSecondary->SetName( "Secondary" );
            pSecondary->LoadFromFont( THEME->GetPathF(m_sName,"title") );
            GameButton mb = INPUTMAPPER->GetInputScheme()->GameButtonToMenuButton( pKey->m_GameButton );
            RString sText;
            if( mb != GameButton_Invalid && mb != pKey->m_GameButton )
                sText = GameButtonToLocalizedString( INPUTMAPPER->GetInputScheme(), mb );
            ActorUtil::LoadAllCommands( *pSecondary, m_sName );
            pSecondary->SetText( sText );
            m_Line.back()->AddChild( pSecondary );
        }


        GameController c = GameController_1;
        for( int s=0; s<NUM_SHOWN_GAME_TO_DEVICE_SLOTS; s++ ) //Generate NUM_SHOWN_GAME_TO_DEVICE_SLOTS of columns
        {
            pKey->m_textMappedTo[c][s] = new BitmapText;
            pKey->m_textMappedTo[c][s]->SetName( "MappedTo" );
            pKey->m_textMappedTo[c][s]->LoadFromFont( THEME->GetPathF(m_sName,"entry") );
            pKey->m_textMappedTo[c][s]->RunCommands( MAPPED_TO_COMMAND(s) );
            ActorUtil::LoadAllCommands( *pKey->m_textMappedTo[c][s], m_sName );
            m_Line.back()->AddChild( pKey->m_textMappedTo[c][s] );
        }
        m_Line.back()->DeleteChildrenWhenDone();
        m_Line.back()->SetName( "Line" );
        ActorUtil::LoadAllCommands(m_Line.back(), m_sName);
        m_LineScroller.AddChild(m_Line.back()); //We have to add the ActorFrame to the ActorScroller now.

        //Probably not the best way to do it but it's copypasted from ScreenSelectMaster
        RString actorName = "ScrollItem";
        RString button = GameButtonToString( INPUTMAPPER->GetInputScheme(), pKey->m_GameButton );
        //ex. ScreenMapControllersPump ScrollItem UpLeft
        //actorName+=button;
        LuaThreadVariable var("Button", button);
        m_UserScrollerActors[b].Load(THEME->GetPathG(m_sName,actorName));
        m_UserScroller.AddChild(m_UserScrollerActors[b]);
    }

#define ADD_ACTION(index, a, b, c, d, e) \
	m_Line.push_back(new ActorFrame);	\
	m_Actions[index].Load(a, b, c, d, e);

    m_Actions.resize(4);
    ADD_ACTION(0, m_sName, "Clear", &ScreenMapControllersPump::ClearToDefault,
               m_Line.back(), &m_LineScroller);
    ADD_ACTION(1, m_sName, "Reload", &ScreenMapControllersPump::ReloadFromDisk,
               m_Line.back(), &m_LineScroller);
    ADD_ACTION(2, m_sName, "Save", &ScreenMapControllersPump::SaveToDisk,
               m_Line.back(), &m_LineScroller);
    //ADD_ACTION(3, m_sName, "SetList", &ScreenMapControllersPump::SetListMode,
    //           m_Line.back(), &m_LineScroller);
    ADD_ACTION(3, m_sName, "Exit", &ScreenMapControllersPump::ExitAction,
               m_Line.back(), &m_LineScroller);
#undef ADD_ACTION

    m_MaxDestItem= (1 + m_KeysToMap.size() + m_Actions.size()) -
                   THEME->GetMetricI("ScreenMapControllers", "LinesVisible");

    m_UserScroller.SetName("UserScroller");
    m_UserScroller.SetNumItemsToDraw((float)m_UserScroller.GetNumChildren()*2);
    m_UserScroller.Load2();
    m_UserScroller.SetTransformFromReference(USERSCROLLER_TRANSFORM);
    LOAD_ALL_COMMANDS_AND_SET_XY(m_UserScroller);
    this->AddChild(&m_UserScroller);


    m_LineScroller.SetName( "LineScroller" );
    ActorUtil::LoadAllCommands( m_LineScroller, m_sName );
    m_LineScroller.SetNumItemsToDraw( (float) m_LineScroller.GetNumChildren()*2 );
    m_LineScroller.Load2();
    this->AddChild( &m_LineScroller );

    m_NoSetListPrompt.Load(THEME->GetPathG(m_sName, "nosetlistprompt"));
    m_NoSetListPrompt->SetName("NoSetListPrompt");
    this->AddChild(m_NoSetListPrompt);

    m_SanityMessage.Load(THEME->GetPathG(m_sName, "sanitymessage"));
    m_SanityMessage->SetName("SanityMessage");
    this->AddChild(m_SanityMessage);

}
void ScreenMapControllersPump::BeginScreen()
{
    m_CurController = 0;
    m_CurButton = 0;
    m_CurSlot = 0;

    LOG->Trace("Call SWME BeginScreen!");
    ScreenWithMenuElements::BeginScreen();

    m_WaitingForPress.SetZero();

    LOG->Trace("Call refresh!");
    Refresh();
    LOG->Trace("Call AfterChangeFocus!");
    AfterChangeFocus();
    LOG->Trace("BeginScreen finish");
    //m_fLockInputSecs= 2.5f;
    m_AutoDismissNoSetListPromptSecs= 0.0f;
    m_AutoDismissSanitySecs= 0.0f;
}


void ScreenMapControllersPump::Update( float fDeltaTime )
{
    ScreenWithMenuElements::Update( fDeltaTime );

    //LOG->Trace("Call update");
    if(m_AutoDismissNoSetListPromptSecs > 0.0f)
    {
        m_AutoDismissNoSetListPromptSecs-= fDeltaTime;
        if(m_AutoDismissNoSetListPromptSecs <= 0.0f)
        {
            m_NoSetListPrompt->PlayCommand("TweenOff");
        }
    }
    if(m_AutoDismissSanitySecs > 0.0f)
    {
        m_AutoDismissSanitySecs-= fDeltaTime;
        if(m_AutoDismissSanitySecs <= 0.0f)
        {
            m_SanityMessage->PlayCommand("TweenOff");
        }
    }

    //
    // Update devices text
    //
    m_textDevices.SetText( INPUTMAN->GetDisplayDevicesString() );

    if( !m_WaitingForPress.IsZero() && m_DeviceIToMap.IsValid() ) // we're going to map an input
    {
        if( m_WaitingForPress.PeekDeltaTime() < g_fSecondsToWaitForInput )
            return; /* keep waiting */
        m_WaitingForPress.SetZero();

        ASSERT(CursorOnKey());
        const KeyToMap *pKey = &m_KeysToMap[CurKeyIndex()];

        GameInput curGameI( (GameController)m_CurController, pKey->m_GameButton );

        INPUTMAPPER->SetInputMap( m_DeviceIToMap, curGameI, m_CurSlot );
        INPUTMAPPER->AddDefaultMappingsForCurrentGameIfUnmapped();
        m_ChangeOccurred= true;

        BitmapText *pText = pKey->m_textMappedTo[m_CurController][m_CurSlot];
        pText->PlayCommand("MappedInput");
        if(m_InSetListMode)
        {
            pText->PlayCommand("LoseMark");
            ++m_SetListCurrent;
            if(m_SetListCurrent == m_SetList.end())
            {
                m_InSetListMode= false;
                m_SetList.clear();
            }
            else
            {
                BeforeChangeFocus();
                SetCursorFromSetListCurrent();
                AfterChangeFocus();
                StartWaitingForPress();
            }
        }
        Refresh();
        SCREENMAN->PlayStartSound();
    }
    //LOG->Trace("Update fin");
}


/* Note that this isn't necessarily correct.  For example, JOY_LEFT might actually be
 * a D-pad and not an axis.  All this is actually doing is giving priority to some
 * inputs over others; this function is unsuitable for other use. */
static bool IsAxis( const DeviceInput& DeviceI )
{
    if( !DeviceI.IsJoystick() )
        return false;

    static int axes[] =
            {
                    JOY_LEFT, JOY_RIGHT, JOY_UP, JOY_DOWN,
                    JOY_LEFT_2, JOY_RIGHT_2, JOY_UP_2, JOY_DOWN_2,
                    JOY_Z_UP, JOY_Z_DOWN,
                    JOY_ROT_UP, JOY_ROT_DOWN, JOY_ROT_LEFT, JOY_ROT_RIGHT, JOY_ROT_Z_UP, JOY_ROT_Z_DOWN,
                    JOY_HAT_LEFT, JOY_HAT_RIGHT, JOY_HAT_UP, JOY_HAT_DOWN,
                    JOY_AUX_1, JOY_AUX_2, JOY_AUX_3, JOY_AUX_4,
                    -1
            };

    for( int ax = 0; axes[ax] != -1; ++ax )
        if( DeviceI.button == axes[ax] )
            return true;

    return false;
}

bool ScreenMapControllersPump::Input( const InputEventPlus &input )
{
    if(m_fLockInputSecs > 0.0f)
    {
        return false;
    }


    if(m_AutoDismissSanitySecs > 0.0f)
    {
        if(input.type == IET_FIRST_PRESS &&
           input.DeviceI.device == DEVICE_KEYBOARD &&
           input.DeviceI.button == KEY_ENTER)
        {
            m_AutoDismissSanitySecs = 0.0f;
            m_SanityMessage->PlayCommand("TweenOff");
        }
        return false;
    }

    if( input.type != IET_FIRST_PRESS && input.type != IET_REPEAT )
    {
        return false;	// ignore
    }
    if( IsTransitioning() )
    {
        return false;	// ignore
    }

    // Whoever wants it can uncomment this log spew, I don't think it's necessary. -Kyz
    // LOG->Trace( "ScreenMapControllersPump::Input():  device: %d, button: %d", 
    // 	input.DeviceI.device, input.DeviceI.button );

    int button = input.DeviceI.button;
    bool bHandled = false;

    /* TRICKY: Some adapters map the PlayStation digital d-pad to both axes and
     * buttons. We want buttons to be used for any mappings where possible
     * because presses of buttons aren't mutually exclusive and presses of axes
     * are (e.g. can't read presses of both Left and Right simultaneously). So,
     * when the user presses a button, we'll wait until the next Update before
     * adding a mapping so that we get a chance to see all input events the 
     * user's press of a panel. Prefer non-axis events over axis events. */
    if( !m_WaitingForPress.IsZero() )
    {
        if( input.type != IET_FIRST_PRESS )
            return false;

        // Don't allow function keys to be mapped.
        if( input.DeviceI.device == DEVICE_KEYBOARD && (input.DeviceI.button >= KEY_F1 && input.DeviceI.button <= KEY_F12) )
        {
            SCREENMAN->SystemMessage( INVALID_BUTTON );
            SCREENMAN->PlayInvalidSound();
        }
        else
        {
            if( m_DeviceIToMap.IsValid() &&
                !IsAxis(m_DeviceIToMap) &&
                IsAxis(input.DeviceI) )
            {
                LOG->Trace("Ignored input; non-axis event already received");
                return false;	// ignore this press
            }

            m_DeviceIToMap = input.DeviceI;
        }
    }
    else if( input.DeviceI.device == DEVICE_KEYBOARD )
    {
        switch( button )
        {
            /* We only advertise space as doing this, but most games use either delete
             * or backspace, and I find them more intuitive, so allow them, too. -gm */

            /* XXX: For some reason that eludes me, this function gets sent an
             * KEY_SPACE button press every time the JOY_HAT_UP button is pressed.
             * Had to put this in to prevent mappings being erased everytime the user
             * pressed up on the joypad. */

            case KEY_DEL:
            case KEY_SPACE:
            case KEY_BACK: // Clear the selected input mapping.
                if(!CursorOnKey())
                {
                    break;
                }
                {
                    const KeyToMap *pKey = &m_KeysToMap[CurKeyIndex()];
                    GameInput curGameI( (GameController)m_CurController, pKey->m_GameButton );
                    if( !INPUTMAPPER->ClearFromInputMap(curGameI, m_CurSlot) )
                        break;

                    INPUTMAPPER->AddDefaultMappingsForCurrentGameIfUnmapped();

                    m_soundDelete.Play(true);
                    bHandled = true;
                }
                break;
            case KEY_LEFT: // Move the selection left, wrapping up.
                if(!CursorCanGoLeft())
                {
                    break;
                }
                BeforeChangeFocus();
                if(m_CurSlot == 0)
                {
                    m_CurSlot = NUM_CHANGABLE_SLOTS-1;
                    --m_CurController;
                }
                else
                {
                    --m_CurSlot;
                }
                AfterChangeFocus();
                m_soundChange.Play(true);
                bHandled = true;
                break;
            case KEY_RIGHT:	// Move the selection right, wrapping down.
                if(!CursorCanGoRight())
                {
                    break;
                }
                BeforeChangeFocus();
                m_CurSlot++;
                if( m_CurSlot > NUM_CHANGABLE_SLOTS-1 )
                {
                    m_CurSlot = 0;
                    m_CurController++;
                }
                AfterChangeFocus();
                m_soundChange.Play(true);
                bHandled = true;
                break;
            case KEY_UP: // Move the selection up.
                if(!CursorCanGoUp())
                {
                    break;
                }
                BeforeChangeFocus();
                m_CurButton--;
                AfterChangeFocus();
                m_soundChange.Play(true);
                bHandled = true;
                break;
            case KEY_DOWN: // Move the selection down.
                if(!CursorCanGoDown())
                {
                    break;
                }
                BeforeChangeFocus();
                m_CurButton++;
                AfterChangeFocus();
                m_soundChange.Play(true);
                bHandled = true;
                break;
            case KEY_ESC: // Quit the screen.
                ExitAction();
                bHandled = true;
                break;
            case KEY_Cm:
                if(CursorOnKey())
                {
                    SetListEntry to_add(SetListEntry(CurKeyIndex(), m_CurController, m_CurSlot));
                    set<SetListEntry>::iterator found= m_SetList.find(to_add);
                    if(found == m_SetList.end())
                    {
                        m_SetList.insert(to_add);
                        GetActorWithFocus()->PlayCommand("GainMark");
                    }
                    else
                    {
                        m_SetList.erase(found);
                        GetActorWithFocus()->PlayCommand("LoseMark");
                    }
                }
                break;
            case KEY_ENTER: // Change the selection.
            case KEY_KP_ENTER:
                bHandled = true;
                if(CursorOnAction())
                {
                    (this->*m_Actions[CurActionIndex()].m_action)();
                    SCREENMAN->PlayStartSound();
                    break;
                }
                if(CursorOnHeader())
                {
                    break;
                }
                StartWaitingForPress();
                SCREENMAN->PlayStartSound();
                break;
        }
    }

//	ScreenWithMenuElements::Input( input );	// default handler

    // This trace is also useless log spew.  Create a preference or something
    // configurable if you disagree. -Kyz
    // LOG->Trace( "m_CurSlot: %d m_CurController: %d m_CurButton: %d", m_CurSlot, m_CurController, m_CurButton );

    Refresh();
    return bHandled;
}

Actor *ScreenMapControllersPump::GetActorWithFocus()
{
    if(CursorOnAction())
    {
        LOG->Trace("CursorOnAction=true");
        return m_Actions[CurActionIndex()].m_actor;
    }
    LOG->Trace("CursorOnAction=false, returning a key binding");
    const KeyToMap *pKey = &m_KeysToMap[CurKeyIndex()];
    //LOG->Trace("%p",pKey);
    return pKey->m_textMappedTo[GameController_1][m_CurSlot];
}

void ScreenMapControllersPump::BeforeChangeFocus()
{
    Actor *pActor = GetActorWithFocus();
    pActor->PlayCommand( "LoseFocus" );
}

void ScreenMapControllersPump::AfterChangeFocus()
{
    Actor *pActor = GetActorWithFocus();
    pActor->PlayCommand( "GainFocus" );
}

void ScreenMapControllersPump::Refresh()
{

    LOG->Trace("Call refresh");
    for( unsigned b=0; b<m_KeysToMap.size(); b++ )
    {
        const KeyToMap *pKey = &m_KeysToMap[b];
        for( int s=0; s<NUM_SHOWN_GAME_TO_DEVICE_SLOTS; s++ )
        {
            BitmapText *pText = pKey->m_textMappedTo[GameController_1][s];
            GameInput cur_gi( GameController_1, pKey->m_GameButton );
            DeviceInput di;
            RString sText = "-----------";
            if( INPUTMAPPER->GameToDevice( cur_gi, s, di ) )
                sText = INPUTMAN->GetDeviceSpecificInputString( di );
            pText->SetText( sText );
        }
    }

    m_LineScroller.SetDestinationItem(
            static_cast<float>(min(m_CurButton, m_MaxDestItem)));
    m_UserScroller.SetDestinationItem(0);
    for (unsigned i = 0; i<m_UserScrollerActors.size(); i++)
    {
        if (i==CurKeyIndex())
            m_UserScrollerActors[i]->PlayCommand("GainFocus");
        else
            m_UserScrollerActors[i]->PlayCommand("LoseFocus");
    }
    LOG->Trace("Refresh complete");
}

bool ScreenMapControllersPump::CursorOnAction()
{
    // We have a header row, the rows for the keys, and the action rows.
    // So every row after m_KeysToMap.size is an action row.
    return m_CurButton >= m_KeysToMap.size();
}

bool ScreenMapControllersPump::CursorOnHeader()
{
    // We have a header row before all others.
    // So the header is at 0.
    //return m_CurButton == 0;
    return false;
}

bool ScreenMapControllersPump::CursorOnKey()
{
    return !(CursorOnHeader() || CursorOnAction());
}

bool ScreenMapControllersPump::CursorCanGoUp()
{
    return m_CurButton > 0;
}

bool ScreenMapControllersPump::CursorCanGoDown()
{
    return m_CurButton < m_KeysToMap.size() + m_Actions.size() - 1;
}

bool ScreenMapControllersPump::CursorCanGoLeft()
{
    return !CursorOnAction() && (m_CurSlot > 0 || m_CurController > 0);
}

bool ScreenMapControllersPump::CursorCanGoRight()
{
    return !CursorOnAction() && (m_CurSlot < NUM_CHANGABLE_SLOTS-1);
}

int ScreenMapControllersPump::CurKeyIndex()
{
    // The header row is at 0, so subtract 1 from m_CurButton.
    //LOG->Trace("m_CurButton: %i",m_CurButton);
    return m_CurButton;
}

int ScreenMapControllersPump::CurActionIndex()
{
    // Subtract the header row and the keys.
    return m_CurButton - m_KeysToMap.size();
}

void ScreenMapControllersPump::SetCursorFromSetListCurrent()
{
    m_CurButton= m_SetListCurrent->m_button + 1;
    m_CurController= m_SetListCurrent->m_controller;
    m_CurSlot= m_SetListCurrent->m_slot;
}

void ScreenMapControllersPump::StartWaitingForPress()
{
    const KeyToMap *pKey = &m_KeysToMap[CurKeyIndex()];
    BitmapText *pText = pKey->m_textMappedTo[GameController_1][m_CurSlot];
    pText->PlayCommand( "Waiting" );
    m_WaitingForPress.Touch();
    m_DeviceIToMap.MakeInvalid();
}

void ScreenMapControllersPump::HandleScreenMessage(const ScreenMessage SM)
{
    if(SM == SM_DoSaveAndExit)
    {
        switch(ScreenPrompt::s_LastAnswer)
        {
            DEFAULT_FAIL(ScreenPrompt::s_LastAnswer);
            case ANSWER_YES:
                SaveToDisk();
                StartTransitioningScreen(SM_GoToNextScreen);
                break;
            case ANSWER_NO:
                ReloadFromDisk();
                StartTransitioningScreen(SM_GoToNextScreen);
                break;
            case ANSWER_CANCEL:
                break;
        }
    }
    else
    {
        ScreenWithMenuElements::HandleScreenMessage(SM);
    }
}

void ScreenMapControllersPump::HandleMessage( const Message &msg )
{
    if( msg == Message_AutoJoyMappingApplied )
    {
        Refresh();
    }

    ScreenWithMenuElements::HandleMessage( msg );
}

void ScreenMapControllersPump::ClearToDefault()
{
    INPUTMAPPER->ResetMappingsToDefault();
}

void ScreenMapControllersPump::ReloadFromDisk()
{
    INPUTMAPPER->ReadMappingsFromDisk();
}

void ScreenMapControllersPump::SaveToDisk()
{
    if(SanityCheckWrapper())
    {
        INPUTMAPPER->SaveMappingsToDisk();
        m_ChangeOccurred= false;
    }
}

void ScreenMapControllersPump::SetListMode()
{
    if(m_SetList.size() < 1)
    {
        m_NoSetListPrompt->PlayCommand("TweenOn");
        m_AutoDismissNoSetListPromptSecs= THEME->GetMetricF(m_sName, "AutoDismissNoSetListPromptSecs");
    }
    else
    {
        m_SetListCurrent= m_SetList.begin();
        m_InSetListMode= true;
        BeforeChangeFocus();
        SetCursorFromSetListCurrent();
        AfterChangeFocus();
        StartWaitingForPress();
    }
}

void ScreenMapControllersPump::ExitAction()
{
    if(m_ChangeOccurred)
    {
        // If the current mapping doesn't pass the sanity check, then the user
        // can't navigate the prompt screen to pick a choice. -Kyz
        if(SanityCheckWrapper())
        {
            ScreenPrompt::Prompt(SM_DoSaveAndExit, SAVE_PROMPT,
                                 PROMPT_YES_NO_CANCEL, ANSWER_YES);
        }
    }
    else
    {
        SCREENMAN->PlayStartSound();
        StartTransitioningScreen(SM_GoToNextScreen);
    }
}

bool ScreenMapControllersPump::SanityCheckWrapper()
{
    vector<RString> reasons_not_sane;
    INPUTMAPPER->SanityCheckMappings(reasons_not_sane);
    if(reasons_not_sane.empty())
    {
        return true;
    }
    else
    {
        FOREACH(RString, reasons_not_sane, reason)
        {
            *reason= THEME->GetString("ScreenMapControllers", *reason);
        }
        RString joined_reasons= join("\n", reasons_not_sane);
        joined_reasons= THEME->GetString("ScreenMapControllers", "VitalButtons") + "\n" + joined_reasons;
        Message msg("SetText");
        msg.SetParam("Text", joined_reasons);
        m_SanityMessage->HandleMessage(msg);
        m_AutoDismissSanitySecs= THEME->GetMetricF(m_sName, "AutoDismissSanitySecs");
        return false;
    }
}

void ScreenMapControllersPump::ActionRow::Load(RString const& scr_name,
                                           RString const& name, ScreenMapControllersPump::action_fun_t action,
                                           ActorFrame* line, ActorScroller* scroller)
{
    m_action= action;
    RString lower_name= name;
    lower_name.MakeLower();
    // Make the specific actor optional, use a fallback if it doesn't exist.
    RString path= THEME->GetPathG(scr_name, lower_name, true);
    if(path.empty())
    {
        path= THEME->GetPathG(scr_name, "action");
    }
    m_actor.Load(path);
    m_actor->SetName(name);
    ActorUtil::LoadAllCommands(*m_actor, scr_name);
    line->AddChild(m_actor);
    scroller->AddChild(line);
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

    This file incorporates work covered by the following copyright and
    permission notice:

     * (c) 2001-2005 Chris Danford, Glenn Maynard
     * 2014 Eric Reese
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