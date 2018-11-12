#ifndef AI_INPUT_H
#define AI_INPUT_H
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

// KDE includes
#include "lskat_debug.h"

// Local includes
#include "abstractinput.h"
#include "engine_two.h"
#include "deck.h"

/**
 * AI input device
 */
class AiInput : public AbstractInput
{
    Q_OBJECT

public:
    /**
     * Constructor for the input
     * @param engine  The game engine
     * @param parent The parent object
     */
    AiInput(EngineTwo *engine, QObject *parent);

    /**
     * Allow or disallow input with this device
     * @param allowed True if input is allowed
     */
    void setInputAllowed(bool allowed) override;

    /**
     * Retrieve the type of device.
     * @return The device type.
     */
    InputDevice::InputDeviceType type() override {return InputDevice::TypeAiInput;}

public slots:
    /**
     * AI turn is performed.
     */
    void aiTurn();

private:
    /**
     * AI storage of the game board.
     */
    class Board
    {
    public:
        /**
         * Constructor
         */
        Board() {}
        /**
         * Copy constructor
         * @param board Another board
         */
        Board(const Board &board);

        /**
         * Retrieve card of given player on given position
         * @param p   player [0,1]
         * @param pos Position [0-7]
         * @return The card id.
         */
        int card(int p, int pos) const;

        /**
         * Take away card of given player on given position
         * @param p   player [0,1]
         * @param pos Position [0-7]
         * @return The card id.
         */
        int takeCard(int p, int pos);

        /**
         * Check amount of cards at position, that is
         * card on top (2), bottom (1), or none at all (0)
         * @param p   player [0,1]
         * @param pos Position [0-7]
         * @return The amount of cards.
         */
        int cardsAtPos(int p, int pos) const;

        /**
         * Analyze board, e.g. count how many of each suite.
         */
        void analyze();

        /** Cards of both players or -1 for used cards */
        int cards[2][16];
        /** Already played cards */
        int playedCards[32];
        /** How many cards of each suite (4==Trump color) */
        int amountOfSuite[2][5];
        /** How many cards of each type  */
        int amountOfCardType[2][8];
        /** Currently played card of first player or -1 */
        int playedCard;
        /** Points of both players */
        int points[2];
        /** Whose turn is it 0/1 UNUSED */
        int whoseTurn;
        /** True if first player movement phase UNUSED */
        bool firstPlay;
        /** Trump color */
        Suite trump;
    };
    /**
     * AI representation of a move.
     */
    class Move
    {
    public:
        /** The move value */
        double value;
        /** The move position */
        int move;
    };

protected:
    /**
     * Extract the current game board from the engine.
     * @return The game board.
     */
    Board getBoardFromEngine();

    /**
     * Initiate a new move as first player.
     * @param p The current player number.
     * @param board The current game board.
     * @return The best move.
     */
    Move initiateMove(int p, const Board &board);

    /**
     * Answer a move as second player.
     * @param p The current player number.
     * @param board The current game board.
     * @return The best move.
     */
    Move answerMove(int p, const Board &board);

    /**
     * Evaluate the current game board and return a rating.
     * @param p The current player (we)
     * @param current The current game board
     * @return The rating for this situation.
     */
    double evaluteGame(int p, const AiInput::Board &current);

    int findCard(const AiInput::Board &current, Suite sSuite, CardType sCardType) const;
    int amountOfWinningCards(int p, Suite sSuite, const AiInput::Board &current) const;
    bool wouldWinMove(int p, int card, const AiInput::Board &current) const;
    double rulebaseFirstMover(int p, int card, const AiInput::Board &current) const;
    double rulebaseAnswerMover(int p, int card, const AiInput::Board &current) const;
    bool hasAmount(int player, Suite suite, int min, int max, const AiInput::Board &current) const;
    int amountOfOpenCards(int p, const AiInput::Board &current) const;
    bool isLegalMove(int card1, int card2, int pl, const AiInput::Board &current) const;

private:
    /** The game engine used */
    EngineTwo *mEngine;
};

#endif
