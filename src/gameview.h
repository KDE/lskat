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

#ifndef LSKAT_GAMEVIEW_H
#define LSKAT_GAMEVIEW_H

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
#include <QGraphicsTextItem>

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
    GameView(const QSize &size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent = 0);

    /** Destructor.
    */
    ~GameView();

 protected:
    /** React to mouse clicks.
     *  @param event The mouse event
     */
    void mouseReleaseEvent(QMouseEvent *event);

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

    /** Overwritten Qt function.
    */
    virtual void drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[]);


  private:
    // Theme manager
    ThemeManager* mTheme;
    // Theme queue
    QList<int> mThemeQueue;
     // Theme offset queue
    QList<QPoint> mThemeOffset;
    //	Active input offset
    QPoint mInputOffset;
    // Theme queue delay time [ms]
    QTime mTimeStart;
    // Debug frame rate sprite
    QGraphicsTextItem* mFrameSprite;
    // Time between updates
    int mDisplayUpdateTime;
    // Average update times
    QList<int> mDrawTimes;


};

#endif // LSKAT_GAMEVIEW_H
