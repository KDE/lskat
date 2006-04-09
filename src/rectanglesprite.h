#ifndef RECTANGLE_SPRITE_H
#define RECTANGLE_SPRITE_H

// Qt includes
#include <Q3CanvasRectangle>
#include <QColor>


/**
 * The sprite for canvas rectangle
 */
class RectangleSprite : public Q3CanvasRectangle
{

  public:
    /** Constructor for the sprite.
     *  @param width The rectangle border width
     *  @param color The boeder color
     *  @param parent The parent canvas
     */
    RectangleSprite(int width, QColor color, Q3Canvas* canvas);

     /** Standard Q3CanvasItem advance method
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
