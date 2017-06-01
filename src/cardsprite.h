/*
   This file is part of the KDE games lskat program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CARD_SPRITE_H
#define CARD_SPRITE_H

// Qt includes
#include <QGraphicsPixmapItem>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QPointF>

// Local includes
#include "thememanager.h"
#include "deck.h"

/**
 * The sprite for a card on the canvas.
 */
class CardSprite : public QGraphicsPixmapItem, public virtual Themable
{
public:
    /**
     * Constructor for the sprite.
     * @param suite         The suite of the card to use [Club, ...]
     * @param type          The card type [Ace, ...]
     * @param theme         The theme manager
     * @param advancePeriod The scene advance period [ms]
     * @param scene         The scene object
     */
    CardSprite(const Suite suite, const CardType type, ThemeManager *theme, int advancePeriod, QGraphicsScene *scene);

    /**
     * Possible animation states of the sprite
     */
    enum AnimationState {Idle, Turning, Moving, Removing, ShuffleMove};

    /**
     * Main theme manager function. Called when any theme change like
     * a new theme or a theme size change occurs. This object needs to
     * resize and redraw then.
     */
    void changeTheme() Q_DECL_OVERRIDE;

    /**
     * Display the card front pixmap image
     */
    void setFrontside();

    /**
     * Display the card back pixmap image
     */
    void setBackside();

    /**
     * Standard advance method
     * @param phase Advance phase
     */
    void advance(int phase) Q_DECL_OVERRIDE;

    /**
     * Stop all movement and animation.
     */
    void stop();

    /**
     * Turn the card to frontside or backside.
     * @param front True (turn to frontside) or false (turn to backside)
     */
    void setTurning(bool front);

    /**
     * Move the sprite slowly to the target area. Stop
     * movement if it arrived there.
     * @param pos  Target position [0..1]
     * @param time Time for the move [ms]
     */
    void setMove(QPointF pos, double time);

    /**
     * Set the position of the sprite.
     * @param pos  Target position [0..1]
     */
    void setPosition(QPointF pos);

    /**
     * Move the sprite slowly to the target area. Stop
     * movement and hide sprite if it arrived there.
     * @param pos  Target position [0..1]
     * @param time Time for the move [ms]
     */
    void setRemove(QPointF pos, double time);

    /**
     * Delay before moving, then move the sprite fast to the target area.
     * Stop movement and depending on the last argument turn backside/frontside
     * sprite if it arrived there.
     * @param pos   Target position [0..1]
     * @param delay The Move start delay [ms]
     * @param front Show frontside(true) or backside
     */
    void setShuffleMove(QPointF pos, double delay, bool front);

    /**
     * Set the current frame.
     * @param no    The frame number 0..n-1
     * @param force Force redraw
     */
    void setFrame(int no, bool force = false);

    /**
     * Retrieve the maximum frame number.
     * @return The frame count.
     */
    int count();

    /**
     * Retrieve the card id, a combination of suite and card type.
     * @return The card id.
     */
    int cardid() {return mCardType * 4 + mSuite;}

    /**
     * Retrieve the current frame number.
     * @return The frame number 0..n-1.
     */
    int frame() {return mCurrentFrame;}

    /**
     * Check whether the sprite is idle,
     * @return True if idle.
     */
    bool isIdle() {return mAnimationState == Idle;}

protected:
    /**
     * Calculate a sprite frame (for turning sprites).
     * Called when the frame is asked for.
     * @param no The frame number.
     */
    void calcFrame(int no);

    /**
     * Create the pixmap for one frame of the frame turning animation.
     * @param front The front pixmap
     * @param back  The back pixmap
     * @param curNo Which number in the animation sequence [0..count]
     * @param count How many frames in the animation
     * @return The calculated pixmap.
     */
    QPixmap createCard(const QPixmap &front, const QPixmap &back, int curNo, int count);

    /**
     * Set target position and calculate moving speed.
     * @param pos  Target [0..1, 0..1]
     * @param time The movement speed [ms]
     */
    void calcTargetAndSpeed(QPointF pos, double time);

    /**
     * Perform a move by a delta given by the sprites velocity.
     * @return True if the target position is reached false otherwise.
     */
    bool deltaMove();

private:
    // Type of animation
    AnimationState mAnimationState;
    // Counter of animation [ms]
    double mTime;
    // Target point for movement
    QPointF mMoveTarget;
    // Movement speed X
    double mMoveSpeedX;
    // Movement speed Y
    double mMoveSpeedY;
    // Front/Backside flag
    bool mFrontFlag;
    // Store our pixmap array
    QList<QPixmap> mFrames;
    // Store our pixmap offset values
    QList<QPointF> mHotspots;
    // The current frame number
    int mCurrentFrame;

    // Sprite advance period [ms]
    int mAdvancePeriod;
    // Card width
    double mWidth;
    // Suite of card
    Suite mSuite;
    // Type of card
    CardType mCardType;

    // The position of the sprite [rel 0..1, rel 0..1]
    QPointF mStart;
};

#endif
