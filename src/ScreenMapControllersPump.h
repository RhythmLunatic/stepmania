//
// Created by rl on 5/17/21.
//

#ifndef STEPMANIA_SCREENMAPCONTROLLERSPUMP_H
#define STEPMANIA_SCREENMAPCONTROLLERSPUMP_H

//#include "ScreenMapControllers.h"
#include "ScreenWithMenuElements.h"
#include "BitmapText.h"
#include "InputMapper.h"
#include "ActorScroller.h"
#include "RageSound.h"

class ScreenMapControllersPump : public ScreenWithMenuElements {

public:
    ScreenMapControllersPump();
    ~ScreenMapControllersPump();
    virtual void Init();
    virtual void BeginScreen();

    virtual void Update( float fDeltaTime );
    virtual bool Input( const InputEventPlus &input );
    virtual void HandleMessage( const Message &msg );
    virtual void HandleScreenMessage( const ScreenMessage SM );

private:
    Actor *GetActorWithFocus();
    void BeforeChangeFocus();
    void AfterChangeFocus();
    void Refresh();
    void DismissWarning();
    bool CursorOnAction();
    bool CursorOnHeader();
    bool CursorOnKey();
    bool CursorCanGoUp();
    bool CursorCanGoDown();
    bool CursorCanGoLeft();
    bool CursorCanGoRight();
    int CurKeyIndex();
    int CurActionIndex();
    void SetCursorFromSetListCurrent();
    void StartWaitingForPress();

    unsigned int m_CurController;
    unsigned int m_CurButton;
    unsigned int m_CurSlot;
    unsigned int m_MaxDestItem;

    bool m_ChangeOccurred;

    RageTimer m_WaitingForPress;
    DeviceInput m_DeviceIToMap;

    /*struct KeyToMap
    {
        GameButton m_GameButton;

        // owned by m_Line
        BitmapText	*m_textMappedTo;
    };
    vector<KeyToMap> m_KeysToMap;*/
    struct KeyToMap
    {
        GameButton m_GameButton;

        // owned by m_Line
        BitmapText	*m_textMappedTo[NUM_GameController][NUM_SHOWN_GAME_TO_DEVICE_SLOTS];
    };
    vector<KeyToMap> m_KeysToMap;

    BitmapText m_textDevices;

    BitmapText m_textLabel[NUM_GameController];
    BitmapText m_ListHeaderCenter;
    BitmapText m_ListHeaderLabels[NUM_GameController][NUM_SHOWN_GAME_TO_DEVICE_SLOTS];

    float m_AutoDismissNoSetListPromptSecs;
    AutoActor m_NoSetListPrompt;

    float m_AutoDismissSanitySecs;
    AutoActor m_SanityMessage;

    ThemeMetric<LuaReference> USERSCROLLER_TRANSFORM;
    ActorScroller m_UserScroller; //User handled actors
    vector<AutoActor> m_UserScrollerActors;

    struct SetListEntry
    {
        int m_button;
        int m_controller;
        int m_slot;
        SetListEntry(int b, int c, int s)
                :m_button(b), m_controller(c), m_slot(s) {}
        bool operator<(SetListEntry const& rhs) const
        {
            if(m_controller != rhs.m_controller)
            {
                return m_controller < rhs.m_controller;
            }
            if(m_button != rhs.m_button)
            {
                return m_button < rhs.m_button;
            }
            return m_slot < rhs.m_slot;
        }
    };
    set<SetListEntry> m_SetList;
    set<SetListEntry>::iterator m_SetListCurrent;
    bool m_InSetListMode;

    typedef void (ScreenMapControllersPump::* action_fun_t)();
    struct ActionRow
    {
        RString m_name;
        AutoActor m_actor;
        action_fun_t m_action;
        void Load(RString const& scr_name, RString const& name,
                  ScreenMapControllersPump::action_fun_t action, ActorFrame* line,
                  ActorScroller* scroller);
    };
    void ClearToDefault();
    void ReloadFromDisk();
    void SaveToDisk();
    void SetListMode();
    void ExitAction();
    bool SanityCheckWrapper();

    vector<ActionRow> m_Actions;

    //Holds every BitmapText actor
    vector<ActorFrame*> m_Line;
    //Holds the rows. Each row contains an ActorFrame with an array of actors inside it, these make up the columns on the screen.
    ActorScroller m_LineScroller;

    RageSound m_soundChange;
    RageSound m_soundDelete;
};


#endif //STEPMANIA_SCREENMAPCONTROLLERSPUMP_H

/*
 *  (c) 2020 Rhythm Lunatic
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