/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DISPLAY_INTRO_H
#define DISPLAY_INTRO_H

// Qt includes
#include <QGraphicsScene>
#include <QTimer>

// KDE includes

// local includes
#include "abstractdisplay.h"
#include "deck.h"
#include "lskat_debug.h"
#include "thememanager.h"

// Forward declaration
class Player;

/**
 * The display engine for the intro graphics animation.
 */
class DisplayIntro : public AbstractDisplay, public virtual Themable
{
    Q_OBJECT

public:
    /** Animation state of the intro display */
    enum AnimState {Idle, Putting, Turning, Waiting, Clearing, Waiting2};

    /**
     * Constructor for the display.
     * @param deck          The card deck
     * @param scene         The graphics scene to work with
     * @param theme         The theme manager
     * @param advancePeriod The advance period [ms]
     * @param parent        The parent object
     */
    DisplayIntro(Deck *deck, QGraphicsScene *scene, ThemeManager *theme, int advancePeriod, QGraphicsView *parent);

    /**
     * Main theme function. Called by the theme manager. Redraw and resize
     * display.
     */
    void changeTheme() override;

    /**
     * Start the intro.
     */
    void start() override;

    /*
     * Deal cards - unused.
     * @param player The player object
     * @param position The position to place the player (0,1)
     */
    void deal(Player */*player*/, int /*position*/) {}

    /**
     * Play a card on the display - unused.
     */
    void play(int /*cardNumber*/, int /*playerNumber*/, int /*phase*/) override {}

    /**
     * Turn a card on the display - unused.
     */
    void turn(int /*cardNumber*/) override {}

    /**
     * Remove the given card from the display - unused.
     */
    void remove(int /*winnerPosition*/, int /*cardNumber*/, int /*delta*/) override {}

public slots:
    /**
     * Convert the position of a mouse click to a logical
     * game position - unused.
     */
    void convertMousePress(const QPoint &/*mouse*/, int &/*playerNumber*/, int &/*cardNumber*/) override {}

    /**
     * Animation loop. Called by timer.
     */
    void loop();

private:
    // Timer for animation loop
    QTimer *mTimer;
    // Animation counter
    int mAnimCnt;
    // Animation state
    AnimState mState;
    // Text shown?
    bool mTextShown;
};

#endif
