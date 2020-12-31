/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "engine_two.h"

// Qt includes
#include <QTimer>

// KDE includes
#include <KLocalizedString>

// Local includes
#include "lskatglobal.h"
#include "display_two.h"

#define TIME_END_MOVE    1000  /* Wait this [ms] after end of move */

// Constructor for the game document/engine
EngineTwo::EngineTwo(QWidget *parent, Deck *deck, DisplayTwo *display)
               : AbstractEngine(parent)
{
    mDisplay       = display;
    mDeck          = deck;
    mCurrentPlayer = 0;

    connect(mDisplay, &DisplayTwo::dealingDone, this, &EngineTwo::gameLoopStart);
}

// Initial part of the game loop. Prepare new move etc
void EngineTwo::gameLoopStart()
{
    if (global_debug > 1) qCDebug(LSKAT_LOG) << "GAME LOOP START";
    if (!isGameRunning()) return;

    // Switch to the current player
    activatePlayer(mCurrentPlayer);
}

// Middle part of the game loop. Receive input of a player
// Here: inputId == playerNumber
void EngineTwo::playerInput(int inputId, int playerNumber, int cardNumber)
{
    if (global_debug > 0)
        qCDebug(LSKAT_LOG) << "Engine got player input: card=" << cardNumber
                << "Player=" << playerNumber << "Id=" << inputId;
    if (playerNumber != mCurrentPlayer)
    {
        if (global_debug > 0)
            qCDebug(LSKAT_LOG) << "EngineTwo::playerInput: Input from wrong player";
        return;
    }

    Player *player = mPlayers[playerNumber];
    int height = 0;
    int card = player->getCard(cardNumber);
    // Check whether top card is still available - if not try bottom one
    if (card < 0)
    {
        height = 1;
        card = player->getCard(cardNumber + 8 * height);
    }

    // Check whether player still has this card
    if (card < 0)
    {
        if (global_debug > 0)
            qCDebug(LSKAT_LOG) << "EngineTwo::playerInput: Card" << cardNumber + 8 * height
                    << "not available anymore ";
        return;
    }

    // Remove this, Debug current card
    if (global_debug > 0)
    {
        Suite   suite = Deck::getSuite(card);
        CardType type = Deck::getCardType(card);
        qCDebug(LSKAT_LOG) << "Gameloop " <<mCurrentPlayer <<" plays" << Deck::name(suite, type);
    }

    // Check for legal move (first player always ok)
    if (mCurrentMovePhase == SecondPlayerTurn)
    {
        //   Check (card of player 1), (card of player 2), (player 2)
        if (!isLegalMove(mCurrentMoveCards[FirstPlayerTurn], card, playerNumber))
        {
            if (global_debug > 0)
                qCDebug(LSKAT_LOG) << "EngineTwo::playerInput: Card" << cardNumber + 8 * height
                        << "is not a valid move ";
            return;
        }
    }

    // Delete card from player
    player->deleteCard(cardNumber + 8 * height);

    // Finish input from player
    player->stopTurn();

    // Play out card
    mDisplay->play(card, playerNumber, mCurrentMovePhase);

    // Turn back card if top card is played
    if (height == 0)
    {
        int backcard = player->getCard(cardNumber + 8);
        mCurrentTurnCards[mCurrentMovePhase] = backcard;
    }
    else
    {
        mCurrentTurnCards[mCurrentMovePhase] = -1;
    }

    // Store currently played card
    mCurrentMoveCards[mCurrentMovePhase] = card;

    // Finish game loop
    if (mCurrentMovePhase == SecondPlayerTurn)
    {
        QTimer::singleShot(TIME_END_MOVE, this, &EngineTwo::gameLoopFinish);
    }
    else
    {
        QTimer::singleShot(0, this, &EngineTwo::gameLoopFinish);
    }
}

