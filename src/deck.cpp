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

// KDE includes
#include <klocale.h>
#include <kdebug.h>
#include <klocalizedstring.h>

// Local includes
#include "deck.h"

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

  // Clear stuff
  mCardBackside = 0;
  mCardPixmaps.clear();
  mTrumpIcons.clear();
}

// Destructor
Deck::~Deck()
{
  if (mCardBackside) delete mCardBackside;

  for (int i=0;i<mCardPixmaps.size();i++)
  {
    if (mCardPixmaps[i]) delete mCardPixmaps[i];
  }
  mCardPixmaps.clear();

  QHashIterator<Suite,QPixmap*> it(mTrumpIcons);
  while(it.hasNext())
  {
    it.next();
    QPixmap* pm = it.value();
    if (pm) delete pm;
  }
  mTrumpIcons.clear();
}



// Retrieve the backside pixmap
QPixmap* Deck::backsidePixmap()
{
  return mCardBackside;
}

QPixmap* Deck::cardPixmap(int no)
{
  return mCardPixmaps[no];
}


// Load the card backside from a given file
bool Deck::loadCardBackside(QString filename)
{
  kdDebug() << "Loading backside image " << filename << endl;
  QPixmap* pm = new QPixmap();
  if (!pm->load(filename))
  {
    kdError() << "Cannot load file " << filename << endl;
    return false;
  }
  if (mCardBackside) delete mCardBackside;
  mCardBackside = pm;
  return true;
}

// Load the trump icons from a directory
bool Deck::loadTrump(QString dir)
{
  kdDebug() << "Loading trump images " << dir << endl;
  mTrumpIcons.clear();

  for (int i=0;i<5;i++)
  {
    QPixmap* pm = new QPixmap();
    QString file = dir+"/"+QString("t%1.png").arg(i);
    if (!pm->load(file))
    {
      kdError() << "Cannot load file " << file << endl;
      return false;
    }
    if (mTrumpIcons.contains((Suite)i)) delete mTrumpIcons[(Suite)i];
    mTrumpIcons[(Suite)i] = pm;
  }// next i
  
  return true;
}


// Load a set of cards from the given path
bool Deck::loadCards(QString cardPath)
{
  // Max size of images in cased they are not even
  int maxHeight = 0;
  int maxWidth  = 0;

  // Clear out old stuff
  for (int i=0;i<mCardPixmaps.size();i++)
  {
    if (mCardPixmaps[i]) delete mCardPixmaps[i];
  }
  mCardPixmaps.clear();

  for (int i=0;i<NUMBER_OF_CARDS;i++)
  {
    QString filename = cardPath+"/"+QString("%1.png").arg(i+1);
    //kdDebug() << "Loading image " << filename << endl;
    QPixmap* pm = new QPixmap();
    if (!pm->load(filename))
    {
      mCardPixmaps.clear();
      kdError() << "Cannot load file " << filename << endl;
      maxCardSize = QSize(0,0);
      return false;
    }
    mCardPixmaps.append(pm);
    if (pm->width() > maxWidth) maxWidth = pm->width();
    if (pm->height() > maxHeight) maxHeight = pm->height();
  }

  maxCardSize = QSize(maxWidth, maxHeight);
  return true;
}

// Retrieve the size of a card pixmap.
QSize Deck::cardSize()
{
  return maxCardSize;
}


// Retrieve number of cards in this deck.
int Deck::cardNumber()
{
  // Use pixmaps as this is the same size but earlier filled.
  return mCardPixmaps.size();
}

// Draw a random trump from all cards. 
Suite Deck::randomTrump()
{
  int card = mRandom.getLong(NUMBER_OF_CARDS);
   
  Suite suite = getSuite(card);
  CardType type = getCardType(card);
  if (type == Jack) return Grand;
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
}

// Draw a card from the deck
int Deck::drawCard()
{
  if (mCards.size() < 1)
  {
    kdFatal() << "No more cards to draw from card deck " << endl;
  }
  int card = mCards.takeFirst();
  return card;
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

// Retrive the pixmap icon belonging to trump
QPixmap* Deck::trumpIcon(Suite suite)
{
  return mTrumpIcons[suite];
}

QString Deck::name(int card)
{
  if (card < 0) return i18n("no valid card");
  return name(getSuite(card), getCardType(card));
}

// Returns a verbose name for a card
QString Deck::name(Suite suite, CardType type)
{
    QString suiteName = i18n("unknown");
    if (suite == Club) suiteName = i18nc("suite name", "Clubs");
    if (suite == Spade) suiteName = i18nc("suite name", "Spades");
    if (suite == Heart) suiteName = i18nc("suite name", "Hearts");
    if (suite == Diamond) suiteName = i18nc("suite name", "Diamonds");
    if (suite == Grand) suiteName = i18nc("trump name", "Grand");
    QString typeName = i18n("unknown");
    if (type == Ace) typeName = i18nc("card name", "Ace");
    if (type == King) typeName = i18nc("card name", "King");
    if (type == Queen) typeName = i18nc("card name", "Queen");
    if (type == Jack) typeName = i18nc("card name", "Jack");
    if (type == Ten) typeName = i18nc("card name", "Ten");
    if (type == Nine) typeName = i18nc("card name", "Nine");
    if (type == Eight) typeName = i18nc("card name", "Eight");
    if (type == Seven) typeName = i18nc("card name", "Seven");

    return i18nc("eg jack of clubs", "%1 of %2", typeName, suiteName);
}

