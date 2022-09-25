/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DECK_H
#define DECK_H

// Qt includes
#include <QList>
#include <QObject>
#include <QRandomGenerator>

#include "lskat_debug.h"

/**
 * The card, suite and trump names
 */
namespace CardDeck
{
// Suite names
enum Suite    {Club = 0, Spade = 1, Heart = 2, Diamond = 3, Grand = 4};
// Card types.
enum CardType {Ace = 0, King = 1, Queen = 2, Jack = 3, Ten = 4, Nine = 5, Eight = 6, Seven = 7};
}

using namespace CardDeck;

/**
 * The deck stores a card deck, handles random functions like
 * shuffling and deals the cards.
 */
class Deck : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor for the deck.
     * @param seed The random seed
     * @param parent The parent object
     */
    Deck(quint32 seed, QObject *parent);

    /**
     * Destructor
     */
    ~Deck() override;

    /**
     * Retrieve number of cards in this deck.
     * @return Number of cards.
     */
    int cardNumber();

    /**
     * Shuffle the cards and reset the pile.
     */
    void shuffle();

    /**
     * Draw a random trump from all cards. This is
     * done by drawing a random card and choosing its
     * suite as trump. In case a Jack is drawn a Grand
     * is made trump.
     * @return A random trump.
     */
    Suite randomTrump();

    /**
     * Draw a random card out of the pile of existing ones.
     * @return A random card
     */
    int drawCard();

    /**
     * Get the suite (Club, ...) of a given card (number)
     * @param card The card number
     * @return The suite.
     */
    static Suite getSuite(int card);

    /**
     * Get the card type (ace, king, ...) of a given card (number)
     * @param card The card number
     * @return The card type (ace, king, ...).
     */
    static CardType getCardType(int card);

    /**
     * Get the card number (0, 1, ...) of a card given by suite and type.
     * @param suite The card suite (Club, ...)
     * @param type The card type (Ace, ...)
     * @return The card number.
     */
    static int getCard(Suite suite, CardType type);

    /**
     * Get the value in points of the given card (number).
     * @param card The card number
     * @return The card value [0, 2, 3, 4, 10, 11].
     */
    static int getCardValue(int card);

    /**
     * Returns a verbose name for a card.
     * @param suite The card suite
     * @param type  The card type
     * @return A descriptive string.
     */
    static QString name(Suite suite, CardType type);

    /**
     * Returns a verbose name for a card.
     * @param card The card as integer
     * @return A descriptive string.
     */
    static QString name(int card);

    /**
     * Returns a verbose name for a card type.
     * @param type The card type (e.g. Ace)
     * @return A descriptive string.
     */
    static QString name(CardType type);

    /**
     * Returns a verbose name for a suite.
     * @param suite The suite (e.g. Heart)
     * @return A descriptive string.
     */
    static QString name(Suite suite);

private:
    // Array of card numbers
    QList<int> mCards;
    // Random generator
    QRandomGenerator mRandom;
};

#endif
