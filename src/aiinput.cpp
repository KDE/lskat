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

#include "aiinput.h"

// Qt includes
#include <QPixmap>
#include <QTimer>

// KDE includes
#include <kdebug.h>

// Local includes
#include "lskatglobal.h"
#include "player.h"

// Constructor for the engine
AiInput::AiInput(EngineTwo *engine, QObject *parent)
    : AbstractInput(parent)
{
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

    if (global_debug > 5) kDebug() << "===================================================";
    if (global_debug > 5) kDebug() << "AI TURN START " << mInputAllowed;

    // Check we are the right player
    if (mId != mEngine->currentPlayer())
    {
        kFatal() << "AI plays for wrong player";
        return;
    }

    // Retrieve game board
    AiInput::Board board = getBoardFromEngine();
    AiInput::Move move;

    if (global_debug > 0) kDebug() << QLatin1String("");

    // Initiate move
    if (mEngine->currentMovePhase() == EngineTwo::FirstPlayerTurn)
    {
        if (global_debug > 5) kDebug() << "Performing initiual move " << mId;
        move = initiateMove(mId, board);
    }
    // Respond to move
    else
    {
        if (global_debug > 5) kDebug() << "Performing answer move " << mId;
        move = answerMove(mId, board);
    }

    // Send out move
    if (global_debug > 5) kDebug() << "AI player" << mId << "moves to" << move.move;
    if (move.move >= 0) emit signalPlayerInput(mId, mId, move.move);
    else kError() << "Illegal AI Move ??? ";
}

// Extract the current game board from the engine
AiInput::Board AiInput::getBoardFromEngine()
{
    Board b;
    int cnt = 0;
    // Reset array
    for (int i = 0; i < 32; i++)
    {
        b.playedCards[i] = -1;
    }

    for (int i = 0; i < 2; i++)
    {
        Player *p = mEngine->player(i);
        for (int c = 0; c < 16; c++)
        {
            int card      = p->getCard(c);
            b.cards[i][c] = card;
            b.points[i]   = p->points();
        }
        for (int c = 0; c < p->noOfMovesWon() * 2; c++)
        {
            int card = p->getWonCard(c);
            b.playedCards[cnt] = card;
            cnt++;
        }
    }

    b.whoseTurn  = mEngine->currentPlayer();
    b.firstPlay  = mEngine->currentMovePhase() == EngineTwo::FirstPlayerTurn;
    b.playedCard = mEngine->playedCard(0);
    b.trump      = mEngine->trump();
    b.analyze();
    return b;
}

// Game evaluation ratings
// Point ratings
const double RATING_SCHWARZ             = 100000.0;
const double RATING_SCHNEIDER           =  70000.0;
const double RATING_WON                 =  50000.0;
const double RATING_REMIS               =  20000.0;
const double RATING_ONE_POINT           =    500.0;
// Tactical ratings
const double RATING_AMOUNT_TRUMPCARD    = 3.0 * RATING_ONE_POINT;
const double RATING_AMOUNT_GRANDCARD    = 15.0 * RATING_ONE_POINT;
const double RATING_AMOUNT_OPENCARD     = 1.5 * RATING_ONE_POINT;
const double RATING_AMOUNT_ACES         = 3.0 * RATING_ONE_POINT;
const double RATING_AMOUNT_TENS         = 1.0 * RATING_ONE_POINT;
const double RATING_AMOUNT_JACKS        = 8.5 * RATING_ONE_POINT;
const double RATING_GOOD_MISSING_SUITE  = 3.5 * RATING_ONE_POINT;

const double RATING_JACK_OF_CLUBS       = 0.8 * RATING_ONE_POINT;
const double RATING_JACK_OF_SPADE       = 0.6 * RATING_ONE_POINT;
const double RATING_JACK_OF_HEART       = 0.4 * RATING_ONE_POINT;
const double RATING_JACK_OF_DIAMOND     = 0.2 * RATING_ONE_POINT;

