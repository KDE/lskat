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


/**
 * The sprite for a card on the canvas.
 */
class CardSprite : public QGraphicsPixmapItem
{

  public:
    /** Constructor for the sprite.
     *  @param parent The parent window
     */
    CardSprite(const QPixmap* front, const QPixmap* back, QGraphicsScene* canvas);

    // Possible animation states of the sprite
    enum AnimationState {Idle, Turning, Moving, Removing,
                         ShuffleMove};

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
     void setMove(int tx, int ty);

     /** Move the sprite slowly to the target area. Stop
       * movement and hide sprite if it arrived there.
       * @param tx Target x
       * @param ty Target y
       */
     void setRemove(int tx, int ty);

     /** Delay before moving, then move the sprite fast to the
       * target area. Stop movement and depending on the last
       * argument turn backside/frontside sprite if it arrived there.
       * @param tx Target x
       * @param ty Target y
       * @param delay The Move start delay 
       * @param front Shoe frontside(true) or backside
       */
     void setShuffleMove(int tx, int ty, int delay, bool front);

    /** Update the sprites card graphics by the pixmaps given 
     *  as parameter.
     *  @param front  The frontside pixmap.
     *  @param back   The backside pixmap.
     *  @return The generated canvas pixmap array.
     */
    void updateGraphics(QPixmap* front, QPixmap* back);


    /** Factory constructor method for a new card sprite. The sprite
     *  with frontside and backside as well as an animation between
     *  them is generated.
     *  @param canvas The canvas
     *  @param front  The frontside pixmap.
     *  @param back   The backside pixmap.
     *  @return The generated sprite object.
     */
    static CardSprite* create(QGraphicsScene* canvas, const QPixmap* front, const QPixmap* back);

    /** Set the current frame.
     *  @param no The frame number 0..n-1
     */
    void setFrame(int no);

    /** Retrieve the current frame number.
     *  @return The frame number 0..n-1.
     */
     int frame() {return mCurrentFrame;}
  
  protected:
    /** Load and create card pixmap array. This method create the in-between
     *  sprites necessary.
     *  @param front      The frontside pixmap.
     *  @param back       The backside pixmap.
     *  @param spriteList The resulting pixmap list
     *  @param hotspots   The resulting hotspot offset list
     */
    static void createPixmapArray(const QPixmap* front, const QPixmap* back,
                                  QList<QPixmap>& spriteList, QList<QPointF>& hotspots);

    /** Set target position and calculate moving speed.
     *  @param tx Target x
     *  @param ty Target y
     *  @param time The movement speed [canvas cycles to reach target]
     */
     void calcTargetAndSpeed(int tx, int ty, double time);

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
    QPoint mMoveTarget;
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



};

#endif
