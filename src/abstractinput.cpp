/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "abstractinput.h"

// Qt includes

// KF includes

// Constructor for the engine
AbstractInput::AbstractInput(QObject *parent)
    : QObject(parent)
{
    mInputAllowed = false;
    mId           = -1;
}

// Set player id number
void AbstractInput::setId(int id)
{
    mId = id;
}

// Allow or disallow input with this device
void AbstractInput::setInputAllowed(bool allowed)
{
    mInputAllowed = allowed;
}

// Retrieve whether input is allowed or not.
bool AbstractInput::inputAllowed()
{
    return mInputAllowed;
}
