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

#include "deck.h"
#include "lskatglobal.h"

// Qt includes

// KDE includes
#include <klocale.h>
#include <kdebug.h>
#include <klocalizedstring.h>

// How many cards
#define NUMBER_OF_CARDS   32
// How often to shuffle the cards
#define SHUFFLE_AMOUNT  1000


// Constructor for the view
Deck::Deck(long seed, QObject* parent)
    : QObject(parent)
{
  // Set the random seed
   mRandom.setSeed(seed);
  // TODO: Hardcoded
  // mRandom.setSeed(global_debug);
  shuffle();
}

// Destructor
Deck::~Deck()
{
}

// Retrieve number of cards in this deck.
int Deck::cardNumber()
{
  // Use pixmaps as this is the same size but earlier filled.
  return mCards.size();
}

// Draw a random trump from all cards. 
Suite Deck::randomTrump()
{
  int card = mRandom.getLong(NUMBER_OF_CARDS);
   
  Suite suite = getSuite(card);
  CardType type = getCardType(card);
  if (type == Jack) return Grand;

  // TODO: Hardcoded test
  // return Diamond;
  return suite;
}


// Shuffle deck of cards
void Deck::shuffle()
{
   mCards.clear();
   // Fill card deck with ordered cards
   for (int i=0; i<NUMBER_OF_CARDS; i++)
   {
     mCards.append(i);
   }

   // Shuffle cards
   for (int i=0;i<SHUFFLE_AMOUNT;i++)
   {
     int c1 = mRandom.getLong(NUMBER_OF_CARDS);
     int c2 = mRandom.getLong(NUMBER_OF_CARDS);
     mCards.swap(c1, c2);
   }


   // TODO: Hardcoded deck
   return;
   static int c[] = {
   Ten, Spade, Seven, Diamond, Ten, Diamond, Jack, Club, 
   Eight, Club, Nine, Club, Nine, Heart, Jack, Diamond,
   Eight, Spade, Seven, Heart, Nine, Spade, Ten, Club,
   Ace, Diamond, Eight, Heart, Queen, Spade, Ten, Heart,

   Ace, Club, Ace, Heart, Queen, Club, Jack, Heart,
   King, Heart, Ace, Spade, Queen, Diamond, Jack, Spade,
   King, Club, Nine, Diamond, Seven, Spade, Queen, Heart,
   Eight, Diamond, Seven, Club, King, Spade, King, Diamond
   };

   mCards.clear();
   // Fill card deck with ordered cards
   for (int i=0; i<NUMBER_OF_CARDS; i++)
   {
     int card = c[2*i];
     int suite = c[2*i+1];
     int cc = getCard(Suite(suite),CardType(card));

     mCards.append(cc);
   }
}


// Draw a card from the deck
int Deck::drawCard()
{
  if (mCards.size() < 1)
  {
    kFatal() << "No more cards to draw from card deck";
  }
  int card = mCards.takeFirst();
  return card;
}

int Deck::getCard(Suite suite, CardType type)
{
  return 4*int(type)+int(suite);
}


// Return the suite of a given card (number)
Suite Deck::getSuite(int card)
{
  return (Suite)(card%4);
}


// Return the card type (ace, king, ...) of a given card (number)
CardType Deck::getCardType(int card)
{
  return (CardType)(card/4);
}


// Get the value in points of the given card (number).
int Deck::getCardValue(int card)
{
  CardType type = getCardType(card);
  if (type == Ace) return  11;
  if (type == Ten) return  10;
  if (type == King) return  4;
  if (type == Queen) return 3;
  if (type == Jack) return  2;
  return 0;
}


// Get verbose name of a vard
QString Deck::name(int card)
{
  if (card < 0) return i18n("no valid card");
  return name(getSuite(card), getCardType(card));
}

// Return a verbose name for a suite
QString Deck::name(Suite suite)
{
  QString suiteName = i18n("unknown");
  if (suite == Club) suiteName = i18nc("suite name", "Clubs");
  if (suite == Spade) suiteName = i18nc("suite name", "Spades");
  if (suite == Heart) suiteName = i18nc("suite name", "Hearts");
  if (suite == Diamond) suiteName = i18nc("suite name", "Diamonds");
  if (suite == Grand) suiteName = i18nc("trump name", "Grand");
  return suiteName;
}

// Return a verbose name for a cardtype
QString Deck::name(CardType type)
{
  QString typeName = i18n("unknown");
  if (type == Ace) typeName = i18nc("card name", "Ace");
  if (type == King) typeName = i18nc("card name", "King");
  if (type == Queen) typeName = i18nc("card name", "Queen");
  if (type == Jack) typeName = i18nc("card name", "Jack");
  if (type == Ten) typeName = i18nc("card name", "Ten");
  if (type == Nine) typeName = i18nc("card name", "Nine");
  if (type == Eight) typeName = i18nc("card name", "Eight");
  if (type == Seven) typeName = i18nc("card name", "Seven");
  return typeName;
}


// Returns a verbose name for a card
QString Deck::name(Suite suite, CardType type)
{
    QString suiteName = name(suite);
    QString typeName  = name(type);
    return i18nc("eg jack of clubs", "%1 of %2", typeName, suiteName);
}


#include "deck.moc"
