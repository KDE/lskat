/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "display_intro.h"

// General includes
#include <cmath>

// Qt includes
#include <QPoint>

// KF includes
#include <KLocalizedString>
#include <KConfigGroup>

// Local includes
#include "cardsprite.h"
#include "textsprite.h"

#define WAIT_CNT       100  /* Wait this [ms] before clearing board */

// Constructor for the display
DisplayIntro::DisplayIntro(Deck *deck, QGraphicsScene *theScene, ThemeManager *theme,
                           int advancePeriod, QGraphicsView *parent)
            : Themable(QStringLiteral("display_intro"), theme), AbstractDisplay(deck, theScene, theme, advancePeriod, parent)
{
    mTextShown = false;

    // Choose a background color
    scene()->setBackgroundBrush(QColor(0, 0, 128));

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &DisplayIntro::loop);
    mTimer->stop();

    // Redraw
    if (theme) theme->updateTheme(this);
}

// Called by thememanager when theme or theme geometry changes. Redraw and resize
// this display.
void DisplayIntro::changeTheme()
{
    // Retrieve theme data
    KConfigGroup config = thememanager()->config(id());

    // Retrieve background pixmap
    QString bgsvgid = config.readEntry("background-svgid");
    QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, scene()->sceneRect().size().toSize());
    scene()->setBackgroundBrush(pixmap);
    mView->update();
}

// Start the intro.
void DisplayIntro::start()
{
    mAnimCnt = 0;
    mState   = Putting;
    mTimer->start(50);

    // Stop all card sprites
    for (CardSprite *sprite : std::as_const(mCards)) {
        sprite->stop();
    }
}

// Animation loop
void DisplayIntro::loop()
{
    int no = mCards.size();
    // Catch no card error
    if (no < 1) return;

    // Retrieve theme data
    KConfigGroup cardconfig = thememanager()->config(QStringLiteral("card"));
    double card_width       = cardconfig.readEntry("width", 1.0);
    KConfigGroup config     = thememanager()->config(id());
    QPointF start_shift     = config.readEntry("start-shift", QPointF(1.0, 1.0));
    QPointF start_pos       = config.readEntry("start-pos", QPointF(1.0, 1.0));
    double time_clear_in    = config.readEntry("time-clear-in", 1.0);
    double time_clear_out   = config.readEntry("time-clear-out", 1.0);
    double aspectRatio      = thememanager()->aspectRatio();

    // Display the intro text delayed
    if (mAnimCnt == 2  && mState == Putting && !mTextShown)
    {
        mTextShown = true;
        QString s1 = i18nc("Title of the game - line 1", "Lieutenant Skat");
        QString s2 = i18nc("Title of the game - line 2", "for");
        QString s3 = i18nc("Title of the game - line 3", "K D E");

        mSprites.reserve(6);

        // Text sprite title foreground
        TextSprite *text1a = new TextSprite(s1, QStringLiteral("name-front"), mTheme, scene());
        mSprites.append(text1a);
        text1a->show();

        // Text sprite title background
        TextSprite *text1b = new TextSprite(s1, QStringLiteral("name-back"), mTheme, scene());
        mSprites.append(text1b);
        text1b->show();

        // Text sprite title foreground
        TextSprite *text2a = new TextSprite(s2, QStringLiteral("for-front"), mTheme, scene());
        mSprites.append(text2a);
        text2a->show();

        // Text sprite title background
        TextSprite *text2b = new TextSprite(s2, QStringLiteral("for-back"), mTheme, scene());
        mSprites.append(text2b);
        text2b->show();

        // Text sprite title foreground
        TextSprite *text3a = new TextSprite(s3, QStringLiteral("kde-front"), mTheme, scene());
        mSprites.append(text3a);
        text3a->show();

        // Text sprite title background
        TextSprite *text3b = new TextSprite(s3, QStringLiteral("kde-back"), mTheme, scene());
        mSprites.append(text3b);
        text3b->show();
    }

    // Display a card
    if (mAnimCnt < no && mState == Putting)
    {
        double factor = double(mAnimCnt) / double(no - 1);
        double fsin = sin(factor * M_PI);

        CardSprite *sprite = mCards[mAnimCnt];

        QPointF pos;
        if (mAnimCnt % 2 == 0)
        {
            pos  = QPointF(start_pos.x(), start_pos.y());
            pos += QPointF(start_shift.x() * fsin, start_shift.y() * factor);
        }
        else
        {
            pos  = QPointF(1.0 - start_pos.x() - card_width, start_pos.y());
            pos += QPointF(-start_shift.x() * fsin, start_shift.y() * factor);
        }
        sprite->setBackside();
        sprite->setPosition(pos);
        sprite->setZValue(50 + mAnimCnt);
        sprite->show();
        mAnimCnt++;
    }
    // Change state to turning
    else if (mState == Putting)
    {
        mState   = Turning;
        mAnimCnt = 0;
    }
    // Turn cards
    else if (mAnimCnt < no && mState == Turning)
    {
        CardSprite *sprite = mCards[mAnimCnt];
        sprite->setTurning(true);
        mAnimCnt++;
    }
    // Change state to waiting
    else if (mState == Turning)
    {
        mState   = Waiting;
        mAnimCnt = 0;
    }
    // Wait
    else if (mAnimCnt < WAIT_CNT && mState == Waiting)
    {
        mAnimCnt++;
    }
    // Change state to clearing the board
    else if (mState == Waiting)
    {
        mState   = Clearing;
        mAnimCnt = 0;
    }
    // Clear the board, step 1
    else if (mAnimCnt == 0 && mState == Clearing)
    {
        for (int i = 0; i < no; i++)
        {
            CardSprite *sprite = mCards[i];
            sprite->setMove(QPointF((1.0 - card_width) / 2.0, (1.0 / aspectRatio - card_width) / 2.0), time_clear_in);
        }
        mAnimCnt++;
    }
    // Clear the board, step 2
    else if (mAnimCnt < 30 && mState == Clearing)
    {
        mAnimCnt++;
    }
    // Clear the board, step 3 and change state to waiting
    else if (mState == Clearing)
    {
        for (int i = 0; i < no; i++)
        {
            double r = 1.0;
            double x = r * cos(double(i) / double(no - 1) * M_PI * 2.0) + 0.5;
            double y = r * sin(double(i) / double(no - 1) * M_PI * 2.0) + 0.5;
            CardSprite *sprite = mCards[i];
            sprite->setMove(QPointF(x, y / aspectRatio), time_clear_out);
        }
        mState = Waiting2;
        mAnimCnt = 0;
    }
    // Wait
    else if (mAnimCnt < WAIT_CNT && mState == Waiting2)
    {
        mAnimCnt++;
    }
    // Restart cycle
    else if (mState == Waiting2)
    {
        for (int i = 0; i < no; i++)
        {
            CardSprite *sprite = mCards[i];
            sprite->stop();
        }

        mState   = Putting;
        mAnimCnt = 0;
    }
}

#include "moc_display_intro.cpp"
