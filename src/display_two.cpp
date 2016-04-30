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

#include "display_two.h"

// Qt includes
#include <QPoint>
#include <QTimer>

// KDE includes
#include "lskat_debug.h"
#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes
#include "abstractinput.h"
#include "cardsprite.h"
#include "textsprite.h"
#include "pixmapsprite.h"
#include "scoresprite.h"

// Display defines
#define TIME_DELAY_SHUFFLE       100 /* Delay time in shuffling in [ms] */
#define TIME_DELAY_AFTER_SHUFFLE 500 /* Extra delay after shuffling [ms] */

// Constructor for the engine
DisplayTwo::DisplayTwo(Deck *deck, QGraphicsScene *theScene, ThemeManager *theme,
                       int advancePeriod, QGraphicsView *parent)
          : Themable(QLatin1String("display_two"), theme), AbstractDisplay(deck, theScene, theme, advancePeriod, parent)

{
    // Choose a background color
    scene()->setBackgroundBrush(QColor(0, 0, 128));

    // Create move icon
    mMoveSprites[0] = new PixmapSprite(QLatin1String("moveicon0"), mTheme, mAdvancePeriod, 0, scene());
    if (!mMoveSprites[0]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "moveicon";
    mSprites.append(mMoveSprites[0]);

    mMoveSprites[1] = new PixmapSprite(QLatin1String("moveicon1"), mTheme, mAdvancePeriod, 1, scene());
    if (!mMoveSprites[1]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "moveicon";
    mSprites.append(mMoveSprites[1]);

    // Create score board
    mScoreBoard[0] = new ScoreSprite(QLatin1String("scoreboard0"), mTheme, mAdvancePeriod, 0, scene());
    if (!mScoreBoard[0]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "scoreboard0";
    mSprites.append(mScoreBoard[0]);

    mScoreBoard[1] = new ScoreSprite(QLatin1String("scoreboard1"), mTheme, mAdvancePeriod, 1, scene());
    if (!mScoreBoard[1]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "scoreboard0";
    mSprites.append(mScoreBoard[1]);

    // Create card area
    mCardArea[0] = new PixmapSprite(QLatin1String("cardarea0"), mTheme, mAdvancePeriod, 0, scene());
    if (!mCardArea[0]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "cardarea0";
    mSprites.append(mCardArea[0]);

    mCardArea[1] = new PixmapSprite(QLatin1String("cardarea1"), mTheme, mAdvancePeriod, 1, scene());
    if (!mCardArea[1]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "cardarea1";
    mSprites.append(mCardArea[1]);

    // Create play area
    mPlayArea = new PixmapSprite(QLatin1String("playarea"), mTheme, mAdvancePeriod, 0, scene());
    if (!mPlayArea) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "playarea";
    mSprites.append(mPlayArea);

    // Create text sprites
    mText[0] = new TextSprite(QLatin1String("scoretext0"), mTheme, scene());
    if (!mText[0]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "scoretext0";
    mSprites.append(mText[0]);

    mText[1] = new TextSprite(QLatin1String("scoretext1"), mTheme, scene());
    if (!mText[1]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "scoretext1";
    mSprites.append(mText[1]);

    mText[2] = new TextSprite(QLatin1String("resulttext"), mTheme, scene());
    if (!mText[2]) qCCritical(LSKAT_LOG) << "Cannot load sprite" << "resulttext";
    mSprites.append(mText[2]);

    // Redraw
    if (theme) theme->updateTheme(this);
}

// Called by thememanager when theme or theme geometry changes. Redraw and
// resize this display.
void DisplayTwo::changeTheme()
{
    // Retrieve theme data
    KConfigGroup config = thememanager()->config(id());

    // Retrieve background pixmap
    QString bgsvgid = config.readEntry("background-svgid");
    QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, scene()->sceneRect().size().toSize());
    scene()->setBackgroundBrush(pixmap);
    mView->update();
}

// Start display
void DisplayTwo::start()
{
    // Stop all card sprites
    for (int i = 0; i < mCards.size(); i++)
    {
        CardSprite *sprite = mCards[i];
        sprite->stop();
    }

    // Hide or show sprites
    mMoveSprites[0]->hide();
    mMoveSprites[1]->hide();
    mScoreBoard[0]->show();
    mScoreBoard[1]->show();
    mCardArea[0]->show();
    mCardArea[1]->show();
    mPlayArea->show();
    mText[0]->hide();
    mText[1]->hide();
    mText[2]->hide();
}

// Connect a player with the score widget
void DisplayTwo::updatePlayer(Player *player)
{
    int id = player->id();
    mScoreBoard[id]->setPlayerName(player->name());
    mScoreBoard[id]->setPoints(player->points());
    mScoreBoard[id]->setScore(player->score());
    mScoreBoard[id]->setGames(player->wonGames(), player->games());
    mScoreBoard[id]->setInput(player->input()->type());
    mScoreBoard[id]->setTrump(player->trump());
}

// Init a player on a given screen/board position (0,1)
void DisplayTwo::deal(Player *player, int position)
{
    if (position != 0 && position != 1)
    {
        qCCritical(LSKAT_LOG) << "Wrong player position" << position;
        return;
    }
    if (!player)
    {
        qCCritical(LSKAT_LOG) << "No player given";
        return;
    }

    KConfigGroup config = thememanager()->config(id());
    QPointF deck_pos    = config.readEntry("deck-pos", QPointF(1.0, 1.0));

    // Start offset for display
    QPointF board_pos   = config.readEntry("board-pos1", QPointF(1.0, 1.0));
    QPointF board_sep   = config.readEntry("board-sep", QPointF(1.0, 1.0));
    QPointF board_shift = config.readEntry("board-shift", QPointF(1.0, 1.0));
    // Offset for second player
    if (position == 1)
    {
        board_pos  = config.readEntry("board-pos2", QPointF(1.0, 1.0));
    }

    // Two height level of cards
    for (int h = 0; h < 2; h++)
    {
        // Two rows of cards
        for (int y = 0; y < 2; y++)
        {
            // Four columns of cards
            for (int x = 0; x < 4; x++)
            {
                // Get playerNumber on game board (0 1 2 3)
                //                                (4 5 6 7)
                int cardPos = x + 4 * y + 8 * h;
                // Get card of player belonging to this playerNumber
                int cardNo  = player->getCard(cardPos);
                // Create sprite with card correct card image
                CardSprite *sprite = mCards[cardNo];
                // Move sprite to correct board playerNumber
                QPointF pos = board_pos + QPointF(x * board_sep.x(), y * board_sep.y());
                // Add shift for stacked cards
                pos += h * board_shift;
                sprite->setZValue(50 - 10 * h);
                sprite->setPosition(deck_pos);
                sprite->show();
                double delay = position + 2 * x + 8 * y + 16 * (1 - h);
                delay *= TIME_DELAY_SHUFFLE; // [ms]
                // Move to the target position. The setPos is started with
                // a little delay and depending on the last argument the
                // backside or frontside is shown after the setPos
                sprite->setShuffleMove(pos, delay, h == 0);
                // Store sprite
            }// next x
        }// next y
    }// next h

    // Check dealing for only one player to avoid double timer events
    if (position == 1)
    {
        QTimer::singleShot(100, this, SLOT(checkShuffle()));
    }
}

// Check whether all cardsprites are idle
void DisplayTwo::checkShuffle()
{
    bool idle = true;
    // Check whether the sprites are idle
    for (int i = 0; i < 32; i++)
    {
        CardSprite *sprite = mCards[i];
        if (!sprite->isIdle())
        {
            idle = false;
            break;
        }
    }

    // If sprites are not idle repeat check otherwise emit 'done' signal
    if (!idle)
    {
        QTimer::singleShot(100, this, SLOT(checkShuffle()));
    }
    else
    {
        emit dealingDone();
    }
}

// Convert a mouse coordinate back to card numbers
void DisplayTwo::convertMousePress(const QPoint &mouse, int &playerNumber, int &cardNumber)
{
    double scale = thememanager()->getScale();
    double x = mouse.x() / scale;
    double y = mouse.y() / scale;

    // Check play area 1
    KConfigGroup config0 = thememanager()->config(QLatin1String("cardarea0"));
    QPointF pos0         = config0.readEntry("pos", QPointF(1.0, 1.0));
    double  width0       = config0.readEntry("width", 1.0);
    double  height0      = config0.readEntry("height", 1.0);

    double x0 = (x - pos0.x()) / width0;
    double y0 = (y - pos0.y()) / height0;

    // Check play area 2
    KConfigGroup config1 = thememanager()->config(QLatin1String("cardarea1"));
    QPointF pos1         = config1.readEntry("pos", QPointF(1.0, 1.0));
    double  width1       = config1.readEntry("width", 1.0);
    double  height1      = config1.readEntry("height", 1.0);

    double x1 = (x - pos1.x()) / width1;
    double y1 = (y - pos1.y()) / height1;

    // Check in area 1
    if (x0 >= 0.0 && x0 < 1.0 && y0 >= 0.0 && y0 < 1.0)
    {
        int dx = int(x0 * 4.0);
        int dy = int(y0 * 2.0);
        playerNumber = 0;
        cardNumber = dx + 4 * dy;
        return;
    }

    // Check in area 2
    if (x1 >= 0.0 && x1 < 1.0 && y1 >= 0.0 && y1 < 1.0)
    {
        int dx = int(x1 * 4.0);
        int dy = int(y1 * 2.0);
        playerNumber = 1;
        cardNumber = dx + 4 * dy;
        return;
    }

    playerNumber = -1;
    return;
}

// Get x (0-3) y(0-1) board coordinates from card number (0-7)
void DisplayTwo::calcXYFromNumber(int cardNumber, int &x, int &y)
{
    x = cardNumber % 4;
    y = cardNumber / 4;
}

// Get a cardsprite given the card value
CardSprite *DisplayTwo::getCardSprite(int cardValue)
{
    CardSprite *sprite = mCards[cardValue];
    if (!sprite)
    {
        qCCritical(LSKAT_LOG) << "Could not find cardsprite for card value" << cardValue
                << "Stored are" << mCards.size() << "sprites";
        return 0;
    }
    return sprite;
}

// Play a frontside card to the play area
void DisplayTwo::play(int cardNumber, int playerNumber, int phase)
{
    KConfigGroup config = thememanager()->config(id());
    QPointF play_pos_1     = config.readEntry("play-pos1", QPointF(1.0, 1.0));
    QPointF play_pos_2     = config.readEntry("play-pos2", QPointF(1.0, 1.0));
    double deal_move_time  = config.readEntry("deal-move-time", 1.0);

    CardSprite *sprite = getCardSprite(cardNumber);
    // Set z coordinate depending on setPos phase, that is latter cards will be
    // put on top
    sprite->setZValue(200 + 5 * phase);
    if (playerNumber == 0)
    {
        sprite->setMove(play_pos_1, deal_move_time);
    }
    else
    {
        sprite->setMove(play_pos_2, deal_move_time);
    }
}

// Turn a backside card to the front
void DisplayTwo::turn(int cardNumber)
{
    CardSprite *sprite = getCardSprite(cardNumber);
    sprite->setTurning(true);
}

// Remove the given card from the display.
void DisplayTwo::remove(int winnerPosition, int cardNumber, int delta)
{
    KConfigGroup config = thememanager()->config(id());
    QPointF stack_pos1  = config.readEntry("stack-pos1", QPointF(1.0, 1.0));
    QPointF stack_pos2  = config.readEntry("stack-pos2", QPointF(1.0, 1.0));
    QPointF stack_shift = config.readEntry("stack-shift", QPointF(1.0, 1.0));
    double remove_time  = config.readEntry("remove-time", 1.0);
    CardSprite *sprite  = getCardSprite(cardNumber);
    // Pile cards on top of each other
    sprite->setZValue(100 + delta);
    QPointF pos;
    if (winnerPosition == 0)
    {
        pos = QPointF(stack_pos1.x() + delta * stack_shift.x(),
                    stack_pos1.y() + delta * stack_shift.y());
    }
    else
    {
        pos = QPointF(stack_pos2.x() + delta * stack_shift.x(),
                    stack_pos2.y() + delta * stack_shift.y());
    }
    sprite->setRemove(pos, remove_time);
}

// Display a text on the game board.
void DisplayTwo::showText(const QString &s)
{
    mText[2]->setText(s);
    mText[2]->show();
}

// Display the score on the game board
void DisplayTwo::showScore(int position, int score)
{
    if (position < 0 || position > 1)
    {
        qCCritical(LSKAT_LOG) << "Wrong position (0,1) for showScore =" << position;
    }
    if (score == 0)
        mText[position]->setText(i18nc("Resulting score of a game with no point", "no point"));
    else
        mText[position]->setText(i18ncp("Resulting score of a game between 1 and 4", "%1 point", "%1 points", score));
    mText[position]->show();
}

// Show the setPos icon for the given player
void DisplayTwo::showMove(int no)
{
    QHashIterator<int, PixmapSprite *> it(mMoveSprites);
    while (it.hasNext())
    {
        it.next();
        PixmapSprite *sprite = it.value();
        sprite->hide();
    }

    if (no >= 0)
    {
        mMoveSprites[no]->show();
    }
}
