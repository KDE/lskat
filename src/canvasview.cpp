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

// Qt includes
#include <QPoint>
#include <QFont>
#include <QTimer>

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "canvasview.h"
#include "thememanager.h"


// Constructor for the view
CanvasView::CanvasView(QSize size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent)
          : QGraphicsView(scene, parent)
{
  // Store attributes    
  mTheme         = theme;

  // We do not need scrolling so switch it off
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setCacheMode(QGraphicsView::CacheBackground);


  // Update/advance every 25ms
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateAndAdvance()));
  timer->start(advancePeriod);
  
  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  resize(size);
  scene->setSceneRect(0, 0, this->width(), this->height()); 
  adjustSize();

  setInteractive(true);

  // Scale theme
  mTheme->rescale(this->width());
}

// Advance and update canvas
void CanvasView::updateAndAdvance()
{
  scene()->advance();
  //NOTE regarding QGV porting
  //QGV will handle dirty rects for us
  //Calling update will just dirty the view and cause a full redraw, killing performance
  //scene()->update();
}


// Slot called by the framework when the window is
// resized.
void CanvasView::resizeEvent(QResizeEvent* e)
{
  // Adapt the canvas size to the window size
  if (scene())
  {
    scene()->setSceneRect(0,0, e->size().width(), e->size().height());
  }


  QSizeF size = QSizeF(e->size());
  // Rescale on minimum fitting aspect ratio either width or height limiting
  double aspect = size.width() / size.height();
  if (aspect > mTheme->aspectRatio()) mTheme->rescale(int(e->size().height()*mTheme->aspectRatio()));
  else mTheme->rescale(int(e->size().width()));
}

// Our subclassed (temporary) QGraphicsView paintEvent, see header file
void CanvasView::paintEvent(QPaintEvent* event)
{
    QPaintEvent* newEvent = new QPaintEvent(event->region().boundingRect());
    QGraphicsView::paintEvent(newEvent);
    delete newEvent;
}


// mouse click event
void CanvasView::mousePressEvent(QMouseEvent *ev)
{
  if (ev->button() != Qt::LeftButton) return ;

  QPointF point = ev->pos();
  emit signalLeftMousePress(point.toPoint());
}



#include "canvasview.moc"
