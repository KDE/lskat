#ifndef PLAYERSTATUSWIDGET_H
#define PLAYERSTATUSWIDGET_H
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
#include <QSize>

// KDE includes
#include <kdebug.h>

// Local includes
#include "playerstatusform.h"

using namespace Ui;

class Player;

/**
 * The player status widget.
 */
class PlayerStatusWidget : public QWidget
{
  Q_OBJECT

  public:
    /** Constructor for the widget.
     *  @param player The parent player
     *  @param parent The parent window
     */
    PlayerStatusWidget(Player* player, QWidget* parent);

    /**
     * Reimplement Qt Size hint for widgets.
     * @return The size hint.
     */
    virtual QSize sizeHint() const; 

  public slots:  
    /** Set the player name into the widget.
     * @param name The player's name
     */
    void setName(QString name);

    /** Set the player points into the widget.
     * @param points The player's points
     */
    void setPoints(int points);

    /** Set the player score into the widget.
     * @param score The player's score
     */
    void setScore(int score);

    /** Set the player won games and overall games 
     * @param wonGames The player's won games
     * @param overallGames The player's overall games
     */
    void setGames(int wonGames, int overallGames);

    /** Set the trump icon into the widget.
     * @param icon The trump icon
     */
    void setTrumpIcon(QPixmap* icon);

    /** Set the input icon into the widget.
     * @param icon The input icon
     */
    void setInputIcon(QPixmap* icon);


  private:
    // The UI widget
    PlayerStatusForm ui;
};

#endif
