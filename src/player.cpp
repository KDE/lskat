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
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "abstractinput.h"
#include "deck.h"

// Constructor for the player
Player::Player(int id, QObject* parent)
    : QObject(parent)
{
  mId     = id;
  mCards.clear();
  mPoints = 0;
  mDeck   = 0;
  mInput  = 0;
  mTrump  = Club;
  
  // Reset internal variables - they are set by 'load' 
  setName("");
  mScore         = 0;
  mNumberOfGames = 0;
  mGamesWon      = 0;
}


// Destructor 
Player::~Player()
{
  if (mInput) delete mInput;
}


// Save properties
void Player::save(KConfigGroup& config)
{
  config.writeEntry("name", mName);
  config.writeEntry("gameswon", mGamesWon);
  config.writeEntry("score", mScore);
  config.writeEntry("noofgames", mNumberOfGames);
}


// Load properties
void Player::load(KConfigGroup& config)
{
  mName          = config.readEntry("name", mName);
  mGamesWon      = config.readEntry("gameswon", mGamesWon);
  mScore         = config.readEntry("score", mScore);
  mNumberOfGames = config.readEntry("noofgames", mNumberOfGames);

  // Emit signals
  refresh();

}


// Set the deck for drawing cards.
void Player::setDeck(Deck* deck)
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
    kFatal() << "No deck set to player." << endl;
    return;
  }
  mCards.clear();
  mCards.resize(amount);
  for (int i=0; i<amount; i++)
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
AbstractInput* Player::input()
{
  return mInput;
}


// Set the input device of the player 
void Player::setInput(AbstractInput* input)
{
  // Try to set the same turn status after changing input
  bool oldTurnAllowed = false;
  // Get rid of old input devive if existing
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
   

// Remove a card from the given position. Typically if the
void Player::deleteCard(int cardPosition)
{
  if (cardPosition >= mCards.size())
  {
    kFatal() << "Player " << mId << " tries to delete non esxisting card position " 
              << cardPosition <<" >= " << mCards.size() << endl;
  }
  mCards[cardPosition] = -1;
}


// Add a card to the player
void Player::addCard(int cardPosition, int card)
{
  if (cardPosition >= mCards.size())
  {
    kFatal() << "Player " << mId << " tries to add to esxisting card position " 
              << cardPosition <<" >= " << mCards.size() << endl;
  }
  mCards[cardPosition] = card;
}


// Retrive card value at given logical position
int Player::getCard(int playerNumber)
{
  if (playerNumber >= mCards.size())
  {
    kFatal() << "Player " << mId << " tries to get non esxisting card " 
              << playerNumber <<" >= " << mCards.size() << endl;
  }
  
  int card = mCards[playerNumber];
  return card;
}


// Increases the numebr of moves one for this player
void Player::increaseMovesWon(int amount)
{
  mMovesWon += amount;
}


// Retrive the number of won moves for this player
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
  if (no>=mWonCards.size())
  {
    kFatal() << "Player::getWonCard This card " << no << " is not available. "
              << "Only " << mWonCards.size() << " cards stored." << endl;
    return -1;
  }
  return mWonCards[no];
}


// Retrive the amount of points this player has
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


// Retrive the player's name
QString Player::name()
{
  return mName;
}


// Set the name of the player.
void Player::setName(QString name)
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

#include "player.moc"
