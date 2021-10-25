/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLAYER_H
#define PLAYER_H

// Qt includes
#include <QObject>
#include <QVector>

// KF includes
#include <KConfig>

// Local includes
#include "lskat_debug.h"
#include "deck.h"

// Forward declaration
class AbstractInput;

using namespace CardDeck;

/**
 * A player of the game.
 */
class Player : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor for the player
     * @param id     The player number
     * @param parent The parent object
     */
    explicit Player(int id, QObject *parent = nullptr);

    /**
     * Destructor
     */
    ~Player() override;

    /**
     * Retrieve the player number.
     * @return The id.
     */
    int id() {return mId;}

    /**
     * Retrieve card value at given logical position
     * @param playerNumber (0..max number of cards of player)
     * @return Card number
     */
    int getCard(int playerNumber);

    /**
     * Remove a card from the given position. Typically if the
     * card was played.
     * @param cardPosition (0..max number of cards of player)
     */
    void deleteCard(int cardPosition);

    /**
     * Add a card to the given position. Typically if the
     * card was played back.
     * @param cardPosition (0..max number of cards of player)
     * @param card The actual card
     */
    void addCard(int cardPosition, int card);

    /**
     * Set this player to start a turn
     */
    void startTurn();

    /**
     * Set this player to stop a turn
     */
    void stopTurn();

    /**
     * Deal a number of cards to this player
     * @param amount The amount of cards
     */
    void deal(int amount);

    /**
     * Increases the number of moves by one for this player.
     * @param amount Increase by this amount (default 1)
     */
    void increaseMovesWon(int amount = 1);

    /**
     * Retrieve the number of won moves for this player.
     * @return The amount of won moves for this player.
     */
    int noOfMovesWon();

    /**
     * Adds a card which is one in a move to this player.
     * Also increase this players point by the points of the card. Note, this
     * method has to be called with both cards of a move!
     * @param card The card to store
     */
    void addWonCard(int card);

    /**
     * Retrieve a card won by this player.
     * @param no The number of the card. Can be 0..2*noOfMovesWon().
     * @return The card.
     */
    int getWonCard(int no);

    /**
     * Retrieve the amount of points this player has.
     * @return The amount of points (0-120).
     */
    int points();

    /**
     * Set the points of the player.
     * Emits the signal signalUpdatePoints(int)
     * @param points The points.
     */
    void setPoints(int points);

    /**
     * Retrieve the name of the player.
     * @return The player's name.
     */
    QString name();

    /**
     * Set the name of the player.
     * Emits the signal signalUpdateName(int)
     * @param name The new name.
     */
    void setName(const QString &name);

    /**
     * Add a number of won games to the overall player statistic.
     * @param amount The amount of won games
     */
    void addWonGame(int amount);

    /**
     * Retrieve the number of won games.
     * @return The number of games won.
     */
    int wonGames();

    /**
     * Add a number of games to the overall player statistic.
     * @param amount The amount of games
     */
    void addGame(int amount);

    /**
     * Retrieve the number of games.
     * @return The overall number of games.
     */
    int games();

    /**
     * Add a score to the overall player statistic.
     * @param amount The score amount
     */
    void addScore(int amount);

    /**
     * Retrieve the overall score.
     * @return The score.
     */
    int score();

    /**
     * Retrieve the input device of the player
     * @return The input device.
     */
    AbstractInput *input();

    /**
     * Set the input for this player.
     * @param input The input device.
     */
    void setInput(AbstractInput *input);

    /**
     * Set the deck for drawing cards.
     *  @param deck The deck
     */
    void setDeck(Deck *deck);

    /**
     * Refresh GUI by emitting a signal.
     */
    void refresh();

    /**
     * Clear the all time statistics of this player.
     */
    void clear();

    /**
     * Set the trump to the player
     * @param trump The trump suite
     */
    void setTrump(Suite trump);

    /**
     * Retrieve the trump.
     * @return The trump suite.
     */
    Suite trump();

    /**
     * Saves the properties
     * @param config The config group object.
     */
    void save(KConfigGroup &config);

    /**
     * Read properties.
     * @param config The config group object.
     */
    void load(KConfigGroup &config);

Q_SIGNALS:
    /**
     * Notify change of player data.
     * @param p This player
     */
    void signalUpdate(Player *p);

private:
    // Players input device
    AbstractInput *mInput;
    // Card deck
    Deck *mDeck;
    // Our cards
    QVector<int> mCards;
    // Our won cards
    QList<int> mWonCards;
    // Our points
    int mPoints;
    // Our id
    int mId;
    // Number of moves won in the current game
    int mMovesWon;
    // The current trump
    Suite mTrump;

    // These attributes need to be saved
    // Our name
    QString mName;
    // Overall games won for this player
    int mGamesWon;
    // Overall score for this player
    int mScore;
    // Overall number of games for this player
    int mNumberOfGames;
};

#endif