// Final part of the game loop. Switch player etc.
void EngineTwo::gameLoopFinish()
{
    if (!isGameRunning()) return;

    // If second move phase, remove cards
    if (mCurrentMovePhase == SecondPlayerTurn)
    {
        // Turn cards if available
        if (mCurrentTurnCards[FirstPlayerTurn] >= 0)
            mDisplay->turn(mCurrentTurnCards[FirstPlayerTurn]);
        if (mCurrentTurnCards[SecondPlayerTurn] >= 0)
            mDisplay->turn(mCurrentTurnCards[SecondPlayerTurn]);

        // Switch the current player if second player one
        int winner = whoWonMove(mCurrentMoveCards[FirstPlayerTurn],
                                mCurrentMoveCards[SecondPlayerTurn],
                                mTrump);
        // The first mover won. This means to switch the current player back
        // to him.  Otherwise the current player stays untouched, that is the
        // second player plays again.
        if (winner == 0)
        {
            mCurrentPlayer = 1 - mCurrentPlayer;
        }

        // Move both cards away from play area. Move them to the winning mover's side
        Player *player = mPlayers[mCurrentPlayer];
        mDisplay->remove(mCurrentPlayer,
                        mCurrentMoveCards[FirstPlayerTurn],
                        player->noOfMovesWon());
        mDisplay->remove(mCurrentPlayer,
                        mCurrentMoveCards[SecondPlayerTurn],
                        player->noOfMovesWon() + 1);
        player->increaseMovesWon();
        player->addWonCard(mCurrentMoveCards[FirstPlayerTurn]);
        player->addWonCard(mCurrentMoveCards[SecondPlayerTurn]);

        if (global_debug > 0)
        {
            qCDebug(LSKAT_LOG) << "Winner = " << winner << "current =" << mCurrentPlayer;
            qCDebug(LSKAT_LOG) << "   He has won" << player->noOfMovesWon() << "moves.";
            qCDebug(LSKAT_LOG) << "   He has" << player->points() << "points.";
        }
        // Switch move phase (half moves)
        mCurrentMovePhase = FirstPlayerTurn;
    }
    // For the first part of a half move always swap players
    else if (mCurrentMovePhase == FirstPlayerTurn)
    {
        mCurrentPlayer = 1 - mCurrentPlayer;
        // Switch move phase (half moves)
        mCurrentMovePhase = SecondPlayerTurn;
    }

    // Check whether the game is over
    if (gameOver())
    {
        if (global_debug > 0) qCDebug(LSKAT_LOG) << "GAME OVER";
        mGameStatus = Stopped;
        mDisplay->showMove(-1);
        int winner = evaluateGame();
        emit signalGameOver(winner);
    }
    // Game continues
    else
    {
        // Increase move number
        mMoveNumber++;

        // Delayed call to game loop start
        QTimer::singleShot(0, this, &EngineTwo::gameLoopStart);
    }
}

// Check whether the game is over
bool EngineTwo::gameOver()
{
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Move number in game over" << mMoveNumber;
    // Check number of moves. If all moves are done game is over.
    if (mMoveNumber >= 31) return true;
    return false;
}

// Called after game ends..give points to players
int EngineTwo::evaluateGame()
{
    int winner = -1;
    Player *player1 = mPlayers[0];
    Player *player2 = mPlayers[1];

    // Points in game
    int points1 = player1->points();
    int points2 = player2->points();

    // Score awarded
    int score1 = 0;
    int score2 = 0;

    QString text;

    // Game was aborted
    if (points1 + points2 != 120)
    {
        text = i18n("Game was ended - no winner");
        winner = -2;
    }
    // Drawn
    else if (points1 == points2)
    {
        // Add score
        score1 = 1;
        score2 = 1;
        text = i18n("Game is drawn");
        winner = -1;
    }
    // First player won
    else if (points1 > points2)
    {
        text = i18n("Player 1 - %1 won ", player1->name());

        score1 = 2;
        player1->addWonGame(1);
        if (points1 >= 90)
        {
            score1 += 1;  // Schneider
            text = i18n("%1 won with 90 points. Super!", player1->name());
        }
        // Do not use 'else if' here!
        if (points1 >= 120)
        {
            score1 += 1; // Schwarz
            text = i18n("%1 won to nil. Congratulations!", player1->name());
        }
        winner = 0;
    }
    // Second player won
    else
    {
        text = i18n("Player 2 - %1 won ", player2->name());

        score2 = 2;
        player2->addWonGame(1);
        if (points2 >= 90)
        {
            score2 += 1;  // Schneider
            text = i18n("%1 won with 90 points. Super!", player2->name());
        }
        // Do not use 'else if' here!
        if (points2 >= 120)
        {
            score2 += 1; // Schwarz
            text = i18n("%1 won to nil. Congratulations!", player2->name());
        }
        winner = 1;
    }

    // Add score to players
    player1->addScore(score1);
    player2->addScore(score2);

    // Display game over data
    mDisplay->showText(text);
    mDisplay->showScore(0, score1);
    mDisplay->showScore(1, score2);

    return winner;
}

