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

// General includes
#include <math.h>

// Qt includes

// KDE includes
#include <kdebug.h>

// Local includes
#include "pixmapsprite.h"

// Constructor for the view
PixmapSprite::PixmapSprite(const QPixmap & pixmap, QGraphicsScene* canvas)
    : QGraphicsPixmapItem(pixmap, 0, canvas)
{
  hide();
  // Center pixmap
  resetMatrix();
  translate(-pixmap.width()/2.0, -pixmap.height()/2.0);
}

