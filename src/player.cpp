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

#include "player.h"

// Qt includes

// KDE includes
#include <kconfiggroup.h>

// Local includes
#include "abstractinput.h"
#include "deck.h"
#include "lskat_debug.h"

// Constructor for the player
Player::Player(int id, QObject *parent)
    : QObject(parent)
{
    mId     = id;
    mCards.clear();
    mPoints = 0;
    mDeck   = 0;
    mInput  = 0;
    mTrump  = Club;

    // Reset internal variables - they are set by 'load'
    setName(QLatin1String(""));
    mScore         = 0;
    mNumberOfGames = 0;
    mGamesWon      = 0;
}

// Destructor
Player::~Player()
{
    delete mInput;
}

// Save properties
void Player::save(KConfigGroup &config)
{
    config.writeEntry("name", mName);
    config.writeEntry("gameswon", mGamesWon);
    config.writeEntry("score", mScore);
    config.writeEntry("noofgames", mNumberOfGames);
}

// Load properties
void Player::load(KConfigGroup &config)
{
    mName          = config.readEntry("name", mName);
    mGamesWon      = config.readEntry("gameswon", mGamesWon);
    mScore         = config.readEntry("score", mScore);
    mNumberOfGames = config.readEntry("noofgames", mNumberOfGames);

    // Emit signals
    refresh();
}

// Set the deck for drawing cards.
void Player::setDeck(Deck *deck)
{
    mDeck = deck;
}

// Clear the all time statistics of this player.
void Player::clear()
{
    mNumberOfGames = 0;
    mScore = 0;
    mGamesWon = 0;
    refresh();
}

// Deal a number of cards to this player
void Player::deal(int amount)
{
    if (!mDeck)
    {
        qCCritical(LSKAT_LOG) << "No deck set to player.";
        return;
    }
    mCards.clear();
    mCards.resize(amount);
    for (int i = 0; i < amount; i++)
    {
        int card = mDeck->drawCard();
        mCards[i] = card;
    }

    // Reset moves and points
    mMovesWon = 0;
    setPoints(0);
    mWonCards.clear();

    refresh();
}

// Retrieve the input device of the player
AbstractInput *Player::input()
{
    return mInput;
}

// Set the input device of the player
void Player::setInput(AbstractInput *input)
{
    // Try to set the same turn status after changing input
    bool oldTurnAllowed = false;
    // Get rid of old input device if existing
    if (mInput)
    {
        oldTurnAllowed = mInput->inputAllowed();
        mInput->setInputAllowed(false);
        delete mInput;
    }
    // Store new input
    mInput = input;
    // Store player
    mInput->setId(mId);
    // Restore turn status
    mInput->setInputAllowed(oldTurnAllowed);

    refresh();
}

// Set this player to start a turn
void Player::startTurn()
{
    mInput->setInputAllowed(true);
}

// Set this player to stop a turn
void Player::stopTurn()
{
    mInput->setInputAllowed(false);
}

// Remove a card from the given position. Typically if the card was played.
void Player::deleteCard(int cardPosition)
{
    if (cardPosition >= mCards.size())
    {
        qCCritical(LSKAT_LOG) << "Player " << mId << " tries to delete non existing card position "
                << cardPosition << " >=" << mCards.size();
    }
    mCards[cardPosition] = -1;
}

// Add a card to the player
void Player::addCard(int cardPosition, int card)
{
    if (cardPosition >= mCards.size())
    {
        qCCritical(LSKAT_LOG) << "Player " << mId << " tries to add to existing card position "
                << cardPosition << " >=" << mCards.size();
    }
    mCards[cardPosition] = card;
}

// Retrieve card value at given logical position
int Player::getCard(int playerNumber)
{
    if (playerNumber >= mCards.size())
    {
        qCCritical(LSKAT_LOG) << "Player " << mId << " tries to get non existing card "
                << playerNumber << " >=" << mCards.size();
    }

    int card = mCards[playerNumber];
    return card;
}

// Increases the number of moves one for this player
void Player::increaseMovesWon(int amount)
{
    mMovesWon += amount;
}

// Retrieve the number of won moves for this player
int Player::noOfMovesWon()
{
    return mMovesWon;
}

// Adds a card which is one in a move to this player.
void Player::addWonCard(int card)
{
    // Store card
    mWonCards.append(card);

    // Add points
    int value = mDeck->getCardValue(card);
    setPoints(points() + value);
}

// Retrieve a card won by this player.
int Player::getWonCard(int no)
{
    if (no >= mWonCards.size())
    {
        qCCritical(LSKAT_LOG) << "Player::getWonCard This card " << no << " is not available. "
                << "Only " << mWonCards.size() << " cards stored.";
        return -1;
    }
    return mWonCards[no];
}

// Retrieve the amount of points this player has
int Player::points()
{
    return mPoints;
}

// Set the points of the player.
void Player::setPoints(int points)
{
    mPoints = points;
    refresh();
}

// Retrieve the player's name
QString Player::name()
{
    return mName;
}

// Set the name of the player.
void Player::setName(const QString &name)
{
    mName = name;
    refresh();
}

// Add a number of won games to the overall statistic
void Player::addWonGame(int amount)
{
    mGamesWon += amount;
    refresh();
}

// Get amount of won games
int Player::wonGames()
{
    return mGamesWon;
}

// Add a number of games to the overall statistic
void Player::addGame(int amount)
{
    mNumberOfGames += amount;
    refresh();
}

// Get number of games
int Player::games()
{
    return mNumberOfGames;
}

// Add a score to the overall statistic
void Player::addScore(int amount)
{
    mScore += amount;
    refresh();
}

// Get score
int Player::score()
{
    return mScore;
}

// Set trump
void Player::setTrump(Suite trump)
{
    mTrump = trump;
    refresh();
}

// Get trump
Suite Player::trump()
{
    return mTrump;
}

// Emit all signals for GUI
void Player::refresh()
{
    emit signalUpdate(this);
}
