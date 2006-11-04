#ifndef DISPLAY_TWO_H
#define DISPLAY_TWO_H
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
#include <QPixmap>
#include <QHash>

// KDE includes
#include <kdebug.h>

// local includes
#include "abstractdisplay.h"
#include "deck.h"
#include "player.h"

// Forward declaration
class PixmapSprite;


/**
 * The display engine for a two player game.
 */
class DisplayTwo : public AbstractDisplay
{
  Q_OBJECT

  public:
    /** Constructor for the engine
     *  @param grafixDir The grafic directory
     *  @param deck The card deck
     *  @param scene The graphics scene to work with
     *  @param parent The parent object
     */
    DisplayTwo(QString grafixDir, Deck* deck, QGraphicsScene* scene, QObject* parent);

    /** Start the display.
     */
    void start(); 


    /* Init a player on a given position. Create sprites etc.
     * @param player The player object
     * @param position The position to place the player (0,1)
     */
    void deal(Player* player, int position);

    /** Play a card on the display. The card is moved from
     *  its current position to the card deposit.
     * @param cardNumber The card number (0-7)
     * @param playerNumber The player number (0-1)
     * @param phase Movement phase (1st part, 2nd part, etc) [optional]
     */
    void play(int cardNumber, int playerNumber, int phase);

    /** Turn a card on the display. The card is flipped backside
     *  to frontside.
     * @param cardNumber The card number (0-7)
     */
    void turn(int cardNumber);

    /** Remove the given card from the display.
     * @param winnerPosition  The position of the winner (0,1)
     * @param cardNumber The number of the card
     * @param delta Card offset from target position (0,1,2,...)
     */
    void remove(int winnerPosition, int cardNumber, int delta);
  
    /** Returns the time in milliseconds used for shuffling the
     *  initial cards.
     *  @return The time in milliseconds.
     */
    int shuffleTime();

    /** Display the score on the game board.
     * @param position Which player position
     * @param score The score to display
     */
    void showScore(int position, int score);

    /** Display a text on the game board.
     * @param text The text to display
     */
    void showText(QString text);

    /** Show the move icon for the given player
     *  @param no The player number (-1: show none)
     */
    void showMove(int no = -1);

  public slots:
    /** Convert the position of a mouse click to a logical
      * game position, that is position (up/down) and a 
      * card number (0-7)
      */
    void convertMousePress(QPoint mouse, int& playerNumber, int& cardNumber);

  protected:
    void calcXYFromNumber(int cardNumber, int& x, int& y);
    CardSprite* getCardSprite(int cardValue);
    CardSprite* getCardSprite(int position, int h, int cardNumber);
    CardSprite* getCardSprite(int position, int h, int y, int x);


  private:
    /** Pixmap for movement sprite */
    QPixmap* mMovePixmap;
    QHash<int,PixmapSprite*> mMoveSprites;
};

#endif
