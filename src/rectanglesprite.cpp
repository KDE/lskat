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

// Qt includes
#include <QPen>

// KDE includes
#include <kdebug.h>

// Local includes
#include "rectanglesprite.h"

#define MAX_ANIM_CNT 160

// Constructor for the view
RectangleSprite::RectangleSprite(int width, QColor color, QGraphicsScene* canvas)
    : QGraphicsRectItem(0, canvas)
{
  setZValue(-50);

  QPen pen(color);
  pen.setWidth(width);
  setPen(pen);
  hide();

  mColor = color;
  mWidth = width;
}


// CanvasItem advance method 
void RectangleSprite::advance(int phase)
{
   QGraphicsItem::advance(phase);
}
