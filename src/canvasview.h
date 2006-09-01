#ifndef CANVAS_VIEW_H
#define CANVAS_VIEW_H
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
#include <QWidget>
#include <QPixmap>
#include <Q3Canvas>
#include <QSize>
#include <QPoint>
#include <QResizeEvent>


/**
 * The view object which shows the graphics in a
 * canvas view.
 */
class CanvasView : public Q3CanvasView
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param size The canvas fixed size
     *  @param advancePeriod The canvas advance period
     *  @param parent The parent window
     */
    CanvasView(QSize size, int advancePeriod, QWidget* parent = 0);

    /** 
     * Destructor for the canvas view 
     */
    ~CanvasView();

     void mousePressEvent(QMouseEvent *ev);


  public slots:  

  signals:
    void signalLeftMousePress(QPoint point);

  protected:
    /**
     * Will be called by the Qt CanvasView when its contents
     * are resized. We adapt the canvas then.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);

};

#endif
