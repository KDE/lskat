/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "abstractdisplay.h"

// Qt includes

// KF includes

// Local includes
#include "deck.h"
#include "cardsprite.h"
#include "thememanager.h"

// Define static attributes
QHash<int, CardSprite *> AbstractDisplay::mCards;

// Constructor for the engine
AbstractDisplay::AbstractDisplay(Deck *deck, QGraphicsScene *scene, ThemeManager *theme,
                                 int advancePeriod, QGraphicsView *parent)
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
void AbstractDisplay::setDeck(Deck *deck)
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
    if (!mCards.isEmpty()) return;

    // Loop all cards
    for (int cardNo = 0; cardNo < mDeck->cardNumber(); cardNo++)
    {
        // Create sprite with card correct card image
        Suite suite        = Suite(cardNo % 4);
        CardType cardtype  = CardType(cardNo / 4);
        CardSprite *sprite = new CardSprite(suite, cardtype, mTheme, mAdvancePeriod, mScene);
        // Display sprite
        sprite->hide();

        // Store sprite
        mCards[cardNo] = sprite;
    }// next
}
