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

#include "gameview.h"

// General includes
#include <math.h>

// Qt includes
#include <QPoint>
#include <QFont>
#include <QTimer>
#include <QTime>

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "thememanager.h"
#include "lskatglobal.h"


// Constructor for the view
GameView::GameView(const QSize &size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent)
        : QGraphicsView(scene, parent)
{
  // Store attributes    
  mTheme  = theme;

  // We do not need scrolling so switch it off
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Frame off
  setFrameStyle(QFrame::NoFrame);

  // Cache on
  setCacheMode(QGraphicsView::CacheBackground);
  setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  setOptimizationFlags(QGraphicsView::DontClipPainter |
                       QGraphicsView::DontSavePainterState |
                       QGraphicsView::DontAdjustForAntialiasing );

  // Debug                      
  mDisplayUpdateTime = 0;
  mFrameSprite = new QGraphicsTextItem(0);
  scene->addItem(mFrameSprite);
  mFrameSprite->setPos(QPointF(0.0, 0.0));
  mFrameSprite->setZValue(1000.0);
  if (global_debug > 0) mFrameSprite->show();
  else mFrameSprite->hide();


  // Update/advance in [ms]
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateAndAdvance()));
  timer->start(advancePeriod);
  
  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  resize(size);
  scene->setSceneRect(0, 0, this->width(), this->height()); 
  adjustSize();

  // Enable mouse
  setInteractive(true);

  // Scale theme
  //mTheme->rescale(this->width());
  mThemeQueue.clear();
  mThemeOffset.clear();
  mTimeStart.start();
}

GameView::~GameView()
{
  delete mFrameSprite;
}


// Advance and update canvas
void GameView::updateAndAdvance()
{
  static int elapsed = -1;
  static QTime timer;
  if (elapsed < 0 ) 
  {
    timer.start();
    elapsed = 0;
  }
  else
  {
    elapsed = timer.elapsed();
    timer.start();
    mDisplayUpdateTime = elapsed;
  }

  scene()->advance();
  //NOTE regarding QGV porting
  //QGV will handle dirty rects for us
  //Calling update will just dirty the view and cause a full redraw, killing performance
  //scene()->update();
}


// Slot called by the framework when the window is
// resized.
void GameView::resizeEvent(QResizeEvent* e)
{
  QTime t;
  t.start();
  if (global_debug > 2) kDebug() <<"RESIZE EVENT" << e->size() << "oldSize="<< e->oldSize() <<" at" << t.msecsTo(mTimeStart);
  double diffW = double(e->oldSize().width()-e->size().width());
  double diffH = double(e->oldSize().height()-e->size().height());
  double delta = fabs(diffW) + fabs(diffH); 

  // Adapt the canvas size to the window size
  if (scene())
  {
    scene()->setSceneRect(0,0, e->size().width(), e->size().height());
  }


  QSizeF size = QSizeF(e->size());
  // Rescale on minimum fitting aspect ratio either width or height limiting
  double aspect = size.width() / size.height();
  QPoint offset;
  double width = 0.0;
  
  // Scale width:
  // Ideal size would be: 'width'*'height'
  // Offset in width is (e->size().width()-width)/2, offset in height is zero
  if (aspect > mTheme->aspectRatio())
  {
     width  = e->size().height()*mTheme->aspectRatio();
     offset = QPoint(int((e->size().width()-width)/2.0), 0);
  }
  // Scale height:
  // 'height' = width/mTheme->aspectRatio()
  // Ideal size would be: 'width'*'height': 
  // Offset in height is (e->size().height()-width/mTheme->aspectRatio())/2, offset in width is zero
  else
  {
    width = e->size().width();
    offset = QPoint(0, int((e->size().height()-width/mTheme->aspectRatio())/2.0));
  }

  // Pixel rescale
  double oldScale = mTheme->getScale();
  resetTransform();
  if (width > oldScale) 
  {
    scale(double(width/oldScale), double(width/oldScale));
  }
  mThemeQueue.prepend(int(width));
  mThemeOffset.prepend(offset);
  if (global_debug > 2) kDebug() << "Quequed resize, aspect=" << aspect << "theme aspect="<< mTheme->aspectRatio();

  long queueDelay = 0;
  if (delta < 15) queueDelay = 750;
  else if (delta < 35) queueDelay = 500;

  QTimer::singleShot(queueDelay, this, SLOT(rescaleTheme()) );
}


// Rescale the theme (update theme SVG graphics) from the theme list
void GameView::rescaleTheme()
{
  if (mThemeQueue.size() == 0)
  {
    if (global_debug > 2) kDebug() << "***************** Swallowing rescale event ***********************";
    return;
  }

  QTime t;
  t.start();

  if (global_debug > 2) kDebug() << "Theme queue rescale start at"  << t.msecsTo(mTimeStart);
  resetTransform();
  int width = mThemeQueue.first();
  mInputOffset = mThemeOffset.first();
  if (global_debug > 2) kDebug() << "Theme queue size=" << mThemeQueue.size() << "Rescale width to" << width;
  
  mThemeQueue.clear();
  mThemeOffset.clear();
  mTheme->rescale(width, mInputOffset);

   if (global_debug > 2) kDebug() << "Time elapsed: "<< t.elapsed() << "ms";
}


// Our subclassed (temporary) QGraphicsView paintEvent, see header file
void GameView::paintEvent(QPaintEvent* event)
{
    QPaintEvent* newEvent = new QPaintEvent(event->region().boundingRect());
    QGraphicsView::paintEvent(newEvent);
    delete newEvent;
}


// Mouse click event
void GameView::mouseReleaseEvent(QMouseEvent *ev)
{
  if (ev->button() != Qt::LeftButton) return ;

  QPointF point = ev->pos()-mInputOffset;
  emit signalLeftMousePress(point.toPoint());
}

void GameView::drawItems(QPainter* painter, int numItems, QGraphicsItem* items[], const QStyleOptionGraphicsItem options[])
{
  QTime time;
  time.start();
  QGraphicsView::drawItems(painter, numItems, items, options);

  // Time display
  int elapsed = time.elapsed();
  mDrawTimes.append(elapsed);
  if (mDrawTimes.size() > 50) mDrawTimes.removeFirst();
  double avg = 0.0;
  for (int i=0; i<mDrawTimes.size(); i++) avg += mDrawTimes[i];
  avg /= mDrawTimes.size();


  if (global_debug > 0)
     mFrameSprite->setPlainText(QString::fromLatin1( "Draw: %1 ms  Average %2 ms  Update: %3 ms").arg(elapsed).arg(int(avg)).arg(mDisplayUpdateTime));

}


