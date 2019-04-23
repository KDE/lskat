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

#include "scoresprite.h"
#include "lskat_debug.h"

// General includes
#include <math.h>

// Qt includes
#include <QFont>
#include <QGraphicsScene>

// KDE includes
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

// Constructor for the score sprite
ScoreSprite::ScoreSprite(const QString &id, ThemeManager *theme, int advancePeriod, int no, QGraphicsScene *scene)
           :  Themable(id, theme), PixmapSprite(advancePeriod, no, scene)
{
    // Create all sub sprites
    mName    = new QGraphicsTextItem(this);
    mPoints  = new QGraphicsTextItem(this);
    mScore   = new QGraphicsTextItem(this);
    mGames   = new QGraphicsTextItem(this);
    mInput   = new PixmapSprite(QStringLiteral("scoreinput"), theme, advancePeriod, 0, scene);
    if (!mInput) qCCritical(LSKAT_LOG) << "Cannot load sprite " << "scoreinput";
    mInput->setParentItem(this);
    mInput->setOffsetStatus(false);
    mInputFrame = 0;

    mTrump   = new PixmapSprite(QStringLiteral("scoretrump"), theme, advancePeriod, 0, scene);
    if (!mTrump) qCCritical(LSKAT_LOG) << "Cannot load sprite " << "scoretrump";
    mTrump->setParentItem(this);
    mTrump->setOffsetStatus(false);
    mTrumpFrame = 0;

    // Redraw us
    if (theme) theme->updateTheme(this);
}

// Destructor
ScoreSprite::~ScoreSprite()
{
    // Clean up
    delete mName;
    delete mPoints;
    delete mScore;
    delete mGames;
    delete mInput;
    delete mTrump;
}

// Redraw the theme.
void ScoreSprite::changeTheme()
{
    // The main display is handled by the parent
    PixmapSprite::changeTheme();

    // Retrieve our size
    double width  = this->boundingRect().width();
    double height = this->boundingRect().height();

    // Retrieve theme data
    KConfigGroup config = thememanager()->config(id());
    QPointF posName     = config.readEntry("posName", QPointF(1.0, 1.0));
    QPointF posPoints   = config.readEntry("posPoints", QPointF(1.0, 1.0));
    QPointF posScore    = config.readEntry("posScore", QPointF(1.0, 1.0));
    QPointF posGames    = config.readEntry("posGames", QPointF(1.0, 1.0));

    // Calculate proper font size
    double fontHeightPoints = config.readEntry("fontHeightPoints", 1.0);
    fontHeightPoints       *= height;
    double fontHeight       = config.readEntry("fontHeight", 1.0);
    fontHeight             *= height;
    double fontWidthUpper   = config.readEntry("fontWidthUpper", 1.0);
    double fontWidthLower   = config.readEntry("fontWidthLower", 1.0);
    fontWidthUpper         *= width;
    fontWidthLower         *= width;

    // Retrieve font color
    QColor fontColor;
    fontColor = config.readEntry("fontColorPlayer", QColor(Qt::white));

    // Set position of sub sprites
    mName->setPos(posName.x() * width, posName.y() * height);
    mPoints->setPos(posPoints.x() * width, posPoints.y() * height);
    mScore->setPos(posScore.x() * width, posScore.y() * height);
    mGames->setPos(posGames.x() * width, posGames.y() * height);

    // Create and set current font
    QFont fontPoints;
    fontPoints.setPixelSize(int(fontHeightPoints));
    QFont font;
    font.setPixelSize(int(fontHeight));

    // Set font and color for all text items
    mName->setFont(font);
    mPoints->setFont(fontPoints);
    mScore->setFont(font);
    mGames->setFont(font);

    mName->setDefaultTextColor(fontColor);
    mPoints->setDefaultTextColor(fontColor);
    mScore->setDefaultTextColor(fontColor);
    mGames->setDefaultTextColor(fontColor);

    mName->setTextWidth(fontWidthUpper);
    mPoints->setTextWidth(fontWidthUpper);
    mScore->setTextWidth(fontWidthLower);
    mGames->setTextWidth(fontWidthLower);

    QPoint offset = thememanager()->getOffset();

    // Restore the frame of the input device sprite
    if (mInputFrame >= 0) mInput->setFrame(mInputFrame);
}

// QGI advance method
void ScoreSprite::advance(int phase)
{
    // Advance time and animation etc
    PixmapSprite::advance(phase);
}

// Store and display the name of a player
void ScoreSprite::setPlayerName(const QString &s)
{
    mName->setPlainText(s);
    update();
}

// Store and display amount of points
void ScoreSprite::setPoints(int points)
{
    QString s = QStringLiteral("%1").arg(points, 3);
    mPoints->setPlainText(s);
    update();
}

// Store and display score
void ScoreSprite::setScore(int score)
{
    QString s = i18nc("Score in score widget", "Score: %1", score);
    mScore->setPlainText(s);
    update();
}

// Store and display amount of games
void ScoreSprite::setGames(int won, int all)
{
    QString s = i18nc("Won and overall games in score widget", "Games: %1 / %2", won, all);
    mGames->setPlainText(s);
    update();
}

// Store and display input device
void ScoreSprite::setInput(int device)
{
    mInputFrame = device;
    mInput->setFrame(mInputFrame);
    mInput->show();
    update();
}

// Store and display trump icon
void ScoreSprite::setTrump(int suite)
{
    mTrumpFrame = suite;
    mTrump->setFrame(mTrumpFrame);
    mTrump->show();
    update();
}
