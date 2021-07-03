#ifndef STEPMANIA_SPRITEASYNC_H
#define STEPMANIA_SPRITEASYNC_H

#include "Sprite.h"
#include <thread>

class SpriteAsync: public Sprite {
public:
    /** @brief The Sprite's present state. */
    struct State
    {
        RectF rect;
        /** @brief The number of "seconds to show". */
        float fDelay;
    };

    SpriteAsync();
    SpriteAsync( const SpriteAsync &cpy );
    virtual ~SpriteAsync();

    virtual void Load(RageTextureID ID);
    virtual void SetTexture( RageTexture *pTexture);
    virtual void UnloadTexture();
    void LoadFromNode( const XNode* pNode );
    virtual SpriteAsync *Copy() const;

    virtual bool EarlyAbortDraw() const;
    virtual void DrawPrimitives();
    virtual void Update( float fDeltaTime );

    virtual void UpdateAnimationState();	// take m_fSecondsIntoState, and move to a new state

    virtual void EnableAnimation( bool bEnable );

    virtual int GetNumStates() const;
    void InitState();
    virtual void SetState( int iNewState );
    int GetState() { return m_iCurState; }
    virtual float GetAnimationLengthSeconds() const
    { return m_animation_length_seconds; }
    virtual void RecalcAnimationLengthSeconds();
    virtual void SetSecondsIntoAnimation( float fSeconds );
    void SetStateProperties(const vector<State>& new_states)
    { m_States= new_states; RecalcAnimationLengthSeconds(); SetState(0); }

    RString	GetTexturePath() const;

    void SetCustomTextureRect( const RectF &new_texcoord_frect );
    void SetCustomTextureCoords( float fTexCoords[8] );
    void SetCustomImageRect( RectF rectImageCoords );	// in image pixel space
    void SetCustomImageCoords( float fImageCoords[8] );
    void SetCustomPosCoords( float fPosCoords[8] );
    const RectF *GetCurrentTextureCoordRect() const;
    const RectF *GetTextureCoordRectForState( int iState ) const;
    void StopUsingCustomCoords();
    void StopUsingCustomPosCoords();
    void GetActiveTextureCoords(float fTexCoordsOut[8]) const;
    void StretchTexCoords( float fX, float fY );
    void AddImageCoords( float fX, float fY ); // in image pixel space
    virtual void SetEffectMode( EffectMode em ) { m_EffectMode = em; }

    //void LoadFromCached( const RString &sDir, const RString &sPath );

    void SetTexCoordVelocity(float fVelX, float fVelY);
    /**
     * @brief Scale the Sprite while maintaining the aspect ratio.
     *
     * It has to fit within and become clipped to the given parameters.
     * @param fWidth the new width.
     * @param fHeight the new height. */
    void ScaleToClipped( float fWidth, float fHeight );
    void CropTo( float fWidth, float fHeight );

    // Commands
    virtual void PushSelf( lua_State *L );


    void SetAllStateDelays(float fDelay);

    bool m_DecodeMovie;

    bool m_use_effect_clock_for_texcoords;
private:
    std::thread imageLoadingThread;
    virtual void LoadFromTexture(RageTextureID ID);
    void LoadStatesFromTexture();

    void DrawTexture( const TweenState *state );

    RageTexture* m_pTexture;

    vector<State> m_States;
    int		m_iCurState;
    /** @brief The number of seconds that have elapsed since we switched to this frame. */
    float	m_fSecsIntoState;
    float m_animation_length_seconds;

    EffectMode m_EffectMode;
    bool m_bUsingCustomTexCoords;
    bool m_bUsingCustomPosCoords;
    bool m_bSkipNextUpdate;
    /**
     * @brief Set up the coordinates for the texture.
     *
     * The first two parameters are the (x, y) coordinates for the top left.
     * The remaining six are for the (x, y) coordinates for bottom left,
     * bottom right, and top right respectively. */
    float m_CustomTexCoords[8];
    /**
     * @brief Set up the coordinates for the position.
     *
     * These are offsets for the quad the sprite will be drawn to.
     * The first two are the (x, y) offsets for the top left.
     * The remaining six are for the (x, y) coordinates for bottom left,
     * bottom right, and top right respectively.
     * These are offsets instead of a replacement for m_size to avoid
     * complicating the cropping code. */
    float m_CustomPosCoords[8];

    // Remembered clipped dimensions are applied on Load().
    // -1 means no remembered dimensions;
    float	m_fRememberedClipWidth, m_fRememberedClipHeight;

    float m_fTexCoordVelocityX;
    float m_fTexCoordVelocityY;

};


#endif //STEPMANIA_SPRITEASYNC_H
