#ifndef DISPLAY_INTRO_H
#define DISPLAY_INTRO_H
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
#include <QGraphicsScene>
#include <QTimer>


// KDE includes
#include <kdebug.h>

// local includes
#include "abstractdisplay.h"
#include "thememanager.h"
#include "deck.h"

// Forward declaration


/**
 * The display engine for a two player game.
 */
class DisplayIntro : public AbstractDisplay, public virtual Themable
{
  Q_OBJECT

  public:
    enum AnimState {Idle, Putting, Turning, Waiting, Clearing, Waiting2};

  
    /** Constructor for the engine
     *  @param grafixDir The grafic directory
     *  @param deck The card deck
     *  @param scene The graphics scene to work with
     *  @param parent The parent object
     */
    DisplayIntro(Deck* deck, QGraphicsScene* scene, ThemeManager* theme, int advancePeriod, QGraphicsView* parent);

    /** Main theme function. Called by the theme manager. Redraw and resize 
      * display.
      */
    virtual void changeTheme();

    /** Start the intro.
    */
    void start();


    /* Init a player on a given position. Create sprites etc.
     * @param player The player object
     * @param position The position to place the player (0,1)
     */
    void deal(Player* , int ) {}

    /** Play a card on the display. The card is moved from
     *  its current position to the card deposit.
     * @param cardNumber The card number (0-7)
     * @param playerNumber The player number (0-1)
     * @param phase Movement phase (1st part, 2nd part, etc) [optional]
     */
    void play(int , int , int ) {}

    /** Turn a card on the display. The card is flipped backside
     *  to frontside.
     * @param cardNumber The card number (0-7)
     */
    void turn(int ) {}

    /** Remove the given card from the display.
     * @param winnerPosition  The position of the winner (0,1)
     * @param cardNumber The number of the card
     * @param delta Card offset from target position (0,1,2,...)
     */
    void remove(int , int , int ) {}
  
  public slots:
    /** Convert the position of a mouse click to a logical
      * game position, that is position (up/down) and a 
      * card number (0-7)
      */
    void convertMousePress(QPoint , int& , int& ) {}

    void loop();

  protected:


  private:
    /** Timer for animation loop */
    QTimer* mTimer;
    /** Animation counter */
    int mAnimCnt;
    /** Animation state */
    AnimState mState;
};

#endif
