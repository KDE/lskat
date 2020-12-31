/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config_two.h"

// Qt includes

// KDE includes
#include <KConfigGroup>
#include <KLocalizedString>

// Constructor for the configuration
ConfigTwo::ConfigTwo(QObject *parent)
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
    mPlayers[1]->setName(i18nc("Default player name", "Bob"));

    // Default input types (must be after GUI and players)
    setInputType(0, TypeMouseInput);
    setInputType(1, TypeAiInput);
}

// Destructor
ConfigTwo::~ConfigTwo()
{
    QHashIterator<int, Player *> it(mPlayers);
    while (it.hasNext())
    {
        it.next();
        Player *player = it.value();
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
    QHashIterator<int, Player *> it = playerIterator();
    while (it.hasNext())
    {
        it.next();
        Player *player = it.value();
        int no = it.key();
        KConfigGroup playercfg = cfg->group(QStringLiteral("LSkat_Player%1").arg(no));
        player->save(playercfg);
    }
}

// Load properties
void ConfigTwo::load(KConfig *cfg)
{
    reset();
    KConfigGroup group = cfg->group("LSkatData");
    int num;
    num = group.readEntry("input0", (int)mInputTypes[0]);
    setInputType(0, (InputDeviceType)num);
    num = group.readEntry("input1", (int)mInputTypes[1]);
    setInputType(1, (InputDeviceType)num);

    // Load player
    QHashIterator<int, Player *> it = playerIterator();
    while (it.hasNext())
    {
        it.next();
        Player *player = it.value();
        int no = it.key();
        KConfigGroup playercfg = cfg->group(QStringLiteral("LSkat_Player%1").arg(no));
        player->load(playercfg);
    }
}

// Retrieve a player.
Player *ConfigTwo::player(int no)
{
    if (!mPlayers.contains(no)) return nullptr;
    return mPlayers[no];
}

// Retrieve player as iterator
QHashIterator<int, Player *> ConfigTwo::playerIterator()
{
    QHashIterator<int, Player *> it(mPlayers);
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
