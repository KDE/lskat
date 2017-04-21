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

#include "cardsprite.h"

// General includes
#include <math.h>

// Qt includes
#include <QPoint>
#include <QMatrix>
#include <QGraphicsScene>

// KDE includes
#include "lskat_debug.h"
#include <kconfiggroup.h>

// Delay for card turning animation [ms]
#define ANIM_CNT_TURNING          20.0
// Time in [ms] for a shuffle move
#define SHUFFLEMOVE_TIME 100.0

// Theme manager stuff
#define THEME_ID "card"

// Constructor for the view
CardSprite::CardSprite(const Suite suite, const CardType cardtype, ThemeManager *theme,
                       int advancePeriod, QGraphicsScene *scene)
          : Themable(QLatin1String(THEME_ID), theme), QGraphicsPixmapItem(0)

{
    scene->addItem(this);
    mAnimationState = Idle;
    mCurrentFrame   = -1; // Frame will be set to backside
    mAdvancePeriod  = advancePeriod;
    mSuite          = suite;
    mCardType       = cardtype;
    mFrames.clear();

    // Redraw
    if (theme) theme->updateTheme(this);
}

// Main themable function. Called for any theme change. The sprites needs to
// resize and redraw here.
void CardSprite::changeTheme()
{
    // Get scaling change
    double oldscale = this->getScale();
    double scale    = thememanager()->getScale();
    Themable::setScale(scale);

    // Retrieve theme data from configuration
    KConfigGroup config = thememanager()->config(id());
    double width        = config.readEntry("width", 1.0);
    width *= scale;
    mWidth = width; // Store for later use

    // Z-value by program only
    // Nothing to do

    // Animation
    int startFrame      = config.readEntry("start-frame", 0);
    int endFrame        = config.readEntry("end-frame", 0);

    // Pos py program only
    setPos(x() * scale / oldscale, y() * scale / oldscale);

    // Frames (loaded on demand)
    mFrames.clear();
    mHotspots.clear();
    QPixmap nullPixmap;
    for (int i = startFrame; i <= endFrame; i++)
    {
        mFrames.append(nullPixmap);
        mHotspots.append(QPointF(0.0, 0.0));
    }

    // Start with backside to save calculation time
    if (mCurrentFrame < 0) mCurrentFrame = endFrame;

    // Set pixmap to sprite
    setFrame(mCurrentFrame, true);
    update();
}

// Stop all movement and animation
void CardSprite::stop()
{
    mAnimationState = Idle;
    mTime           = 0.0;
    setBackside();
}

// Set the current animation mode of this sprite
void CardSprite::setTurning(bool front)
{
    mFrontFlag      = front;
    mAnimationState = Turning;
    mTime           = 0.0;
}

// Set target position and calculate moving speed.
void CardSprite::calcTargetAndSpeed(QPointF pos, double time)
{
    // Convert time from [ms] to advance cycles
    time = time / mAdvancePeriod;

    double scale = getScale();
    mMoveTarget  = pos;
    // Calculate move speed so that the duration for the move
    // is fixed
    double dx    = mMoveTarget.x() - x() / scale;
    double dy    = mMoveTarget.y() - y() / scale;
    double angle = atan2(dy, dx);
    mMoveSpeedX  = cos(angle) * sqrt(dx * dx + dy * dy) / time;
    mMoveSpeedY  = sin(angle) * sqrt(dx * dx + dy * dy) / time;
}

// Move the sprite to the given relative position
void CardSprite::setPosition(QPointF pos)
{
    mStart       = pos;
    setPos(mStart.x() * getScale(), mStart.y() * getScale());
}

// Move the sprite slowly to the target area. Stop movement if it arrived there.
void CardSprite::setMove(QPointF pos, double time)
{
    mAnimationState = Moving;
    mTime           = 0.0;
    calcTargetAndSpeed(pos, time);
}

// Move the sprite slowly to the target area. Stop
// movement and remove sprite if it arrived there.
void CardSprite::setRemove(QPointF pos, double time)
{
    mAnimationState = Removing;
    calcTargetAndSpeed(pos, time);
}

// Delay before moving, then move the sprite fast to the
// target area. Stop movement and depending on the last
// argument turn backside/frontside sprite if it arrived there.
void CardSprite::setShuffleMove(QPointF pos, double delay, bool front)
{
    setBackside();
    calcTargetAndSpeed(pos, SHUFFLEMOVE_TIME);
    mAnimationState = ShuffleMove;
    mTime           = delay;
    mFrontFlag      = front;
}

// Display the card front pixmap image
void CardSprite::setFrontside()
{
    // Choose card front frame
    setFrame(0);
}

// Display the card back pixmap image
void CardSprite::setBackside()
{
    // Choose card back frame (last one in the animation sequence)
    setFrame(mFrames.size() - 1);
}

int CardSprite::count()
{
    return mFrames.count();
}

