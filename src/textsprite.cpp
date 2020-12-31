/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "textsprite.h"
#include "lskat_debug.h"

// Qt includes
#include <QGraphicsScene>
#include <QFont>
// KF includes
#include <KConfigGroup>

// Constructor for the sprite
TextSprite::TextSprite(const QString &text, const QString &id, ThemeManager *theme, QGraphicsScene *scene)
          : Themable(id, theme), QGraphicsTextItem(nullptr)
{
    scene->addItem(this);
    setPlainText(text);
    hide();

    if (theme) theme->updateTheme(this);
}

// Constructor for the sprite
TextSprite::TextSprite(const QString &id, ThemeManager *theme, QGraphicsScene *scene)
          : Themable(id, theme), QGraphicsTextItem(nullptr)
{
    scene->addItem(this);
    hide();

    if (theme) theme->updateTheme(this);
}

void TextSprite::setText(const QString &text)
{
    setPlainText(text);
    thememanager()->updateTheme(this);
}

// Main themable function. Called for any theme change. The sprites needs to
// resize and redraw here.
void TextSprite::changeTheme()
{
    // Get scaling change
    double scale    = thememanager()->getScale();
    Themable::setScale(scale);

    // Retrieve theme data from configuration
    KConfigGroup config = thememanager()->config(id());
    QPoint offset = thememanager()->getOffset();

    // Size
    double width  = config.readEntry("width", 1.0);
    double height = config.readEntry("height", 0.0);
    width *= scale;
    height *= scale;

    // Position
    QPointF pos = config.readEntry("pos", QPointF(1.0, 1.0));
    pos *= scale;
    setPos(pos.x(), pos.y());

    // z-Value
    double zValue = config.readEntry("zValue", 0.0);
    setZValue(zValue);

    // Text font
    bool bold = config.readEntry("bold", false);
    QFont font;
    font.setPixelSize(int(height));
    font.setBold(bold);
    setFont(font);
    if (width < boundingRect().width())
    {
        setTextWidth(width);
    }

    // Retrieve font color
    QColor fontColor;
    fontColor = config.readEntry("fontColor", QColor(Qt::white));
    setDefaultTextColor(fontColor);

    // Centering
    bool center = config.readEntry("center", false);
    resetTransform();
    if (center)
    {
        setTransform(QTransform::fromTranslate(-boundingRect().width() / 2.0 + offset.x(), offset.y()), true);
    }
    else
    {
        setTransform(QTransform::fromTranslate(offset.x(), offset.y()), true);
    }

    update();
}
