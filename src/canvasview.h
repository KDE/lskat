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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QSize>
#include <QPoint>
#include <QHash>
#include <QResizeEvent>

// Forward declaration
class PlayerStatusWidget;

/**
 * The view object which shows the graphics in a
 * canvas view.
 */
class CanvasView : public QGraphicsView
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param size The canvas fixed size
     *  @param advancePeriod The canvas advance period
     *  @param scene The graphics scene
     *  @param parent The parent window
     */
    CanvasView(QSize size, int advancePeriod, QGraphicsScene* scene, QWidget* parent = 0);

    /** Retrive the status widget of a player 
     *  @param pos The widget position number (0,1,...)
     *  @return The widget.
     */
    PlayerStatusWidget* statusWidget(int pos);

    /** Store and display a new status widget at the given position.
     *  Old widgets on this position are removed and destroyed.
     *  @param pos The position on the screen (0,1,2)
     *  @param widget The new widget
     */
    void setStatusWidget(int pos, PlayerStatusWidget* widget);

 protected:
    /** React to mouse clicks
     *  @param ev The mouse event
     */
    void mousePressEvent(QMouseEvent *event);

    /** Position score widgets at window border
     *  @param widget The Player status widget
     *  @param pos    The positon of the windwo 0,1,2
     */
    void moveStatusWidget(QWidget* widget, int pos);


  public slots:  
    /** The update and advance for the canvas. 
     *  This is called by a timer at regular intervals.
     */
    void updateAndAdvance();

  signals:
    void signalLeftMousePress(QPoint point);

  protected:
    /**
     * Will be called by the Qt CanvasView when its contents
     * are resized. We adapt the canvas then.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);

  private:
    // The status widgets
    QHash<int,PlayerStatusWidget*> mPlayerWidgets;
};

#endif
