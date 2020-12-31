/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LSKAT_ABSTRACTENGINE_H
#define LSKAT_ABSTRACTENGINE_H

// Qt includes
#include <QHash>
#include <QWidget>

// KF includes
#include "lskat_debug.h"

// Local includes
#include "deck.h"

// Forward declaration
class Player;

using namespace CardDeck;

/**
 * The document/data object for the game.
 */
class AbstractEngine : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor for the game engine.
     * @param parent The parent window
     */
    explicit AbstractEngine(QWidget *parent);

    /**
     * Stati of the game
     */
    enum GameStatus {Running, Stopped};

    /**
     * Start a new game.
     * @param trump The trump suite.
     * @param startPlayer The start player
     */
    virtual void startGame(Suite trump, int startPlayer) = 0;

    /**
     * Stop a game.
     */
    virtual void stopGame() = 0;

    /**
     * Is the game still running?
     * @return True if the game is running.
     */
    virtual bool isGameRunning() {return mGameStatus == Running;}

    /**
     * Add a player to the game
     * @param no Player number
     * @param player Player to add
     */
    virtual void addPlayer(int no, Player *player);

    /**
     * Retrieve the player of the given number.
     * @param no The player number.
     * @return The player.
     */
    virtual Player *player(int no);

    /**
     * Retrieve the current player id
     * @return The current player.
     */
    virtual int currentPlayer() {return mCurrentPlayer;}

public Q_SLOTS:
    /**
     * Player input available
     * @param inputId      The input device id
     * @param playerNumber The player number [0-1]
     * @param cardNumber   The card number [0-7]
     */
    virtual void playerInput(int inputId, int playerNumber, int cardNumber) = 0;

Q_SIGNALS:
    /**
     * The game over signal.
     * @param winner The game winner (0,1,-1:drawn, -2:abort)
     */
    void signalGameOver(int winner);

    /**
     * A players turn is about to start.
     * @param player The new player
     */
    void signalNextPlayer(Player *player);

protected:
    /** Current game status */
    GameStatus mGameStatus;
    /** Whose turn is it */
    int mCurrentPlayer;

    /** Player in the game */
    QHash<int, Player *> mPlayers;
};

#endif // LSKAT_ABSTRACTENGINE_H
