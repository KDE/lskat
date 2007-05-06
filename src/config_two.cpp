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

#include "config_two.h"

// Qt includes

// KDE includes
#include <kdebug.h>
#include <klocale.h>


// Constructor for the configuration
ConfigTwo::ConfigTwo(QObject* parent)
         : QObject(parent)
{
  // Create Players
  mPlayers.clear();
  mPlayers[0] = new Player(0, this);
  mPlayers[1] = new Player(1, this);
}


// Resets the data
void ConfigTwo::reset()
{
  mPlayers[0]->setName(i18nc("Default player name", "Alice")); 
  mPlayers[1]->setName(i18nc("Default player name","Bob")); 

  // Default input types (must be after GUI and players)
  setInputType(0, TypeMouseInput);
  setInputType(1, TypeAiInput);
}


// Destructor
ConfigTwo::~ConfigTwo()
{
  QHashIterator<int,Player*> it(mPlayers);
  while(it.hasNext())
  {
    it.next();
    Player* player = it.value();
    delete player;
  }
  mPlayers.clear();
}


// Save properties
void ConfigTwo::save(KConfig *cfg)
{
  KConfigGroup group = cfg->group("LSkatData");
  group.writeEntry("input0", (int)mInputTypes[0]);
  group.writeEntry("input1", (int)mInputTypes[1]);

  // Save player
  QHashIterator<int,Player*> it = playerIterator();
  while(it.hasNext())
  {
    it.next();
    Player* player = it.value();
    int no = it.key();
    KConfigGroup playercfg = cfg->group(QString("LSkat_Player%1").arg(no));
    player->save(playercfg);
  }
}


// Load properties
void ConfigTwo::load(KConfig* cfg)
{
  reset();
  KConfigGroup group = cfg->group("LSkatData");
  int num;
  num = group.readEntry("input0", (int)mInputTypes[0]);
  setInputType(0, (InputDeviceType)num);
  num = group.readEntry("input1", (int)mInputTypes[1]);
  setInputType(1, (InputDeviceType)num);

  // Load player
  QHashIterator<int,Player*> it = playerIterator();
  while(it.hasNext())
  {
    it.next();
    Player* player = it.value();
    int no = it.key();
    KConfigGroup playercfg = cfg->group(QString("LSkat_Player%1").arg(no));
    player->load(playercfg);
  }
}


// Retrieve a player.
Player* ConfigTwo::player(int no)
{
  if (!mPlayers.contains(no)) return 0;
  return mPlayers[no];
}


// Retrieve player as iterator
QHashIterator<int,Player*> ConfigTwo::playerIterator()
{
  QHashIterator<int,Player*> it(mPlayers);
  return it;
}


// Retrieve input type of given player
InputDeviceType ConfigTwo::inputType(int no)
{
  return mInputTypes[no];
}


// Set the input type for a given players
void ConfigTwo::setInputType(int no, InputDeviceType type)
{
  mInputTypes[no] = type;
  emit signalInputType(no, type);
}


#include "config_two.moc"
