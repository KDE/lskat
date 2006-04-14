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

// KDE includes
#include <kdebug.h>

// Local includes
#include "abstractengine.h"


// Constructor for the game document/engine
AbstractEngine::AbstractEngine(QWidget* parent)
               : QObject(parent)
{
  mGameStatus = Stopped;
  mPlayers.clear();
}


// Add a player to the game 
void AbstractEngine::addPlayer(int no, Player* player)
{
  mPlayers[no] = player;
}

// Retrieve the player of the given number.
Player* AbstractEngine::player(int no)
{
  return mPlayers[no];
}





