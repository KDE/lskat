#ifndef PIXMAP_SPRITE_H
#define PIXMAP_SPRITE_H

// Qt includes
#include <Q3CanvasSprite>


/**
 * The sprite for a pixmap on the canvas.
 */
class PixmapSprite : public Q3CanvasSprite
{

  public:
    /** Constructor for the sprite.
     *  TODO
     */
    PixmapSprite(Q3CanvasPixmapArray* a, Q3Canvas* canvas);

    /** Factory constructor method for a new sprite. 
     *  @param canvas The canvas
     *  @param pixmap  The pixmap.
     *  @return The generated sprite object.
     */
    static PixmapSprite* create(Q3Canvas* canvas, QPixmap* pixmap);
  

};

#endif
