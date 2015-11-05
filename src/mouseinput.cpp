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

#include "mouseinput.h"

// Qt includes
#include <QPixmap>

// KDE includes
#include <kdebug.h>


// Constructor for the input
MouseInput::MouseInput(QObject* parent)
    : AbstractInput(parent)
{
}


// Mouse press received
void MouseInput::mousePress(const QPoint &point)
{
  int playerNumber;
  int cardNumber;

  // Do only process input if it is our turn
  if (!mInputAllowed) return;

  emit signalConvertMousePress(point, playerNumber, cardNumber);
  // Check whether the move is for the right player
  if (playerNumber == mId)
  {
    //kDebug() << "MouseInput:: Convert to" << playerNumber << "," << cardNumber;
    emit signalPlayerInput(mId, playerNumber, cardNumber);
  }
}


#include "mouseinput.moc"
