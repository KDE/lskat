#ifndef ABSTRACT_INPUT_H
#define ABSTRACT_INPUT_H
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
#include <QObject>

// KDE includes
#include "lskat_debug.h"

// Local includes


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

    /** Constructor for the input
     *  @param parent The parent object
     */
    AbstractInput(QObject* parent);

    /** Allow or disallow input with this device 
      * @param allowed True if input is allowed
      */
    virtual void setInputAllowed(bool allowed);  

    /** Retrieve whether input is allowed or not.
     *  @return True if input from this device is allowed.
     */
    virtual bool inputAllowed();

    /** Set the player id number to whom this input
      * belongs to.
      * @param id The id number
      */
    virtual void setId(int id);

    /** Retrieve the type of device.
      * @return The decice type.
      */
    virtual InputDevice::InputDeviceType type() = 0;


  signals:
    /** Signals the availability of player input
     * @param id           The input device id
     * @param playerNumber The player number [0-1]
     * @param cardNumber   The card number [0-7]
    */
   void signalPlayerInput(int id, int playerNumber, int cardNumber);


  protected:
    /** Player id to whom this input belongs */
    int mId;
    /** Is input allowed ? */
    bool mInputAllowed;

};

#endif
