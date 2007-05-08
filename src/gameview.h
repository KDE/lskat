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
#include <QList>
#include <QTime>
#include <QResizeEvent>

// Forward declaration
class ThemeManager;

/**
 * The view object which shows the graphics in a
 * Qt graphics view.
 */
class GameView : public QGraphicsView
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param size The canvas fixed size
     *  @param advancePeriod The canvas advance period
     *  @param scene The graphics scene
     *  @param theme The theme manager
     *  @param parent The parent window
     */
    GameView(QSize size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent = 0);

 protected:
    /** React to mouse clicks.
     *  @param event The mouse event
     */
    void mousePressEvent(QMouseEvent *event);

  public slots:  
    /** The update and advance for the canvas. 
     *  This is called by a timer at regular intervals.
     */
    void updateAndAdvance();

    /** Rescale the theme (update theme SVG graphics).
      */
    void rescaleTheme();  

  signals:
    /** Emit the signal on mouse clicks.
      * @param point The mouse coordinate.
      */
    void signalLeftMousePress(QPoint point);

  protected:
    /** Will be called by the Qt View when its contents
     * are resized. We adapt the canvas then.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);

    /** Paint function for the widget, temporary fix while we wait for QGV 4.3
      * @param event The paint event
      */
    void paintEvent(QPaintEvent* event);


  private:
    // Theme manager
    ThemeManager* mTheme;
    // Theme Queue
    QList<int> mThemeQueue;
    // Theme queue delay time [ms]
    QTime mTimeStart;
};

#endif
