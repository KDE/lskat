#ifndef RECTANGLE_SPRITE_H
#define RECTANGLE_SPRITE_H
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
#include <QGraphicsRectItem>
#include <QColor>


/**
 * The sprite for canvas rectangle
 */
class RectangleSprite : public QGraphicsRectItem
{

  public:
    /** Constructor for the sprite.
     *  @param width The rectangle border width
     *  @param color The boeder color
     *  @param parent The parent canvas
     */
    RectangleSprite(int width, QColor color, QGraphicsScene* canvas);

     /** Standard advance method
      *  @param phase Advance phase
      */
     virtual void advance(int phase);

  private:
    // Original rectangle color
    QColor mColor;
    // Original width
    int mWidth;

    
  

};

#endif
