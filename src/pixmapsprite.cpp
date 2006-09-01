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
#include <Q3CanvasPixmapArray>
#include <Q3ValueList>
#include <Q3PointArray>

// KDE includes
#include <kdebug.h>

// Local includes
#include "pixmapsprite.h"

// Constructor for the view
PixmapSprite::PixmapSprite(Q3CanvasPixmapArray* a, Q3Canvas* canvas)
    : Q3CanvasSprite(a, canvas)
{
}


// Factory constructor method for a new sprite.
PixmapSprite* PixmapSprite::create(Q3Canvas* canvas, QPixmap* pixmap)
{
  // List necessary to construct pixmap array
  Q3ValueList<QPixmap> spriteList;
  Q3PointArray hotspots;

  // Append front card
  spriteList.append(pixmap->copy());
  hotspots.append(QPoint(pixmap->width()/2,pixmap->height()/2));

  Q3CanvasPixmapArray* a = new Q3CanvasPixmapArray(spriteList, hotspots);
  PixmapSprite* sprite = new PixmapSprite(a, canvas);
  sprite->setAnimated(false);
  sprite->hide();
  return sprite;
}
