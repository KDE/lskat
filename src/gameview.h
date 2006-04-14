#ifndef GAMEVIEW_H
#define GAMEVIEW_H
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
#include <QPushButton>
#include <QSize>
#include <QHash>
#include <QGridLayout>

// KDE includes
#include <kdebug.h>

// Local includes
#include "canvasview.h"

// Forward declaration
class Deck;
class Q3Canvas;
class PlayerStatusWidget;


/**
 * The view object for the game.
 * This class servers as a container for the actual
 * canvas view and the control buttons.
 */
class GameView : public QWidget
{
  Q_OBJECT

  public:
    /** Constructor for the main view.
     *  @param size The canvas view's fixed size
     *  @param advancePeriod The canvas advance period
     *  @param parent The parent window
     */
    GameView(QSize size, int advancePeriod, QWidget* parent = 0);
    
    /** Destructor
    */
    virtual ~GameView();

    /** Retrive the canvas view.
      * @return The canvas view.
      */
    CanvasView* canvasView() {return mCanvasView;}

    /** Retrive the canvas.
      * @return The canvas.
      */
    Q3Canvas* canvas() {return mCanvasView->canvas();}

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

  public slots:

  private:
    // The canvas view
    CanvasView* mCanvasView;
    // The status widgets
    QHash<int,PlayerStatusWidget*> mPlayerWidgets;
    // The layout grid
    QGridLayout* mGrid;
};

#endif