// Evaluate the current game board and return a rating
double AiInput::evaluteGame(int p, const AiInput::Board &current)
{
    double rating = 0.0;

    // ===== Points evaluation =====
    // Check for won games of our side
    if (current.points[p] == 120) rating += RATING_SCHWARZ;
    else if (current.points[p] >= 90) rating += RATING_SCHNEIDER;
    else if (current.points[p] > 60) rating += RATING_WON;
    else if (current.points[p] == 60) rating += RATING_REMIS;

    // Check for won games of other side
    if (current.points[1 - p] == 120) rating -= RATING_SCHWARZ;
    else if (current.points[1 - p] >= 90) rating -= RATING_SCHNEIDER;
    else if (current.points[1 - p] > 60) rating -= RATING_WON;
    else if (current.points[1 - p] == 60) rating -= RATING_REMIS;

    // Evaluate points
    rating += (current.points[p] - current.points[1 - p]) * RATING_ONE_POINT;

    // ===== Tactical evaluation =====
    // Number of trumps (Stored under index Grand)
    int trump1 = current.amountOfSuite[p][int(Grand)];
    int trump2 = current.amountOfSuite[1 - p][int(Grand)];
    rating += (trump1 - trump2) * RATING_AMOUNT_TRUMPCARD;
    // Increase value of trumps for Grand
    if (current.trump == Grand) rating += (trump1 - trump2) * RATING_AMOUNT_TRUMPCARD;

    // Missing suites
    //if (current.amountOfSuite[p][int(Club)] == 0 && (trump1 > trump2)) rating += RATING_GOOD_MISSING_SUITE;
    //if (current.amountOfSuite[p][int(Spade)] == 0 && (trump1 > trump2)) rating += RATING_GOOD_MISSING_SUITE;
    //if (current.amountOfSuite[p][int(Heart)] == 0 && (trump1 > trump2)) rating += RATING_GOOD_MISSING_SUITE;
    //if (current.amountOfSuite[p][int(Diamond)] == 0 && (trump1 > trump2)) rating += RATING_GOOD_MISSING_SUITE;

    //if (current.amountOfSuite[1 - p][int(Club)] == 0 && (trump1 < trump2)) rating -= RATING_GOOD_MISSING_SUITE;
    //if (current.amountOfSuite[1 - p][int(Spade)] == 0 && (trump1 < trump2)) rating -= RATING_GOOD_MISSING_SUITE;
    //if (current.amountOfSuite[1 - p][int(Heart)] == 0 && (trump1 < trump2)) rating -= RATING_GOOD_MISSING_SUITE;
    //if (current.amountOfSuite[1 - p][int(Diamond)] == 0 && (trump1 < trump2)) rating -= RATING_GOOD_MISSING_SUITE;

    // Number of open cards
    int amount1 = amountOfOpenCards(p, current);
    int amount2 = amountOfOpenCards(1 - p, current);
    rating += (amount1 - amount2) * RATING_AMOUNT_OPENCARD;

    // Card types
    // Aces
    int amountAce1 = current.amountOfCardType[p][int(Ace)];
    int amountAce2 = current.amountOfCardType[1 - p][int(Ace)];
    rating += (amountAce1 - amountAce2) * RATING_AMOUNT_ACES;

    // Tens
    int amountTen1 = current.amountOfCardType[p][int(Ten)];
    int amountTen2 = current.amountOfCardType[1 - p][int(Ten)];
    rating += (amountTen1 - amountTen2) * RATING_AMOUNT_TENS;

    // Jacks
    int amountJack1 = current.amountOfCardType[p][int(Jack)];
    int amountJack2 = current.amountOfCardType[1 - p][int(Jack)];
    rating += (amountJack1 - amountJack2)* RATING_AMOUNT_JACKS;
    //kDebug() << "    Add rating(p=" << p << ") for jacks j1=" << amountJack1 << " j2=" << amountJack2 << " has JC=" << findCard(current, Club, Jack);

    if (findCard(current, Club, Jack) == p)    rating += RATING_JACK_OF_CLUBS;
    if (findCard(current, Spade, Jack) == p)   rating += RATING_JACK_OF_SPADE;
    if (findCard(current, Heart, Jack) == p)   rating += RATING_JACK_OF_HEART;
    if (findCard(current, Diamond, Jack) == p) rating += RATING_JACK_OF_DIAMOND;

    if (findCard(current, Club, Jack) == 1 - p)    rating -= RATING_JACK_OF_CLUBS;
    if (findCard(current, Spade, Jack) == 1 - p)   rating -= RATING_JACK_OF_SPADE;
    if (findCard(current, Heart, Jack) == 1 - p)   rating -= RATING_JACK_OF_HEART;
    if (findCard(current, Diamond, Jack) == 1 - p) rating -= RATING_JACK_OF_DIAMOND;
    //kDebug() << "    Rating after jacks" << rating;

    return rating;
}

