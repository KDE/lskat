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
#include "playerstatuswidget.h"



// Constructor for the view
CanvasView::CanvasView(QSize size, int advancePeriod, QGraphicsScene* scene, QWidget* parent)
          : QGraphicsView(scene, parent)
{
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
  //setFixedSize(size);
  setMinimumSize(size);

  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  scene->setSceneRect(0, 0, this->width(), this->height()); 
  adjustSize();

  setInteractive(true);
  mPlayerWidgets.clear();
}

// Advance and update canvas
void CanvasView::updateAndAdvance()
{
  scene()->advance();
  scene()->update();
}


// Slot called by the framework when the window is
// resized.
void CanvasView::resizeEvent (QResizeEvent* e)
{
  //kDebug() << "++++ CanvasView::resizeEvent "<<e->size().width()<<" , "<< e->size().height() <<endl;
  // Adapt the canvas size to the window size
  if (scene())
  {
    scene()->setSceneRect(0,0, e->size().width(), e->size().height());

    // Realign score widget
    QHashIterator<int,PlayerStatusWidget*> it(mPlayerWidgets);
    while(it.hasNext())
    {
      it.next();
      moveStatusWidget(it.value(), it.key());
    }
  }
}


// mouse click event
void CanvasView::mousePressEvent(QMouseEvent *ev)
{
  if (ev->button() != Qt::LeftButton) return ;

  QPointF point = ev->pos();
  emit signalLeftMousePress(point.toPoint());
}

void CanvasView::setStatusWidget(int pos, PlayerStatusWidget* widget)
{
  // Remove old widget
  if (mPlayerWidgets.contains(pos))
  {
    PlayerStatusWidget* old = mPlayerWidgets[pos];
    delete old;
  }
  mPlayerWidgets[pos] = widget;
  widget->setParent(this);
  // Add spacing
  moveStatusWidget(widget, pos);
  widget->show();
  update();
}


// Position score widgets at window border
void CanvasView::moveStatusWidget(QWidget* widget, int pos)
{
  widget->move(width()-widget->width() -10 , pos*widget->height() + 10 );
}


// Retrive the status widget of a player 
PlayerStatusWidget* CanvasView::statusWidget(int pos)
{
  if (!mPlayerWidgets.contains(pos))
  {
    kFatal() << "Player status widget " << pos << "does not exists" << endl;
    return 0;
  }
  return mPlayerWidgets[pos];
}


#include "canvasview.moc"
