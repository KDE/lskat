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

// Local includes
#include "abstractdisplay.h"
#include "deck.h"
#include "cardsprite.h"
#include "textsprite.h"
#include "rectanglesprite.h"

// Define static attributes
QHash<int,CardSprite*> AbstractDisplay::mCards;

// Constructor for the engine
AbstractDisplay::AbstractDisplay(QString grafixDir, Deck* deck, Q3Canvas* canvas, QObject* parent)
    : QObject(parent)
{
  // Store our working canvas
  mGrafixDir = grafixDir;
  mCanvas = canvas;
  setDeck(deck);
  createSprites();

  mSprites.clear();
}


// Destructor.
AbstractDisplay::~AbstractDisplay()
{
  clearSprites();
  // Do not delete static cards for performance reasons
}

// Store the deck object for accesing the card deck
void AbstractDisplay::setDeck(Deck* deck)
{
  mDeck = deck;
}  


// Explicitely reset stored data, e.g. sprites.
void AbstractDisplay::reset()
{
  mCards.clear();
}


// Create a Canvas sprite for a given card number
CardSprite* AbstractDisplay::createSprite(int no)
{
  if (!mDeck)
  {
    kdFatal() << "createSprite::No deck set " << endl;
    return (CardSprite*)0;
  }

  QPixmap* front = mDeck->cardPixmap(no);
  QPixmap* back  = mDeck->backsidePixmap();
  CardSprite* sprite = CardSprite::create(mCanvas, front, back);
  return sprite;
}

// Create all sprites and store them for later access
void AbstractDisplay::createSprites()
{
  // Only create sprites once (unless explicitely reset)
  if (mCards.size() > 0) return;

  // Loop all cards
  for (int cardNo=0; cardNo<mDeck->cardNumber(); cardNo++)
  {
    // Create sprite with card correct card image
    CardSprite* sprite = createSprite(cardNo);
    sprite->setBackside();
    // Display sprite
    sprite->hide();

    // Store sprite
    mCards[cardNo] = sprite;
  }// next
}

// Reload all card sprite graphics
void AbstractDisplay::updateSpriteGraphics()
{
  if (!mDeck)
  {
    kdFatal() << "updateSpriteGraphics::No deck set " << endl;
    return;
  }
  QHashIterator<int,CardSprite*> it(mCards);
  while(it.hasNext())
  {
    it.next();
    CardSprite* sprite = it.value();
    int no = it.key();

    QPixmap* front = mDeck->cardPixmap(no);
    QPixmap* back  = mDeck->backsidePixmap();
    sprite->updateGraphics(front, back);
  }
}


// Set the advance period 
void AbstractDisplay::setAdvancePeriod(int advancePeriod)
{
  mAdvancePeriod = advancePeriod;
}


// Removes all text sprites from the display.
void AbstractDisplay::clearSprites()
{
  while(!mSprites.isEmpty())
  {
    Q3CanvasItem* item = mSprites.takeFirst();
    item->hide();
    delete item;
  }
}

