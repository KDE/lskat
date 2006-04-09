#ifndef TEXT_SPRITE_H
#define TEXT_SPRITE_H

// Qt includes
#include <Q3CanvasText>


/**
 * The sprite for a card on the canvas.
 */
class TextSprite : public Q3CanvasText
{

  public:
    /** Constructor for the sprite.
     *  @param parent The parent canvas
     */
    TextSprite(Q3Canvas* canvas);

  private:

};

#endif
