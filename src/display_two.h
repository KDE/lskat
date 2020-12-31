/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DISPLAY_TWO_H
#define DISPLAY_TWO_H

// Qt includes
#include <QGraphicsScene>
#include <QHash>
#include <QPixmap>

// KF includes

// Local includes
#include "abstractdisplay.h"
#include "deck.h"
#include "lskat_debug.h"
#include "player.h"
#include "thememanager.h"

// Forward declaration
class PixmapSprite;
class ScoreSprite;
class TextSprite;

/**
 * The display engine for a two player game.
 */
class DisplayTwo : public AbstractDisplay, public virtual Themable
{
    Q_OBJECT

public:
    /**
     * Constructor for the engine
     * @param deck The card deck
     * @param scene The graphics scene to work with
     * @param theme The theme manager
     * @param advancePeriod The advance period [ms]
     * @param parent The parent object
     */
    DisplayTwo(Deck *deck, QGraphicsScene *scene, ThemeManager *theme, int advancePeriod, QGraphicsView *parent);

    /**
     * Start the display.
     */
    void start() override;

    /**
     * Main theme function. Called by the theme manager. Redraw and resize
     * display.
     */
    void changeTheme() override;

    /**
     * Init a player on a given position. Create sprites etc.
     * @param player The player object
     * @param position The position to place the player (0,1)
     */
    void deal(Player *player, int position);

    /**
     * Play a card on the display. The card is moved from
     * its current position to the card deposit.
     * @param cardNumber The card number (0-7)
     * @param playerNumber The player number (0-1)
     * @param phase Movement phase (1st part, 2nd part, etc) [optional]
     */
    void play(int cardNumber, int playerNumber, int phase) override;

    /**
     * Turn a card on the display. The card is flipped backside
     * to frontside.
     * @param cardNumber The card number (0-31)
     */
    void turn(int cardNumber) override;

    /**
     * Remove the given card from the display.
     * @param winnerPosition  The position of the winner (0,1)
     * @param cardNumber The number of the card
     * @param delta Card offset from target position (0,1,2,...)
     */
    void remove(int winnerPosition, int cardNumber, int delta) override;

    /**
     * Display the score on the game board.
     * @param position Which player position
     * @param score The score to display
     */
    void showScore(int position, int score);

    /**
     * Display a text on the game board.
     * @param text The text to display
     */
    void showText(const QString &text);

    /**
     * Show the move icon for the given player
     * @param no The player number (-1: show none)
     */
    void showMove(int no = -1);

public slots:
    /**
     * Convert the position of a mouse click to a logical
     * game position, that is position (up/down) and a
     * card number (0-7)
     * @param mouse        The mouse coordinates [screen coordinates]
     * @param playerNumber The resulting player number [0-1]
     * @param cardNumber   The resulting card number [0-7]
     */
    void convertMousePress(const QPoint &mouse, int &playerNumber, int &cardNumber) override;

    /**
     * Connect a player with the score widget by setting the player properties
     * to the score board.
     * @param player The player to set
     */
    void updatePlayer(Player *player);

    /**
     * Checks whether the shuffling is still ongoing (timer).
     */
    void checkShuffle();

signals:
    /**
     * Signal is emitted when the dealing animation is done.
     * Game can logically start then.
     */
    void dealingDone();

protected:
    /**
     * Calculate the x,y position values from a card number.
     * @param cardNumber The card number [0-7]
     * @param x          The board x coordinate [0-3]
     * @param y          The board y coordinate [0-1]
     */
    void calcXYFromNumber(int cardNumber, int &x, int &y);

    /**
     * Get a card sprite for a card value.
     * @param cardValue The card value [0-31]
     * @return The sprite.
     */
    CardSprite *getCardSprite(int cardValue);

private:
    // Pixmap for movement sprite
    QPixmap *mMovePixmap;
    // Store all move sprites
    QHash<int, PixmapSprite *> mMoveSprites;
    // The score sprites
    ScoreSprite *mScoreBoard[2];
    // The card area background sprites
    PixmapSprite *mCardArea[2];
    // The play area background sprite
    PixmapSprite *mPlayArea;
    // The text sprites
    TextSprite   *mText[3];
};

#endif
