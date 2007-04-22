#ifndef ABSTRACT_DISPLAY_H
#define ABSTRACT_DISPLAY_H
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
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <QHash>

// KDE includes
#include <kdebug.h>

// local includes

// Forward declaration
class Deck;
class Player;
class CardSprite;
class TextSprite;
class ThemeManager;



/**
 * Abstract display engine.
 */
class AbstractDisplay : public QObject
{
  Q_OBJECT

  public:
    /** Constructor for the engine
     *  @param grafixDir The grafic directory
     *  @param deck The card deck
     *  @param scene The graphics scene to work with
     *  @param advancePeriod The advance period [ms]
     *  @param parent The parent object
     */
    AbstractDisplay(Deck* deck, QGraphicsScene* scene, ThemeManager* theme, int advancePeriod, QGraphicsView* parent);

    /** Destructor.
    */
    virtual ~AbstractDisplay();

    /** Set a new deck object.
     *  @param deck The deck
     */
    void setDeck(Deck* deck);  

    /** Reset the display. Clear all sprites etc.
    */
    virtual void reset();

    /** Start the display.
     */
    virtual void start() = 0; 

    /** Play a card on the display. The card is moved from
     *  its current position to the card deposit.
     * @param playerNumber  The player number
     * @param height Top (0) or bottm (1) card
     * @param cardNumber The card number (0-7)
     * @param playerNumber The player number (0-1)
     * @param phase Movement phase (1st part, 2nd part, etc) [optional]
     */
    virtual void play(int cardNumber, int playerNumber, int phase) = 0;

    /** Turn a card on the display. The card is flipped backside
     *  to frontside.
     * @param playerNumber  The player number
     * @param height Top (0) or bottm (1) card
     * @param cardNumber The card number (0-7)
     */
    virtual void turn(int cardNumber) = 0;

    /** Remove the given card from the display.
     * @param winnerPosition  The position of the winner (0,1)
     * @param cardNumber The number of the card
     * @param delta Card offset from target position (0,1,2,...)
     */
    virtual void remove(int winnerPosition, int cardNumber, int delta) = 0;

    /** Retrieve the graphics scene of this display.
     *  @return The QGraphicsScene of this display.
     */
    QGraphicsScene* canvas() {return  mCanvas;} 

  
  public slots:
    virtual void convertMousePress(QPoint mouse, int& position, int& number) = 0;

  protected:
    /** Create the sprites for all possible cards and store them
      * here.
      */
    virtual void createCardSprites();


  protected:
    /** The card deck */
    Deck* mDeck;
    /** The work canvas */
    QGraphicsScene* mCanvas;
    // The graphics view
    QGraphicsView* mView;
    /** Canvas advance period */
    int mAdvancePeriod;
    /** Text sprite list */
    QList<QGraphicsItem*> mSprites;
    /** Store the card sprite indexed by the card value */
    static QHash<int,CardSprite*> mCards;
    /** Theme manager */
    ThemeManager* mTheme;
};


#endif
