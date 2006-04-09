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
