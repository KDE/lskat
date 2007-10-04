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

#include "textsprite.h"

// Qt includes

// KDE includes
#include <kdebug.h>
#include <kconfiggroup.h>

// Constructor for the sprite
TextSprite::TextSprite(const QString &text, const QString &id, ThemeManager* theme, QGraphicsScene* scene)
          : Themable(id, theme), QGraphicsTextItem(0, scene)
{
  setPlainText(text);
  hide();
 
  if (theme) theme->updateTheme(this);
}


// Constructor for the sprite
TextSprite::TextSprite(const QString &id, ThemeManager* theme, QGraphicsScene* scene)
          : Themable(id, theme), QGraphicsTextItem(0, scene)
{
  hide();
 
  if (theme) theme->updateTheme(this);
}


void TextSprite::setText(const QString &text)
{
  setPlainText(text);
  thememanager()->updateTheme(this);
}


// Main themable function. Called for any theme change. The sprites needs to
// resize and redraw here.
void TextSprite::changeTheme()
{

  // Get scaling change
  double scale    = thememanager()->getScale();
  setScale(scale);

  // Retrieve theme data from configuration
  KConfigGroup config = thememanager()->config(id());

  // Size
  double width  = config.readEntry("width", 1.0);
  double height = config.readEntry("height", 0.0);
  width *= scale;
  height *= scale;

  // Position
  QPointF pos = config.readEntry("pos", QPointF(1.0,1.0));
  pos *= scale;
  setPos(pos.x(), pos.y());

  // z-Value
  double zValue = config.readEntry("zValue", 0.0);
  setZValue(zValue);

  // Text font
  bool bold = config.readEntry("bold", false);
  QFont font;
  font.setPixelSize(int(height));
  font.setBold(bold);
  setFont(font);
  if (width < boundingRect().width())
  {
    setTextWidth(width);   
  }

  // Retrieve font color
  QColor fontColor;
  fontColor = config.readEntry("fontColor", QColor(Qt::white));
  setDefaultTextColor(fontColor);

  // Centering
  bool center = config.readEntry("center", false);
  resetMatrix();
  if (center)
  {
    translate(-boundingRect().width()/2.0, 0.0);
  }

  update();
}

