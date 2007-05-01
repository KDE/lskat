#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H
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
#include <QPoint>

// KDE includes
#include <kdebug.h>

// Local includes
#include "abstractinput.h"


/** Mouse input device
 */
class MouseInput : public AbstractInput
{
  Q_OBJECT

  public:
    /** Constructor for the input
     *  @param parent The parent object
     */
    MouseInput(QObject* parent);

    /** Retrieve the type of device.
      * @return The decice type.
      */
    virtual InputDevice::InputDeviceType type() {return InputDevice::TypeMouseInput;}

  public slots:  
    /** Recevied a mouse press event 
      * @param point The position [screen coordiantes]
      */
    void mousePress(QPoint point);

  signals:  
    /** Convert mouse coordinate. 
      * @param mouse The mouse position [screen coordinates]
      * @param playerNumber The player number [0-1]
      * @param cardNumber   The card number [0-7]
      */
    void signalConvertMousePress(QPoint mouse, int& playerNumber, int &cardNumber);
};

#endif
