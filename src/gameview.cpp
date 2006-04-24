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
#include <QLabel>
#include <QPalette>

// KDE includes
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "gameview.h"
#include "canvasview.h"
#include "playerstatuswidget.h"

using namespace Ui;

// Constructor for the view
GameView::GameView(QSize size, int advancePeriod, QWidget* parent)
        : QWidget(parent)
{
  // Make a grid layout
  mGrid = new QGridLayout(this);

  // Create and place the canvas view
  mCanvasView = new CanvasView(size, advancePeriod, this);
  mGrid->addWidget(mCanvasView, 0, 0, 3, 3, Qt::AlignLeft|Qt::AlignTop);

  mPlayerWidgets.clear();

  QPalette palette;
  palette.setColor(QPalette::Active, QPalette::Background, QColor(0,0,128));
  palette.setColor(QPalette::Inactive, QPalette::Background, QColor(0,0,128));
  palette.setColor(QPalette::Disabled, QPalette::Background, QColor(0,0,128));
  this->setPalette(palette);

  updateGeometry();
  adjustSize();
}

// Destructor
GameView::~GameView()
{
  // Widgets are destroyed by Qt
}

void GameView::setStatusWidget(int pos, PlayerStatusWidget* widget)
{
  // Remove old widget
  if (mPlayerWidgets.contains(pos))
  {
    PlayerStatusWidget* old = mPlayerWidgets[pos];
    mGrid->removeWidget(old);
    delete old;
  }
  mGrid->addWidget(widget, pos, 3 );
  mPlayerWidgets[pos] = widget;
  updateGeometry();
  adjustSize();
}


// Retrive the status widget of a player 
PlayerStatusWidget* GameView::statusWidget(int pos)
{
  if (!mPlayerWidgets.contains(pos))
  {
    kFatal() << "Player status widget " << pos << "does not exists" << endl;
    return 0;
  }
  return mPlayerWidgets[pos];
}

