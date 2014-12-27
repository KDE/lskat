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

#include "abstractdisplay.h"

// Qt includes

// KDE includes
#include <KLocalizedString>
#include "lskat_debug.h"

// Local includes
#include "deck.h"
#include "cardsprite.h"
#include "thememanager.h"

// Define static attributes
QHash<int,CardSprite*> AbstractDisplay::mCards;

// Constructor for the engine
AbstractDisplay::AbstractDisplay(Deck* deck, QGraphicsScene* scene, ThemeManager* theme,
                                 int advancePeriod, QGraphicsView* parent)
               : QObject(parent)
{
  // Store the scene
  mScene = scene;
  // Store the view
  mView = parent;
  // Store advance period
  mAdvancePeriod = advancePeriod;
  // Store theme manager
  mTheme = theme;

  // Set up deck
  setDeck(deck);

  // Initialize sprites
  createCardSprites();
  mSprites.clear();
}


// Destructor.
AbstractDisplay::~AbstractDisplay()
{
  qDeleteAll(mSprites);
  // Do not delete static cards for performance reasons
}


// Store the deck object for accessing the card deck
void AbstractDisplay::setDeck(Deck* deck)
{
  mDeck = deck;
}  


// Explicitly reset stored data, e.g. sprites.
void AbstractDisplay::reset()
{
  mCards.clear();
}


// Create all sprites and store them for later access
void AbstractDisplay::createCardSprites()
{
  // Only create sprites once (unless explicitly reset)
  if (mCards.size() > 0) return;

  // Loop all cards
  for (int cardNo=0; cardNo<mDeck->cardNumber(); cardNo++)
  {
    // Create sprite with card correct card image
    Suite suite        = Suite(cardNo%4);
    CardType cardtype  = CardType(cardNo/4);
    CardSprite* sprite = new CardSprite(suite, cardtype, mTheme, mAdvancePeriod, mScene);
    // Display sprite
    sprite->hide();

    // Store sprite
    mCards[cardNo] = sprite;
  }// next
}


