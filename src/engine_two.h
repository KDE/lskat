/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ENGINE_TWO_H
#define ENGINE_TWO_H

// Qt includes

// KF includes

// Local includes
#include "abstractengine.h"
#include "deck.h"
#include "lskat_debug.h"

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
    /**
     * Move phase states
     */
    enum MoveState {FirstPlayerTurn = 0, SecondPlayerTurn = 1};

    /**
     * Constructor for the game engine.
     * @param parent The parent window
     * @param deck   The card deck
     * @param display The display engine
     */
    EngineTwo(QWidget *parent, Deck *deck, DisplayTwo *display);

    /**
     * Start a new game.
     * @param trump The trump suite.
     * @param startPlayer The start player
     */
    void startGame(Suite trump, int startPlayer) override;

    /**
     * Stop a game.
     */
    void stopGame() override;

    /**
     * Retrieve the current move phase.
     * @return The move phase.
     */
    MoveState currentMovePhase() const {return mCurrentMovePhase;}

    /**
     * Retrieve the currently played card
     * @param no Which card (0,1)
     * @return The played card.
     */
    int playedCard(int no) const {return mCurrentMoveCards[no];}

    /**
     * Retrieve the current trump color.
     * @return The trump suite.
     */
    Suite trump() const {return mTrump;}

public Q_SLOTS:
    /**
     * Player input available
     * @param inputId      The input device id
     * @param playerNumber The player number [0-1]
     * @param cardNumber   The card number [0-7]
     */
    void playerInput(int inputId, int playerNumber, int cardNumber) override;

    /**
     * Check whether the two cards played are legal, supposed the
     * given player is the second one.
     * @param card1  The card played by the first player
     * @param card2  The card played by the second player
     * @param playerNumber The id of the latter player (to access its cards)
     * @return true if the move is legal, false otherwise.
     */
    bool isLegalMove(int card1, int card2, int playerNumber);

    /**
     * Check who won a move, the first or the second card.
     * @param card1 Card value played by first mover
     * @param card2 Card value played by latter mover
     * @param trump Current trump suite
     * @return The winning player [0,1]
     */
    static int whoWonMove(int card1, int card2, Suite trump);

protected Q_SLOTS:
    /**
     * First part of the game loop.
     */
    void gameLoopStart();

    /**
     * Second part of the game loop.
     */
    void gameLoopFinish();

protected:
    /**
     * Active the current player to the given player number.
     * Enable inputs etc.
     * @param playerNumber The new player playerNumber
     */
    void activatePlayer(int playerNumber);

    /**
     * Check whether the game is over
     * @return True if the game is over, false otherwise
     */
    bool gameOver();

    /**
     * Called after game ends and then give points to players.
     * @return The winning player
     */
    int evaluateGame();

private:
    // The display engine
    DisplayTwo *mDisplay;
    // The card deck
    Deck *mDeck;
    // Current move cards
    int mCurrentMoveCards[2];
    // Current cards to turn at end of move (or -1)
    int mCurrentTurnCards[2];
    // Move phase (1st player or 2nd player)
    MoveState mCurrentMovePhase;
    // Current move number (counting half moves)
    int mMoveNumber;
    // Trump color
    Suite mTrump;
};

#endif
