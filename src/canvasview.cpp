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

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "canvasview.h"



// Constructor for the view
CanvasView::CanvasView(QSize size, int advancePeriod, QWidget* parent)
          : Q3CanvasView(parent)
{
  // We do not need scrolling so switch it off
  setResizePolicy(Q3ScrollView::Manual);
  setVScrollBarMode(AlwaysOff);
  setHScrollBarMode(AlwaysOff);

  // Create a new canvas for this view
  Q3Canvas *canvas=new Q3Canvas(this);

  // Remove flicker by double buffering
  canvas->setDoubleBuffering(true);
  // Update/advance every 25ms
  canvas->setAdvancePeriod(advancePeriod);
  // assign the canvas to the view
  setCanvas(canvas);

  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  setFixedSize(size);
  canvas->resize(this->width(), this->height()); 
}


// Slot called by the framework when the window is
// resized.
void CanvasView::resizeEvent (QResizeEvent* e)
{
  // Adapt the canvas size to the window size
  if (canvas()) canvas()->resize(e->size().width(),
                                 e->size().height());
}

// Standard destructor
CanvasView::~CanvasView()
{
}


// mouse click event
void CanvasView::mousePressEvent(QMouseEvent *ev)
{
  if (ev->button() != Qt::LeftButton) return ;

  QPoint point = ev->pos();
  emit signalLeftMousePress(point);
}
