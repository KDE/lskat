#ifndef ENGINE_TWO_H
#define ENGINE_TWO_H
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
#include <QString>

// KDE includes
#include <kdebug.h>

// Local includes
#include "abstractengine.h"
#include "deck.h"

// Forward declaration
class DisplayTwo;

using namespace CardDeck;


/**
 * The game engine for two players
 */
class EngineTwo : public AbstractEngine
{
  Q_OBJECT

  public:
    /** Move phase states */
    enum MoveState {FirstPlayerTurn=0, SecondPlayerTurn=1};


  
    /** Constructor for the game engine.
     *  @param parent The parent window
     *  @param deck   The card deck
     *  @param display The display engine
     */
    EngineTwo(QWidget* parent, Deck* deck, DisplayTwo* display);


    /** Start a new game.
     *  @param trump The trump suite.
     *  @param startPlayer The start player
     */
    virtual void startGame(Suite trump, int startPlayer);

    /** Stop a game.
     */
    virtual void stopGame();

    /** Retrieve the current move phase.
     *  @return The move phase.
     */
     MoveState currentMovePhase() {return mCurrentMovePhase;}

     /** Retrieve the currently played card 
      *  @param no Which card (0,1)
      *  @return The played card.
      */
     int playedCard(int no) {return mCurrentMoveCards[no];}


  public slots:

    virtual void playerInput(int inputId, int playerNumber, int cardNumber);

    /** Check whether the two cards played are legal, supposed the
     *  given player is the second one.
     * @param card1  The card played by the first player
     * @param card2  The card played by the second player
     * @param playerNumber The id of the latter player (to access its cards)
     * @return true if the move is legal, false otherwise.
     */
    bool isLegalMove(int card1, int card2, int playerNumber);

    /** Check who won a move, the first or the second card.
     *  @param card1 Card value played by first mover
     *  @param card2 Card value played by latter mover
     */
    int whoWonMove(int card1,int card2);

  signals:
    /** The game over signal.
     *  @param winner The game winner (0,1,-1:drawn, -2:abort)
     */
    void signalGameOver(int winner);
    

  protected slots:
    /** First part of the game loop
    */
    void gameLoopStart();

    /** Second part of the game loop
    */
    void gameLoopFinish();

  protected:
    /** Active the current player to the given player number.
      * Enable inputs etc.
      * @param number The new player playerNumber
      */
    void activatePlayer(int playerNumber);

    /** Check whether the game is over
     *  @return True if the game is over, false otherwise
     */
    bool gameOver();

    /** Called after game ends and then give points to players.
     * @return The winning player
     */
    int evaluateGame();

  private:
    // The display engine
    DisplayTwo* mDisplay;
    // The card deck
    Deck* mDeck;
    // Current move cards
    int mCurrentMoveCards[2];
    // Move phase (1st player or 2nd player)
    MoveState mCurrentMovePhase;
    // Current move number (counting half moves)
    int mMoveNumber;

    // Trump color
    Suite mTrump;
    
};

#endif
