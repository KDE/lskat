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

#define WAIT_CNT       100  /* Wait this * timer ms */

// Constructor for the engine
DisplayIntro::DisplayIntro(Deck* deck, QGraphicsScene* scene, ThemeManager* theme, int advancePeriod, QGraphicsView* parent)
    : Themable("display_intro",theme), AbstractDisplay(deck, scene, theme, advancePeriod, parent)
{
  // Choose a background color
  mCanvas->setBackgroundBrush(QColor(0,0,128));

  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(loop()));
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
  QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, mCanvas->sceneRect().size().toSize());
  mCanvas->setBackgroundBrush(pixmap);
  mView->update();
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
  TextSprite* text1a = new TextSprite(s1, "name-front", mTheme, mCanvas);
  mSprites.append(text1a);
  text1a->show();

  TextSprite* text1b = new TextSprite(s1, "name-back", mTheme, mCanvas);
  mSprites.append(text1b);
  text1b->show();

  TextSprite* text2a = new TextSprite(s2, "for-front", mTheme, mCanvas);
  mSprites.append(text2a);
  text2a->show();

  TextSprite* text2b = new TextSprite(s2, "for-back", mTheme, mCanvas);
  mSprites.append(text2b);
  text2b->show();

  TextSprite* text3a = new TextSprite(s3, "kde-front", mTheme, mCanvas);
  mSprites.append(text3a);
  text3a->show();

  TextSprite* text3b = new TextSprite(s3, "kde-back", mTheme, mCanvas);
  mSprites.append(text3b);
  text3b->show();

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

  KConfigGroup cardconfig = thememanager()->config("card");
  double card_width       = cardconfig.readEntry("width", 1.0);
  KConfigGroup config     = thememanager()->config(id());
  QPointF start_shift     = config.readEntry("start-shift", QPointF(1.0,1.0));
  QPointF start_pos       = config.readEntry("start-pos", QPointF(1.0,1.0));
  double time_clear_in    = config.readEntry("time-clear-in", 1.0);
  double time_clear_out   = config.readEntry("time-clear-out", 1.0);
  double aspectRatio      = thememanager()->aspectRatio();

  // Sizes
  // Display a card
  if (mAnimCnt < no && mState == Putting)
  {
    double factor = double(mAnimCnt)/double(no-1);
    double fsin = sin(factor*M_PI);

    CardSprite* sprite = mCards[mAnimCnt];

    QPointF pos;
    if (mAnimCnt %2 == 0) 
    {
      pos  = QPointF(start_pos.x(), start_pos.y());
      pos += QPointF(start_shift.x() * fsin, start_shift.y() * factor);
    }
    else
    {
      pos  = QPointF(1.0-start_pos.x()-card_width, start_pos.y());
      pos += QPointF(-start_shift.x() * fsin, start_shift.y() * factor);
    }
    sprite->setBackside();
    sprite->setPosition(pos);
    sprite->setZValue(50+mAnimCnt);
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
      sprite->setMove(QPointF((1.0-card_width)/2.0, (1.0/aspectRatio-card_width)/2.0), time_clear_in);
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
      double r = 1.0;
      double x = r*cos(double(i)/double(no-1)*M_PI*2.0) + 0.5;
      double y = r*sin(double(i)/double(no-1)*M_PI*2.0) + 0.5;
      CardSprite* sprite = mCards[i];
      sprite->setMove(QPointF(x,y/aspectRatio), time_clear_out);
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
