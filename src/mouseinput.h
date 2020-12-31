/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

// Qt includes
#include <QPoint>

// KF includes

// Local includes
#include "abstractinput.h"
#include "lskat_debug.h"

/**
 * Mouse input device
 */
class MouseInput : public AbstractInput
{
    Q_OBJECT

public:
    /**
     * Constructor for the input
     * @param parent The parent object
     */
    explicit MouseInput(QObject *parent);

    /**
     * Retrieve the type of device.
     * @return The device type.
     */
    InputDevice::InputDeviceType type() override {return InputDevice::TypeMouseInput;}

public Q_SLOTS:
    /**
     * Received a mouse press event
     * @param point The position [screen coordinates]
     */
    void mousePress(const QPoint &point);

Q_SIGNALS:
    /**
     * Convert mouse coordinate.
     * @param mouse The mouse position [screen coordinates]
     * @param playerNumber The player number [0-1]
     * @param cardNumber   The card number [0-7]
     */
    void signalConvertMousePress(QPoint mouse, int &playerNumber, int &cardNumber);
};

#endif