// Set a new bitmap into the sprite. If the number is the same as the
// current one, nothing is done.
void CardSprite::setFrame(int no, bool force)
{
    if (!force && no == mCurrentFrame) return;
    if (no < 0 || no >= mFrames.count()) return;

    // Calculate Pixmap (only done if necessary)
    calcFrame(no);

    // Set frame pixmap
    QPixmap pixmap = mFrames.at(no);
    setPixmap(pixmap);

    // Translation
    QPoint offset = thememanager()->getOffset();
    resetTransform();
    setTransform(QTransform::fromTranslate(mHotspots[no].x() + offset.x(), mHotspots[no].y() + offset.y()), true);

    mCurrentFrame = no;
    update();
}

// Calculate a pixmap for a frame. Only calculates it if it
// is not previously stored to avoid double calculations.
void CardSprite::calcFrame(int no)
{
    QPixmap pixmap = mFrames.at(no);
    // Check whether frame is already loaded
    if (pixmap.isNull())
    {
        double dx = 0.0;
        double dy = 0.0;
        // Frontside
        if (no == 0)
        {
            pixmap = thememanager()->getCard(mSuite, mCardType, mWidth);
        }
        // Backside
        else if (no >= mFrames.count() - 1)
        {
            pixmap = thememanager()->getCardback(mWidth);
        }
        // Animation
        else
        {
            QPixmap front = thememanager()->getCard(mSuite, mCardType, mWidth);
            QPixmap back  = thememanager()->getCardback(mWidth);
            pixmap        = createCard(front, back, no, mFrames.count());
            dx            = (front.width() - pixmap.width()) / 2.0;
            dy            = (front.height() - pixmap.height()) / 2.0;
        }
        mFrames[no]   = pixmap;
        mHotspots[no] = QPointF(dx, dy);
    }
}

// Perform a move by a delta given by the sprites velocity.
// Returns true if the target position is reached
bool CardSprite::deltaMove()
{
    // Calculate difference vector
    double scale = getScale();
    double dx    = mMoveTarget.x() - x() / scale;
    double dy    = mMoveTarget.y() - y() / scale;

    // Check arrival at target
    if (dx * dx + dy * dy < mMoveSpeedX * mMoveSpeedX + mMoveSpeedY * mMoveSpeedY)
    {
        setPosition(mMoveTarget);
        return true;
    }
    // Move towards target by given velocity
    else
    {
        setPosition(QPointF(x() / scale + mMoveSpeedX, y() / scale + mMoveSpeedY));
        return false;
    }
}

// CanvasItem advance method
void CardSprite::advance(int phase)
{
    // Ignore phase 0 (collisions)
    if (phase == 0)
    {
        QGraphicsItem::advance(phase);
        return;
    }

    // Turn a card
    if (mAnimationState == Turning)
    {
        mTime += mAdvancePeriod;
        // Turn delay counter
        if (mTime >= ANIM_CNT_TURNING)
        {
            mTime = 0.0;
            // Check whether animation is over
            if ((mFrontFlag && frame() == 0) ||
                (!mFrontFlag && frame() == mFrames.size() - 1))
            {
                mAnimationState = Idle;
            }
            else
            {
                if (mFrontFlag) setFrame(frame() - 1);
                else setFrame(frame() + 1);
            }
        }
    }// end if Turning

    // Move a card
    else if (mAnimationState == Moving)
    {
        // Perform a move by a delta given by the sprites velocity.
        if (deltaMove())
        {
            mAnimationState = Idle;
        }
    }// end if Moving

    // Move a card
    else if (mAnimationState == Removing)
    {
        // Perform a move by a delta given by the sprites velocity.
        if (deltaMove())
        {
            // Turn to backside
            setTurning(false);
        }
    }// end if Removing

    // Shuffle move a card
    else if (mAnimationState == ShuffleMove)
    {
        // First delay move until counter is run down
        if (mTime > 0.0)
        {
            mTime -= mAdvancePeriod;
        }
        // Then move to target position
        else
        {
            // Perform a move by a delta given by the sprites velocity.
            if (deltaMove())
            {
                if (mFrontFlag) mAnimationState = Turning;
                else mAnimationState = Idle;
                mTime = 0.0;
            }
        }
    }// end if ShuffleMove

    QGraphicsItem::advance(phase);
}

// Create turn animation, i.e. card combined out of backside and frontside
QPixmap CardSprite::createCard(const QPixmap &front, const QPixmap &back, int curNo, int count)
{
    int halfCount = count / 2;
    // Turn the frontside of the card 0..90 degree
    if (curNo < halfCount)
    {
        QMatrix m;
        // Get an angle eps..90 deg for the values i is running
        double angle = (double)curNo / (double)halfCount * 90.0;
        // Conversion to rad
        angle = angle / 180.0 * M_PI;
        // Scale pixmap to simulate rotating card
        m.scale(cos(angle), 1.0);
        QPixmap pm = front.transformed(m, Qt::SmoothTransformation);
        return pm;
    }

    // Turn the backside of the card 90..eps degree
    else
    {
        QMatrix m;
        // Get an angle 0..90 deg for the values i is running
        double angle = 90.0 - ((double)(curNo - halfCount + 1) / (double)halfCount * 90.0);
        // Conversion to rad
        angle = angle / 180.0 * M_PI;
        // Scale pixmap to simulate rotating card
        m.scale(cos(angle), 1.0);
        QPixmap pm = back.transformed(m, Qt::SmoothTransformation);
        return pm;
    }
}
