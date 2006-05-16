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

// KDE includes
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "playerstatuswidget.h"
#include "player.h"

// Constructor for the view
PlayerStatusWidget::PlayerStatusWidget(Player* player, QWidget* parent)
                  : QWidget(parent)
{
  ui.setupUi(this);

  // Connect status widget to player
  connect(player, SIGNAL(signalUpdatePoints(int)),
          this, SLOT(setPoints(int)));
  connect(player, SIGNAL(signalUpdateName(QString)),
          this, SLOT(setName(QString)));
  connect(player, SIGNAL(signalUpdateScore(int)),
          this, SLOT(setScore(int)));
  connect(player, SIGNAL(signalUpdateGames(int, int)),
          this, SLOT(setGames(int, int)));
  connect(player, SIGNAL(signalUpdateInput(QPixmap*)),
          this, SLOT(setInputIcon(QPixmap*)));
  connect(player, SIGNAL(signalUpdateTrump(QPixmap*)),
          this, SLOT(setTrumpIcon(QPixmap*)));
  player->refresh(); // Emit signals
}


// Set the player name into the widget.
void PlayerStatusWidget::setName(QString name)
{
  ui.labelName->setText(name);
}

// Set the player points into the widget.
void PlayerStatusWidget::setPoints(int points)
{
  QString s = QString("%1").arg(points);
  ui.labelPoints->setText(s);
}

// Set the player score into the widget.
void PlayerStatusWidget::setScore(int score)
{
  QString s = QString("%1").arg(score);
  ui.labelScore->setText(s);
}

// Set the player won games and overall games 
void PlayerStatusWidget::setGames(int wonGames, int overallGames)
{
  QString s = QString("%1 / %2").arg(wonGames).arg(overallGames);
  ui.labelGames->setText(s);
}

// Set the trump icon into the widget.
void PlayerStatusWidget::setTrumpIcon(QPixmap* icon)
{
  ui.imageTrump->setPixmap(*icon);
}

// Set the input icon into the widget.
void PlayerStatusWidget::setInputIcon(QPixmap* icon)
{
  ui.imageInput->setPixmap(*icon);
}

// Qt widget size hint
QSize PlayerStatusWidget::sizeHint() const
{
  QSize size =  ui.gridLayout->sizeHint();
return size;
  return QSize(250,80); // TODO: No fixed size
}

#include "playerstatuswidget.moc"
