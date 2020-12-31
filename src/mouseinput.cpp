/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mouseinput.h"

// Qt includes

// KF includes

// Constructor for the input
MouseInput::MouseInput(QObject *parent)
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
        //qCDebug(LSKAT_LOG) << "MouseInput:: Convert to " << playerNumber << "," << cardNumber;
        emit signalPlayerInput(mId, playerNumber, cardNumber);
    }
}
