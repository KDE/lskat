/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ABSTRACT_INPUT_H
#define ABSTRACT_INPUT_H

// Qt includes
#include <QObject>

// KF includes

// Local includes
#include "lskat_debug.h"

namespace InputDevice
{
/** Determine the type of input to use for the player */
typedef enum {TypeMouseInput = 0, TypeAiInput = 1} InputDeviceType;
}

/**
 * Abstract input device for the game.
 */
class AbstractInput : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor for the input
     * @param parent The parent object
     */
    explicit AbstractInput(QObject *parent);

    /**
     * Allow or disallow input with this device
     * @param allowed True if input is allowed
     */
    virtual void setInputAllowed(bool allowed);

    /**
     * Retrieve whether input is allowed or not.
     * @return True if input from this device is allowed.
     */
    virtual bool inputAllowed();

    /**
     * Set the player id number to whom this input belongs to.
     * @param id The id number
     */
    virtual void setId(int id);

    /**
     * Retrieve the type of device.
     * @return The device type.
     */
    virtual InputDevice::InputDeviceType type() = 0;

Q_SIGNALS:
    /**
     * Signals the availability of player input
     * @param id           The input device id
     * @param playerNumber The player number [0-1]
     * @param cardNumber   The card number [0-7]
     */
    void signalPlayerInput(int id, int playerNumber, int cardNumber);

protected:
    /** Player id to whom this input belongs */
    int mId;
    /** Is input allowed? */
    bool mInputAllowed;
};

#endif