// Initiate a new move as first player
AiInput::Move AiInput::initiateMove(int p, const AiInput::Board &board)
{
    AiInput::Move maxMove;
    maxMove.move  = -1;
    maxMove.value = -100.0 * RATING_SCHWARZ; // Absolut minimum score

    // Loop all moves
    for (int m = 0; m < 8; m++)
    {
        AiInput::Board current(board);
        int card = current.takeCard(p, m);
        if (card < 0) continue; // Illegal move
        // Store move
        current.playedCard = card;
        current.whoseTurn = 1 - p;
        if (global_debug > 5) kDebug() << "***** First mover try move on" << m << " (" << Deck::name(card) << ")";
        AiInput::Move answer = answerMove(1 - p, current);
        // Negate answering moves value to get our rating
        double rating = -answer.value;
        if (global_debug > 5) kDebug() << "First mover yields rating of" << rating;

        rating += rulebaseFirstMover(p, card, board);
        if (global_debug > 5) kDebug() << "  rulesbase correction to  " << rating;

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
AiInput::Move AiInput::answerMove(int p, const AiInput::Board &board)
{
    AiInput::Move maxMove;
    maxMove.move  = -1;
    maxMove.value = -100.0 * RATING_SCHWARZ; // Absolut minimum score

    // Loop all moves
    for (int m = 0; m < 8; m++)
    {
        AiInput::Board current(board);
        // kDebug() << "CARD " << m << " is "
        //          << Deck::name(current.cards[p][m]) << " on top of "
        //          << Deck::name(current.cards[p][m + 8]);

        int card = current.takeCard(p, m);
        if (card < 0) continue; // Illegal card

        // Check validity of move
        if (!isLegalMove(current.playedCard, card, p, current)) continue;

        // Check move winner
        int winner = EngineTwo::whoWonMove(current.playedCard, card, current.trump);
        if (global_debug > 5)
        kDebug() << "   Card" << m << " (" << Deck::name(card) << ") is valid "
                    << "countering" << Deck::name(current.playedCard) << " with "
                    << "winner (0:other, 1:we) " << winner;
        int deltaPoints = 0;
        deltaPoints += Deck::getCardValue(current.playedCard);
        deltaPoints += Deck::getCardValue(card);
        // The first mover won
        if (winner == 0)
        {
            current.points[1 - p] += deltaPoints;
        }
        // The second mover won (us)
        else
        {
            current.points[p] += deltaPoints;
        }

        double rating = evaluteGame(p, current);
        rating += rulebaseAnswerMover(p, card, board);

        if (global_debug > 5)
        kDebug() << "   Points after 2nd move " << m << " would be we: "
                    << current.points[p] << "other:" << current.points[1 - p]
                    << "rating is thus" << rating;
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
AiInput::Board::Board(const AiInput::Board &board)
{
    for (int i = 0; i < 2; i++)
    {
        points[i] = board.points[i];
        for (int j = 0; j < 16; j++)
        {
            cards[i][j] = board.cards[i][j];
        }
        for (int j = 0; j < 5; j++)
        {
            amountOfSuite[i][j] = board.amountOfSuite[i][j];
        }
        for (int j = 0; j < 8; j++)
        {
            amountOfCardType[i][j] = board.amountOfCardType[i][j];
        }
    }
    for (int i = 0; i < 32; i++)
    {
        playedCards[i] = board.playedCards[i];
    }
    playedCard = board.playedCard;
    whoseTurn  = board.whoseTurn;
    firstPlay  = board.firstPlay;
    trump      = board.trump;
}

// Retrieve card at given position for given player
int AiInput::Board::card(int p, int pos) const
{
    int card = cards[p][pos];  // 1st card
    if (card < 0)
    {
        card = cards[p][pos + 8];  // 2nd card
    }
    return card;
}

// Check amount of cards at position,
// i.e. card on top (2), bottom(1), or none at all (0)
int AiInput::Board::cardsAtPos(int p, int pos) const
{
    int card = cards[p][pos];  // 1st card
    if (card >= 0) return 2;
    card = cards[p][pos + 8];  // 2nd card
    if (card >= 0) return 1;
    return 0;
}

// Retrieve card at given position for given player and
// reset it (take it away)
int AiInput::Board::takeCard(int p, int pos)
{
    int card = cards[p][pos];  // 1st card
    cards[p][pos] = -1;        // Can do in any case
    if (card < 0)
    {
        card = cards[p][pos + 8];  // 2nd card
        cards[p][pos + 8] = -1;    // Can do in any case
    }
    else
    {
        cards[p][pos + 8] = -cards[p][pos + 8]; // Do not look underneath card
    }
    analyze();
    return card;
}

// Perform pre board analysis
// + Count how many of each color
void AiInput::Board::analyze()
{
    // How many cards of given suite
    for (int pl = 0; pl < 2; pl++)
    {
        for (int i = 0; i < 5; i++) amountOfSuite[pl][i] = 0;
        for (int i = 0; i < 8; i++) amountOfCardType[pl][i] = 0;
        for (int i = 0; i < 8; i++)
        {
            int c         = card(pl, i);
            if (c < 0) continue;
            Suite suite   = Deck::getSuite(c);
            CardType type = Deck::getCardType(c);
            if (suite == trump || type == Jack) amountOfSuite[pl][Grand]++;
            else amountOfSuite[pl][int(suite)]++;
            amountOfCardType[pl][int(type)]++;
        }
    }
}

// Internal return values for find card
const int CARD_CURRENTLY_PLAYED  = 2;
const int CARD_PLAYED            = 3;

// Check where a card is:
// -1: unknown (still covered), 0: player 0, 1: player 1: 2: currently played, 2: already played
int AiInput::findCard(const AiInput::Board &current, Suite sSuite, CardType sCardType) const
{
    // Loop player's cards
    for (int p = 0; p < 2; p++)
    {
        for (int i = 0; i < 8; i++)
        {
            int card      = current.card(p, i);
            if (card < 0) continue;
            Suite suite   = Deck::getSuite(card);
            CardType type = Deck::getCardType(card);
            if (suite == sSuite && type == sCardType)
            {
                //kDebug() << "Found" << Deck::name(sSuite, sCardType) << "at" << p << "," << i;
                return p;
            }
        }
    }

    // Currently played card if any
    int card = current.playedCard;
    if (card >= 0)
    {
        Suite suite   = Deck::getSuite(card);
        CardType type = Deck::getCardType(card);
        if (suite == sSuite && type == sCardType)
        {
            //kDebug() << "Found " << Deck::name(sSuite, sCardType) << " as currently played card";
            return CARD_CURRENTLY_PLAYED;
        }
    }

    // Already played cards
    for (int i = 0; i < 32; i++)
    {
        int card      = current.playedCards[i];
        if (card < 0) continue;
        Suite suite   = Deck::getSuite(card);
        CardType type = Deck::getCardType(card);
        if (suite == sSuite && type == sCardType)
        {
            //kDebug() << "Found " << Deck::name(sSuite, sCardType) << " as played card " << i;
            return CARD_PLAYED;
        }
    }

    return -1;
}

// How many open cards has the given player
int AiInput::amountOfOpenCards(int p, const AiInput::Board &current) const
{
    int cnt = 0;
    for (int i = 0; i < 8; i++)
    {
        if (current.cardsAtPos(p, i) > 0) cnt++;
    }
    return cnt;
}

int AiInput::amountOfWinningCards(int p, Suite sSuite, const AiInput::Board &current) const
{
    int cnt = 0;
    for (int i = 0; i < 8; i++)
    {
        int card      = current.card(p, i);
        if (card < 0) continue;
        Suite suite   = Deck::getSuite(card);
        CardType type = Deck::getCardType(card);
        // Treat jacks as trump
        if (type == Jack) suite = Grand;
        // Check only right suite or trump cards
        if (sSuite != suite) continue;

        // Would we win this card?
        if (wouldWinMove(p, card, current)) cnt++;
    }
    return cnt;
}

// Check whether the given card would win a initial move
bool AiInput::wouldWinMove(int p, int card, const AiInput::Board &current) const
{
    Suite suite   = Deck::getSuite(card);
    CardType type = Deck::getCardType(card);
    if (type == Jack) suite = current.trump;

    // Check only right suite or trump cards
    if (suite != current.trump &&
        current.amountOfSuite[1 - p][suite] == 0 &&
        current.amountOfSuite[1 - p][Grand] > 0)
    {
        //kDebug() << "Player" << (1 - p) << "can use trump against" << Deck::name(suite);
        return false;
    }

    // Other player has suite..check cards
    for (int j = 0; j < 8; j++)
    {
        int ocard      = current.card(1 - p, j);
        if (ocard < 0) continue;
        Suite osuite   = Deck::getSuite(ocard);
        CardType otype = Deck::getCardType(ocard);
        if (otype == Jack) osuite = current.trump;

        // Check only right suite or trump cards
        if (suite == osuite)
        {
            // 0 if card wins ocards, 1 otherwise
            int owinner = EngineTwo::whoWonMove(card, ocard, current.trump);
            if (owinner == 1)
            {
                //kDebug() << "Player" << p << "looses" << Deck::name(card);
                return false;
            }
        }
    }
    //kDebug() << "Player" << p << "wins" << Deck::name(card);
    return true;
}

// Game evaluation ratings

// Try to save a free ten
const double RULE_FREE_TEN        =  20.0 * RATING_ONE_POINT;
// Catch a free ten
const double RULE_CATCH_TEN       =  15.0 * RATING_ONE_POINT;
// Use Ace to hunt free tens
const double RULE_HUNTER_ACE      = -20.0 * RATING_ONE_POINT;
// Do not free Ace to possibly hunt free tens
const double RULE_SUPPORT_ACE     = -14.0 * RATING_ONE_POINT;
// Protect possible free ten with minor card
const double RULE_PROTECT_TEN     = -14.0 * RATING_ONE_POINT;
// Protect possible free ten with removing hunter Ace
const double RULE_KILL_HUNTER_ACE =  25.0 * RATING_ONE_POINT;
// Pull trumps
const double RULE_PULL_TRUMP      =  10.0 * RATING_ONE_POINT;
// Save trumps
const double RULE_SAVE_TRUMP      = -10.0 * RATING_ONE_POINT;
// Stay first mover
const double RULE_FIRST_MOVER     =   4.0 * RATING_ONE_POINT;

// Rule based override over board evaluation to tackle special scenarios
double AiInput::rulebaseFirstMover(int p, int card, const AiInput::Board &current) const
{
    double result = 0.0;
    Suite suite   = Deck::getSuite(card);
    CardType type = Deck::getCardType(card);
    Suite altSuite  = (suite == current.trump || type == Jack)?Grand:suite;

    // Check whether we win the move
    if (wouldWinMove(p, card, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Staying first mover" << Deck::name(card);
        result += RULE_FIRST_MOVER;
    }

    // Protect free ten
    if (type == Ten &&
        findCard(current, suite, Ace) != CARD_PLAYED &&
        findCard(current, suite, Ace) != p &&
        wouldWinMove(p, card, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Saving Ten" << Deck::name(card);
        result += RULE_FREE_TEN;
    }

    // Catch free ten
    if (type == Ace &&
        (1 - p) == findCard(current, suite, Ten) &&
        hasAmount(1 - p, altSuite, 1, 1, current) &&
        wouldWinMove(p, card, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Catching Ten with" << Deck::name(card);
        result += RULE_CATCH_TEN;
    }

    // Saving Ace to try to catch free ten
    if (type == Ace &&
        suite != current.trump &&
        (1 - p) == findCard(current, suite, Ten) &&
        hasAmount(1 - p, suite, 2, 3, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Hunting Ten with" << Deck::name(card);
        result += RULE_HUNTER_ACE;
    }

    // Saving additional cards for hunter Ace
    if (suite != current.trump &&
        type != Jack &&
        type != Ace &&
        (1 - p) == findCard(current, suite, Ten) &&
        (p) == findCard(current, suite, Ace) &&
        hasAmount(1 - p, suite, 2, 3, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Supporting Hunter ACE with" << Deck::name(card);
        if (hasAmount(1 - p, suite, 2, 2, current)) result += RULE_SUPPORT_ACE;
        else result += 0.75 * RULE_SUPPORT_ACE;
    }

    // Saving additional cards for hunted Ten
    if (suite != current.trump &&
        type != Jack &&
        type != Ten &&
        (1 - p) == findCard(current, suite, Ace) &&
        (p) == findCard(current, suite, Ten) &&
        hasAmount(p, suite, 2, 3, current) &&
        hasAmount(1 - p, suite, 2, 11, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Protecting hunted TEN with" << Deck::name(card);
        if (hasAmount(p, suite, 2, 2, current)) result += RULE_PROTECT_TEN;
        else result += 0.5 * RULE_PROTECT_TEN;
    }

    // Killing hunter Ace
    if (suite != current.trump &&
        type != Jack &&
        type != Ten &&
        (1 - p) == findCard(current, suite, Ace) &&
        (p) == findCard(current, suite, Ten) &&
        hasAmount(p, suite, 2, 11, current) &&
        hasAmount(1 - p, suite, 1, 1, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER RULE: Killing hunter ACE with" << Deck::name(card);
        result += RULE_KILL_HUNTER_ACE;
    }

    // Pull trumps
    if (altSuite == Grand)
    {
        int trumpWin1 = amountOfWinningCards(p, altSuite, current);
        //int trumpWin2 = amountOfWinningCards(1 - p, altSuite, current);
        int trump1    = current.amountOfSuite[p][int(altSuite)];
        int trump2    = current.amountOfSuite[1 - p][int(altSuite)];
        // Pull trump
        if (trumpWin1 >= trump2 &&
            trump1 > trump2 &&
            trump2 > 0 &&
            wouldWinMove(p, card, current))
        {
            if (global_debug > 1) kDebug() << "TRIGGER RULE: Pull trump" << Deck::name(card);
            result += RULE_PULL_TRUMP;
        }

        // Do not play trump if other party has none
        if (trump2 == 0)
        {
            if (global_debug > 1) kDebug() << "TRIGGER RULE: Save trump" << Deck::name(card);
            if (trump1 == 1) result += RULE_SAVE_TRUMP;
            else if (trump1 == 2) result += 0.75 * RULE_SAVE_TRUMP;
            else result += 0.5 * RULE_SAVE_TRUMP;
        }
    }

    return result;
}

// Rule based override over board evaluation to tackle special scenarios
double AiInput::rulebaseAnswerMover(int p, int card, const AiInput::Board &current) const
{
    double result = 0.0;
    Suite suite   = Deck::getSuite(card);
    CardType type = Deck::getCardType(card);
    Suite altSuite  = (suite == current.trump || type == Jack)?Grand:suite;

    // Check whether we win the move
    if (wouldWinMove(p, card, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER ANSWER RULE: Becoming first mover" << Deck::name(card);
        result += RULE_FIRST_MOVER;
    }

    // Saving Ace to try to catch free ten
    if (type == Ace &&
        suite != current.trump &&
        (1 - p) == findCard(current, suite, Ten) &&
        hasAmount(1 - p, suite, 1, 2, current) &&
        hasAmount(p, suite, 2, 3, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER ANSWER RULE: Hunting Ten with" << Deck::name(card);
        result += RULE_HUNTER_ACE;
    }

    // Saving additional cards for hunter Ace
    if (suite != current.trump &&
        type != Jack &&
        type != Ace &&
        (1 - p) == findCard(current, suite, Ten) &&
        (p) == findCard(current, suite, Ace) &&
        hasAmount(1 - p, suite, 1, 2, current))
    {
        if (global_debug > 1) kDebug() << "TRIGGER ANSWER RULE: Supporting Hunter ACE with" << Deck::name(card);
        if (hasAmount(1 - p, suite, 1, 1, current)) result += RULE_SUPPORT_ACE;
        else result += 0.75 * RULE_SUPPORT_ACE;
    }

    return result;
}

// Check whether the given player has between [min,max] cards of the given suite
bool AiInput::hasAmount(int player, Suite suite, int min, int max, const AiInput::Board &current) const
{
    if (current.amountOfSuite[player][int(suite)] >= min &&
        current.amountOfSuite[player][int(suite)] <= max)
    {
        return true;
    }
    return false;
}

// Check whether the two cards played are legal, supposed the given player is
// the second one (as the first player always plays a legal card)
bool AiInput::isLegalMove(int card1, int card2, int pl, const AiInput::Board &current) const
{
    Suite suite1   = Deck::getSuite(card1);
    Suite suite2   = Deck::getSuite(card2);
    CardType type1 = Deck::getCardType(card1);
    CardType type2 = Deck::getCardType(card2);

    // Force trump colour as Jacks count as Trump
    if (type1 == Jack) suite1 = current.trump;
    if (type2 == Jack) suite2 = current.trump;

    // Same suite is always OK
    if (suite1 == suite2) return true;

    // Search if current player has a card of the same colour but didn't
    // play it (if it was played we checked already above)
    for (int i = 0; i < 8; i++)
    {
        int card = current.card(pl, i);
        // This card is not available anymore
        if (card < 0) continue;
    //    if (type1 == Jack) kDebug() << "Check card 2 " << Deck::name(card);

        // Ignore played card
        if (card == card2) continue;

        // Analyze card
        Suite suite   = Deck::getSuite(card);
        CardType type = Deck::getCardType(card);

        // Force trump colour as Jacks count as Trump
        if (type == Jack) suite = current.trump;

        // Check whether current card matches the first player card
        if (suite == suite1)
        {
            return false;
        }
    }
    // if (type1 == Jack) kDebug() << "ALLOWED  ";
    return true;
}

#include "aiinput.moc"