// Start a new game
void EngineTwo::startGame(Suite trump, int startPlayer)
{
    // Set the new trump
    mTrump = trump;

    // Set game status to running
    mGameStatus    = Running;

    // Switch to the start player
    mCurrentPlayer = startPlayer;

    // Set first player to move
    mCurrentMovePhase = FirstPlayerTurn;

    // Game starts, reset moves
    mMoveNumber = 0;

    // Loop players to deal cards
    for (int p = 0; p < 2; p++)
    {
        Player *player = mPlayers[p];
        player->addGame(1); // Increase games of player
        mDisplay->deal(player, p);
    }

    // Delayed call to game loop start via display signal 'dealingDone'
    // (see constructor)
}

// Stop a game
void EngineTwo::stopGame()
{
    // Disable inputs for two players
    for (int i = 0; i < 2; i++)
    {
        mPlayers[i]->stopTurn();
    }

    mDisplay->showMove(-1);

    if (isGameRunning())
    {
        // Display game over data
        QString text = i18n("Game ended");
        mDisplay->showText(text);

        // Set game status to stopped
        mGameStatus    = Stopped;
        emit signalGameOver(-2);
    }
}

// Switch the current player to the given player number.
void EngineTwo::activatePlayer(int playerNumber)
{
    // Disable inputs for two players
    for (int i = 0; i < 2; i++)
    {
        mPlayers[i]->stopTurn();
    }
    // enable input of current player
    Player *player = mPlayers[playerNumber];
    player->startTurn();
    mDisplay->showMove(playerNumber);
    mCurrentPlayer = playerNumber;

    emit signalNextPlayer(player);
}

// Check whether the two cards played are legal, supposed the
// given player is the second one (as the first player always
// plays a legal card)
bool EngineTwo::isLegalMove(int card1, int card2, int playerNumber)
{
    Suite suite1   = Deck::getSuite(card1);
    Suite suite2   = Deck::getSuite(card2);
    CardType type1 = Deck::getCardType(card1);
    CardType type2 = Deck::getCardType(card2);

    // Force trump colour as Jacks count as Trump
    if (type1 == Jack) suite1 = mTrump;
    if (type2 == Jack) suite2 = mTrump;

    // Same suite is always ok
    if (suite1 == suite2) return true;

    // Search if current player has a card of the same colour
    // but didn't play it (if it was played we checked already
    // above)
    Player *p = player(playerNumber);
    bool validMove = true;
    for (int i = 0; i < 8; i++)
    {
        int card = p->getCard(i);
        // Ignore played card
        if (card == card2) continue;

        // Check whether top card is still available - if not try bottom one
        if (card < 0)
        {
            card = p->getCard(i + 8);
        }
        // This card is not available anymore
        if (card < 0) continue;

        // Analyze card
        Suite suite   = Deck::getSuite(card);
        CardType type = Deck::getCardType(card);

        // Force trump colour as Jacks count as Trump
        if (type == Jack) suite = mTrump;

        // Check whether current card matches the first player card
        if (suite == suite1)
        {
            validMove = false;
            break;
        }
    }
    return validMove;
}

// Check who won a move, the first or the second card.
// The first card was played first and take precedence
// when possible. The function returns 0 if the first
// card won, 1 if the second card won.
int EngineTwo::whoWonMove(int card1, int card2, Suite trump)
{
    Suite suite1   = Deck::getSuite(card1);
    Suite suite2   = Deck::getSuite(card2);
    CardType type1 = Deck::getCardType(card1);
    CardType type2 = Deck::getCardType(card2);

    // Two jacks
    if (type1 == Jack && type2 == Jack)
    {
        if (suite1 < suite2) return 0;
        else return 1;
    }
    // One Jack wins always
    if ((int)type1 == (int)Jack) return 0;
    if ((int)type2 == (int)Jack) return 1;

    // Higher card wins if both have same suite
    if (suite1 == suite2)
    {
        // Check Ten because it is not in the right card
        // sequence. Ten is only beaten by Ace
        if (type1 == Ten)
        {
            if (type2 == Ace) return 1;
            else return 0;
        }
        if (type2 == Ten)
        {
            if (type1 == Ace) return 0;
            return 1;
        }

        // Otherwise the higher card wins
        if ((int)card1 < (int)card2) return 0;
        return 1;
    }

    if (global_debug > 0)
    {
        if (suite1 == trump) qCDebug(LSKAT_LOG) << "FIRST card wins TRUMP";
        if (suite2 == trump) qCDebug(LSKAT_LOG) << "SECOND card wins TRUMP";
    }

    // If cards are not of the same suite a trump wins
    if (suite1 == trump) return 0;
    if (suite2 == trump) return 1;

    // In all other cases the first card wins
    return 0;
}
