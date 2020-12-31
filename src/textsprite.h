/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEXT_SPRITE_H
#define TEXT_SPRITE_H

// Qt includes
#include <QGraphicsTextItem>

// Local includes
#include "thememanager.h"

/**
 * The sprite for a text on the canvas.
 */
class TextSprite : public QGraphicsTextItem, public virtual Themable
{
public:
    /**
     * Constructor for the sprite.
     * @param text   The text to display
     * @param id     The theme id for the text
     * @param theme  The theme manager
     * @param scene  The graphics scene to use
     */
    TextSprite(const QString &text, const QString &id, ThemeManager *theme, QGraphicsScene *scene);

    /**
     * Constructor for the sprite.
     * @param id     The theme id for the text
     * @param theme  The theme manager
     * @param scene  The graphics scene to use
     */
    TextSprite(const QString &id, ThemeManager *theme, QGraphicsScene *scene);

    /**
     * Set a new text to the sprite.
     * @param text The text to display
     */
    void setText(const QString &text);

    /**
     * Main theme manager function. Called when any theme change like a new
     * theme or a theme size change occurs. This object needs to resize and
     * redraw then.
     */
    void changeTheme() override;
};

#endif
