/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SCORE_SPRITE_H
#define SCORE_SPRITE_H

// Qt includes
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>

// Local includes
#include "pixmapsprite.h"
#include "player.h"
#include "thememanager.h"

/**
 * The sprite for a score board on the canvas.
 */
class ScoreSprite : public PixmapSprite
{
public:
    /**
     * Constructor for the score sprite.
     * @param id              The theme id
     * @param theme           The theme manager
     * @param advancePeriod   The canvas advance period [ms]
     * @param no              A used defined number (unused)
     * @param scene           The graphics scene
     */
    ScoreSprite(const QString &id, ThemeManager *theme, int advancePeriod, int no, QGraphicsScene *scene);

    /**
     * Destructor
     */
    ~ScoreSprite();

    /**
     * Standard QGI advance function.
     * @param phase The advance phase
     */
    void advance(int phase) override;

    /**
     * Retrieve the type of QGI. This item is UserType+10
     * @return The type of item.
     */
    int type() const override {return QGraphicsItem::UserType + 10;}

    /**
     * Main theme change function. On call of this the item needs to redraw and
     * resize.
     */
    void changeTheme() override;

    /**
     * Store the player name.
     * @param s  The name
     */
    void setPlayerName(const QString &s);

    /**
     * Store the amounts of points for a player.
     * @param points The amount of current points.
     */
    void setPoints(int points);

    /**
     * Store the score for a player.
     * @param score  The score
     */
    void setScore(int score);

    /**
     * Store the amounts of games.
     * @param won  The amount of won games
     * @param all  The amount of all games
     */
    void setGames(int won, int all);

    /**
     * Store input device for a player.
     * @param device  The device number [0-2]
     */
    void setInput(int device);

    /**
     * Store trump icon for a player.
     * @param suite  The trump suite
     */
    void setTrump(int suite);

private:
    // Text for name of player
    QGraphicsTextItem *mName;

    // Text for current points
    QGraphicsTextItem *mPoints;

    // Text for score
    QGraphicsTextItem *mScore;

    // Text for games
    QGraphicsTextItem *mGames;

    // Sprite for the input device
    PixmapSprite *mInput;

    // Frame number of the input device sprite
    int mInputFrame;

    // Sprite for the input device
    PixmapSprite *mTrump;

    // Frame number of the input device sprite
    int mTrumpFrame;
};

#endif
