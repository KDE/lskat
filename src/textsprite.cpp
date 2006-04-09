// General includes

// Qt includes

// KDE includes
#include <kdebug.h>

// Local includes
#include "textsprite.h"

// Constructor for the view
TextSprite::TextSprite(Q3Canvas* canvas)
    : Q3CanvasText(canvas)
{
  setZ(200);
  hide();
}

