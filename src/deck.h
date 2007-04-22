#ifndef DECK_H
#define DECK_H
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
#include <QObject>
#include <QList>
#include <QPixmap>
#include <QSize>
#include <QHash>

// KDE includes
#include <kdebug.h>
#include <krandomsequence.h>

// Forward declaration

namespace CardDeck
{
    /** The suite and trump names 
    */
    enum Suite    {Club=0,Spade=1,Heart=2,Diamond=3,Grand=4};
    enum CardType {Ace=0,King=1,Queen=2,Jack=3,Ten=4,Nine=5,Eight=6,Seven=7};
}

using namespace CardDeck;

/**
 * The view object for the game.
 * This class servers as a container for the actual
 * canvas view and the control buttons.
 */
class Deck : public QObject
{
  Q_OBJECT

  public:

    /** Constructor for the main view.
     *  @param seed The random seed
     *  @param parent The parent window
     */
    Deck(long seed, QObject* parent);

    /** Destructor 
    */
    virtual ~Deck();

    /** Retrieve number of cards in this deck.
      * @return Number of cards.
      */
    int cardNumber();

    /** Shuffle the cards and reset the pile.
     */
    void shuffle();

    /** Draw a random trump from all cards. This is
     *  done by drawing a random card and choosing its
     *  suite as trump. In case a Jack is drawn a Grand
     * is made trump.
     */
    Suite randomTrump(); 

    /** Draw a random card out of the pile of existing ones.
     *  @return A random card
     */
     int drawCard();

    /** Get the suite of a given card (number)
     *  @param card The card number
     *  @return The suite.
     */
    static Suite getSuite(int card);

    /** Get the card type (ace, king, ...) of a given card (number)
     *  @param card The card number
     *  @return The card type (ace, king, ...).
     */
    static CardType getCardType(int card);

    /** Get the value in points of the given card (number).
     *  @param card The card number
     *  @return The card value (0, 2, 3, 4, 10, 11).
     */
    static int getCardValue(int card);

    /** Returns a verbose name for a card
     *  @param suite The card suite
     *  @param type  The card type
     *  @return A descriptive string.
     */
    static QString name(Suite suite, CardType type);

    /** Returns a verbose name for a card
     *  @param card The card as integer
     *  @return A descriptive string.
     */
    static QString name(int card);

  private:
    // Array of card numbers
    QList<int> mCards;
    // Random generator
    KRandomSequence mRandom;
};

#endif
