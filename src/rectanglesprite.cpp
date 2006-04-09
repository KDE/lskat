// General includes

// Qt includes
#include <QPen>

// KDE includes
#include <kdebug.h>

// Local includes
#include "rectanglesprite.h"

#define MAX_ANIM_CNT 160

// Constructor for the view
RectangleSprite::RectangleSprite(int width, QColor color, Q3Canvas* canvas)
    : Q3CanvasRectangle(canvas)
{
  setZ(-50);

  QPen pen(color);
  pen.setWidth(width);
  setPen(pen);
  hide();

  mColor = color;
  mWidth = width;
}


// CanvasItem advance method (see Q3CanvasItem)
void RectangleSprite::advance(int phase)
{
   Q3CanvasRectangle::advance(phase);
}
