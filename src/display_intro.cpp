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

// General includes
#include <math.h>

// Qt includes
#include <QSize>
#include <QPoint>

// KDE includes
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "display_intro.h"
#include "cardsprite.h"
#include "textsprite.h"

// Intro geometry
// Where does the display start
#define BOARD_OFFSET_X  40
#define BOARD_OFFSET_Y  50

#define WAIT_CNT       100  /* Wait this * timer ms */
#define WAIT_CNT2       70  /* Wait this * timer ms */

// Constructor for the engine
DisplayIntro::DisplayIntro(QString grafixDir, Deck* deck, Q3Canvas* canvas, QObject* parent)
    : AbstractDisplay(grafixDir, deck, canvas, parent)
{
  // Choose a background color
  canvas->setBackgroundColor(QColor(0,0,128));

  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(loop()));
  mTimer->stop();
}

// Start the intro.
void DisplayIntro::start()
{
  mAnimCnt = 0;
  mState   = Putting;
  mTimer->start(50);

  QString s1 = i18n("Lieutenant Skat");
  QString s2 = i18n("for");
  QString s3 = i18n("K D E");

  // Text sprite
  TextSprite* text = 0;

  // Text font
  QFont font;
  font.setPixelSize(36);
  font.setBold(true);

  // Canvas size
  QSize cSize   = mCanvas->size();   // Canvas size
  int x = cSize.width()/2;
  int y = cSize.height()/2;

  // Display text sprites
  // Text: Lieutenant Skat (bright)
  text = new TextSprite(mCanvas);
  text->setText(s1);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(255, 128, 0));
  text->move(x, y-100);
  text->setZ(200);
  text->show();
  mSprites.append(text);

  // Text: Lieutenant Skat (dark)
  text = new TextSprite(mCanvas);
  text->setText(s1);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(10, 10, 0));
  text->move(x-3, y+3-100);
  text->setZ(190);
  text->show();
  mSprites.append(text);

  // Text: for (bright)
  text = new TextSprite(mCanvas);
  text->setText(s2);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(255, 128, 0));
  text->move(x, y);
  text->setZ(200);
  text->show();
  mSprites.append(text);

  // Text: for (dark)
  text = new TextSprite(mCanvas);
  text->setText(s2);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(15, 15, 0));
  text->move(x-3, y+3);
  text->setZ(190);
  text->show();
  mSprites.append(text);

  // Text: KDE (bright)
  text = new TextSprite(mCanvas);
  text->setText(s3);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(255, 128, 0));
  text->move(x, y+100);
  text->setZ(200);
  text->show();
  mSprites.append(text);

  // Text: KDE (dark)
  text = new TextSprite(mCanvas);
  text->setText(s3);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(15, 15, 0));
  text->move(x-3, y+3+100);
  text->setZ(190);
  text->show();
  mSprites.append(text);

  // Stop all card sprites
  for (int i=0; i<mCards.size(); i++)
  {
    mCards[i]->stop();
  }
}

void DisplayIntro::loop()
{
  int no = mCards.size();
  // Catch no card error
  if (no<1) return;

  // Sizes
  QSize size    = mDeck->cardSize(); // Card size
  QSize cSize   = mCanvas->size();   // Canvas size
  QPoint offset = QPoint(BOARD_OFFSET_X, BOARD_OFFSET_Y);

  // Display a card
  if (mAnimCnt < no && mState == Putting)
  {
    double x= cSize.width()/3*sin(mAnimCnt*M_PI/(double)(no-1));

    CardSprite* sprite = mCards[mAnimCnt];

    QPoint point;
    if (mAnimCnt %2 == 0) 
    {
      point  = offset;
      point += QPoint(int(x), mAnimCnt*10);
    }
    else
    {
      point  = QPoint(cSize.width()-BOARD_OFFSET_X-size.width(), BOARD_OFFSET_Y);
      point += QPoint(int(-x), mAnimCnt*10);
    }
    sprite->setBackside();
    sprite->move(point.x(), point.y());
    sprite->setZ(50+mAnimCnt);
    sprite->show();
    mAnimCnt++;
  }
  else if (mState == Putting)
  {
    mState   = Turning;
    mAnimCnt = 0;
  }
  else if (mAnimCnt < no && mState == Turning)
  {
    CardSprite* sprite = mCards[mAnimCnt];
    sprite->setTurning(true);
    mAnimCnt++;
  }
  else if (mState == Turning)
  {
    mState   = Waiting;
    mAnimCnt = 0;
  }
  else if (mAnimCnt < WAIT_CNT && mState == Waiting)
  {
    mAnimCnt++;
  }
  else if (mState == Waiting)
  {
    mState   = Clearing;
    mAnimCnt = 0;
  }
  else if (mAnimCnt == 0 && mState == Clearing)
  {
    for (int i=0; i<no; i++)
    {
      CardSprite* sprite = mCards[i];
      sprite->setMove(cSize.width()/2-size.width()/2, cSize.height()/2-size.height()/2);
    }
    mAnimCnt++;
  }
  else if (mAnimCnt < 30 && mState == Clearing)
  {
    mAnimCnt++;
  }
  else if (mState == Clearing)
  {
    for (int i=0; i<no; i++)
    {
      double r = 0.7*sqrt(cSize.width()*cSize.width() + cSize.height()*cSize.height());
      double x = r*cos(double(i)/double(no-1)*M_PI*2.0) + cSize.width()/2.0;
      double y = r*sin(double(i)/double(no-1)*M_PI*2.0) + cSize.height()/2.0;
      CardSprite* sprite = mCards[i];
      sprite->setMove(int(x), int(y));
    }
    mState = Waiting2;
    mAnimCnt = 0;
  }
  else if (mAnimCnt < WAIT_CNT && mState == Waiting2)
  {
    mAnimCnt++;
  }
  else if (mState == Waiting2)
  {
    for (int i=0; i<no; i++)
    {
      CardSprite* sprite = mCards[i];
      sprite->stop();
    }

    mState   = Putting;
    mAnimCnt = 0;
  }

}



#include "display_intro.moc"
