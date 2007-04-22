#ifndef CARD_SPRITE_H
#define CARD_SPRITE_H
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

// Qt includes
#include <QGraphicsPixmapItem>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QPixmap>

// Local includes
#include "thememanager.h"
#include "deck.h"


/**
 * The sprite for a card on the canvas.
 */
class CardSprite : public QGraphicsPixmapItem, public virtual Themable
{

  public:
    /** Constructor for the sprite.
     *  @param parent The parent window
     */
    CardSprite(const Suite suite, const CardType type, ThemeManager* theme, int advancePeriod, QGraphicsScene* scene);

    // Possible animation states of the sprite
    enum AnimationState {Idle, Turning, Moving, Removing,
                         ShuffleMove};

    /** Main theme manager function. Called when any theme change like
      * a new theme or a theme size change occurs. This object needs to
      * resiez and redraw then.
      */
    virtual void changeTheme();

    /** Display the card front pixmap image
     */
     void setFrontside();

    /** Display the card back pixmap image
     */
     void setBackside();


     /** Standard advance method
      *  @param phase Advance phase
      */
     virtual void advance(int phase);

      /** Stop all movment and animation.
      */
      void stop();

     /** Turn the card to frontside or backside.
       * @param front True (turn to frontside) or false (turn to backside)
       */
     void setTurning(bool front);

     /** Move the sprite slowly to the target area. Stop
       * movement if it arrived there.
       * @param tx Target x
       * @param ty Target y
       */
     void setMove(QPointF pos, double time);

     void setPosition(QPointF pos);

     /** Move the sprite slowly to the target area. Stop
       * movement and hide sprite if it arrived there.
       * @param tx Target x
       * @param ty Target y
       */
     void setRemove(QPointF pos, double time);

     /** Delay before moving, then move the sprite fast to the
       * target area. Stop movement and depending on the last
       * argument turn backside/frontside sprite if it arrived there.
       * @param tx Target x
       * @param ty Target y
       * @param delay The Move start delay 
       * @param front Shoe frontside(true) or backside
       */
     void setShuffleMove(QPointF pos, int delay, bool front);

    /** Set the current frame.
     *  @param no The frame number 0..n-1
      * @param force Force redraw
     */
    void setFrame(int no, bool force=false);

    void calcFrame(int no);

    int count();

    int cardid() {return mCardType*4+mSuite;}

    /** Retrieve the current frame number.
     *  @return The frame number 0..n-1.
     */
     int frame() {return mCurrentFrame;}
  
  protected:
    QPixmap createCard(const QPixmap front, const QPixmap back, int curNo, int count);

    /** Set target position and calculate moving speed.
     *  @param tx Target x [0..1]
     *  @param ty Target y [0..1]
     *  @param time The movement speed [canvas cycles to reach target]
     */
     void calcTargetAndSpeed(QPointF pos, double time);

     /** Perform a move by a delta given by the sprites velocity.
      *  @param return True if the target position is reached false otherwise.
      */
     bool deltaMove();
    

  private:
    // Type of animation
    AnimationState mAnimationState;  
    // Counter of animation
    int mAnimationCnt;
    // Target point for movment
    QPointF mMoveTarget;
    // Movement speed
    double mMoveSpeedX;
    double mMoveSpeedY;
    // Front/Backside flag
    bool mFrontFlag;
    // Store our pixmap array and its offset values
    QList<QPixmap> mFrames;
    QList<QPointF> mHotspots;
    // The current frame
    int mCurrentFrame;

    // Theme data
    int mAdvancePeriod;
    int mDelay;
    double mWidth;
    Suite mSuite;
    CardType mCardType;

    /** The position of the sprite [rel 0..1, rel 0..1]
     */
    QPointF mStart;


};

#endif
