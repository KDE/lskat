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
#include <QPixmap>
#include <QTimer>

// KDE includes
#include <kdebug.h>

// Local includes
#include "aiinput.h"
#include "player.h"
#include "deck.h"


// Constructor for the engine
AiInput::AiInput(EngineTwo* engine, QString dir, QObject* parent)
    : AbstractInput(parent)
{
  QString filename = dir+ QString("aiinput.png");
  mInputIcon = new QPixmap();
  if (!mInputIcon->load(filename))
  {
    kError() << "Cannot load file " << filename << endl;
  }

  // Store engine
  mEngine = engine;
}

// Allow or disallow input with this device 
void AiInput::setInputAllowed(bool allowed)
{
  AbstractInput::setInputAllowed(allowed);
  if (allowed) QTimer::singleShot(1000, this, SLOT(aiTurn()));
}


// Calculate and send out AI turn
void AiInput::aiTurn()
{
  // Turn was stopped meanwhile
  if (!mInputAllowed) return;

  kDebug() << "==================================================="<<endl;
  kDebug() << "AI TURN START " <<mInputAllowed<< endl;

  // Check we are the right player
  if (mId != mEngine->currentPlayer())
  {
    kFatal() << "AI plays for wrong player " << endl;
    return;
  }

  // Retrieve game board
  AiInput::Board board = getBoardFromEngine();
  AiInput::Move move;

  // Initiate move
  if (mEngine->currentMovePhase() == EngineTwo::FirstPlayerTurn)
  {
    kDebug() << "Performing initiual move "<<mId << endl;
    move = initiateMove(mId, board);
  }
  // Respond to move
  else
  {
    kDebug() << "Performing answer move "<<mId << endl;
    move = answerMove(mId, board);
  }


  // Send out move
  kDebug() << "AI player " << mId << " moves to " << move.move << endl;
  if (move.move>=0) emit signalPlayerInput(mId, mId, move.move);
  else kError() << "Illegal AI Move ??? " << endl;
}


// Extract the current game board from the engine
AiInput::Board AiInput::getBoardFromEngine()
{
  Board b;
  for (int i=0; i<2; i++)
  {
    Player* p = mEngine->player(i);
    for (int c=0; c<16; c++)
    {
      int card = p->getCard(c);
      b.cards[i][c] = card;
      b.points[i]  = p->points();
    }
  }
  b.whoseTurn = mEngine->currentPlayer();
  b.firstPlay = mEngine->currentMovePhase() == EngineTwo::FirstPlayerTurn;
  b.playedCard = mEngine->playedCard(0);
  return b;
}

// Game evaluation ratings
#define RATING_SCHWARZ      100000.0
#define RATING_SCHNEIDER     70000.0
#define RATING_WON           50000.0
#define RATING_REMIS         20000.0
#define RATING_ONE_POINT       500.0

// Evaluate the current game board and return a rating
double AiInput::evaluteGame(int p, const AiInput::Board current)
{
  double rating = 0.0;
  // Check for won games of our side
  if (current.points[p] == 120) rating += RATING_SCHWARZ;
  else if (current.points[p] >= 90) rating += RATING_SCHNEIDER;
  else if (current.points[p] > 60) rating += RATING_WON;
  else if (current.points[p] == 60) rating += RATING_REMIS;
  // Check for won games of other side
  if (current.points[1-p] == 120) rating -= RATING_SCHWARZ;
  else if (current.points[1-p] >= 90) rating -= RATING_SCHNEIDER;
  else if (current.points[1-p] > 60) rating -= RATING_WON;
  else if (current.points[1-p] == 60) rating -= RATING_REMIS;

  // Evaluate points
  rating += (current.points[p]-current.points[1-p])*RATING_ONE_POINT;
  return rating;
}   


// Initiate a new move as first player
AiInput::Move AiInput::initiateMove(int p, const AiInput::Board& board)
{
  AiInput::Move maxMove;
  maxMove.move  = -1;
  maxMove.value = -100.0*RATING_SCHWARZ; // Absolut minimum score
  
  // Loop all moves
  for (int m=0; m<8; m++)
  {
    AiInput::Board current(board);
    int card = current.cards[p][m];  // 1st card
    current.cards[p][m] = -1; // Can do in any case
    if (card < 0)
    {
      card = current.cards[p][m+8]; // 2nd card
      current.cards[p][m+8] = -1; // Can do in any case
    }
    if (card < 0) continue; // Illegal move
    // Store move
    current.playedCard = card;
    kDebug() << "First mover try move on " << m << " ("<<Deck::name(card) <<endl;
    AiInput::Move answer = answerMove(1-p, current);
    // Negate answering moves value to get our rating
    double rating = -answer.value;
    kDebug() << "First mover yields rating of " << rating << endl;
    // New best move?
    if (rating > maxMove.value)
    {
      maxMove.value = rating;
      maxMove.move  = m;
    }
  }
  
  return maxMove;
}

// Answer a move as second player
AiInput::Move AiInput::answerMove(int p, const AiInput::Board& board)
{
  AiInput::Move maxMove;
  maxMove.move  = -1;
  maxMove.value = -100.0*RATING_SCHWARZ; // Absolut minimum score
  
  // Loop all moves
  for (int m=0; m<8; m++)
  {
    AiInput::Board current(board);
   // kDebug() << "CARD "<<m<< " is " 
    //          << Deck::name(current.cards[p][m]) << " on top of " 
    //          << Deck::name(current.cards[p][m+8]) << endl;

    int card = current.cards[p][m];  // 1st card
    current.cards[p][m] = -1; // Can do in any case
    if (card < 0)
    {
      card = current.cards[p][m+8]; // 2nd card
      current.cards[p][m+8] = -1; // Can do in any case
    }
    if (card < 0) continue; // Illegal move

    // Check validity of move
    if (!mEngine->isLegalMove(current.playedCard, card, p)) continue;

    // Check move winner
    int winner = mEngine->whoWonMove(current.playedCard, card);
    kDebug() << "   Card " << m<< " (" << Deck::name(card) << ") is valid " 
              << " countering " << Deck::name(current.playedCard)<<" with "
              << " winner (0:other, 1:we) " << winner << endl;
    int deltaPoints = 0;
    deltaPoints += Deck::getCardValue(current.playedCard);
    deltaPoints += Deck::getCardValue(card);
    // The first mover won
    if (winner == 0)
    {
      current.points[1-p] += deltaPoints;
    }
    // The second mover won (us)
    else
    {
      current.points[p] += deltaPoints;
    }

    double rating = evaluteGame(p, current);


    kDebug() << "   Points after 2nd move "<<m<<" would be we: " 
              << current.points[p] << " other: " << current.points[1-p] 
              << " rating is thus " << rating << endl;
    // New best move?
    if (rating > maxMove.value)
    {
      maxMove.value = rating;
      maxMove.move  = m;
    }
  }
  return maxMove;
}


// Board copy constructor
AiInput::Board::Board(const AiInput::Board& board)
{
   for (int i=0;i<2;i++)
   {
     points[i] = board.points[i];
     for (int j=0;j<16;j++)
     {
       cards[i][j] = board.cards[i][j];
     }
   }
   playedCard = board.playedCard;
   whoseTurn = board.whoseTurn;
   firstPlay = board.firstPlay;     
}
