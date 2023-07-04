/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "abstractengine.h"

// Qt includes

// KF includes

// Constructor for the game document/engine
AbstractEngine::AbstractEngine(QWidget *parent)
               : QObject(parent)
{
    mGameStatus = Stopped;
    mPlayers.clear();
}

// Add a player to the game
void AbstractEngine::addPlayer(int no, Player *player)
{
    mPlayers[no] = player;
}

// Retrieve the player of the given number.
Player *AbstractEngine::player(int no)
{
    return mPlayers[no];
}

#include "moc_abstractengine.cpp"
