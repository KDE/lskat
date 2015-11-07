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

#ifndef CONFIG_TWO_H
#define CONFIG_TWO_H

// Qt includes
#include <QHash>

// KDE includes
#include <kdebug.h>
#include <kconfig.h>

// local includes
#include "abstractinput.h"
#include "player.h"

// Forward declaration

using namespace InputDevice;

/**
 * LSkat game configuration
 */
class ConfigTwo : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor for the config
     * @param parent The parent object
     */
    explicit ConfigTwo(QObject *parent);

    /**
     * Destructor
     */
    virtual ~ConfigTwo();

    /**
     * Reset the config data.
     */
    void reset();

    /**
     * Retrieve hash iterator over players.
     * @return The iterator.
     */
    QHashIterator<int, Player *> playerIterator();

    /**
     * Retrieve a player.
     * @param no The player number
     * @return The player.
     */
    Player *player(int no);

    /**
     * Retrieve input type of given player.
     * @param no The player number
     * @return The input type.
     */
    InputDeviceType inputType(int no);

    /**
     * Set the input type for a given players.
     * @param no The player number
     * @param type The input type
     */
    void setInputType(int no, InputDeviceType type);

    /**
     * Saves the properties
     * @param cfg The config object.
     */
    void save(KConfig *cfg);

    /**
     * Read properties.
     * @param cfg The config object.
     */
    void load(KConfig *cfg);

signals:
    /**
     * Signal emitted when input type changes.
     * @param no The player number
     * @param type The input type
     */
    void signalInputType(int no, InputDeviceType type);

private:
    // Current player
    QHash<int, Player *> mPlayers;
    // Input types
    QHash<int, InputDeviceType> mInputTypes;
};

#endif
