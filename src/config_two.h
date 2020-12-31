/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONFIG_TWO_H
#define CONFIG_TWO_H

// Qt includes
#include <QHash>

// KF includes
#include <KConfig>

// local includes
#include "abstractinput.h"
#include "lskat_debug.h"
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

Q_SIGNALS:
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
