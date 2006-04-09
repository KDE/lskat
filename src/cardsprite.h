#ifndef CARD_SPRITE_H
#define CARD_SPRITE_H

// Qt includes
#include <Q3CanvasSprite>
#include <Q3CanvasPixmapArray>
#include <QPoint>
#include <QPixmap>


/**
 * The sprite for a card on the canvas.
 */
class CardSprite : public Q3CanvasSprite
{

  public:
    /** Constructor for the sprite.
     *  @param parent The parent window
     */
    CardSprite(Q3CanvasPixmapArray* a, Q3Canvas* canvas);

    // Possible animation states of the sprite
    enum AnimationState {Idle, Turning, Moving, Removing,
                         ShuffleMove};

    /** Display the card front pixmap image
     */
     void setFrontside();

    /** Display the card back pixmap image
     */
     void setBackside();


     /** Standard Q3CanvasItem advance method
      *  @param phase Advance phase
      */
     virtual void advance(int phase);

      /** Stop all movment and animation.
      */
      void stop();

     /** Turn the card to frontside or backside.
       * @param front True (turn to frontside) or false (turn to backside)
       */
     void setTurning(bool front);

     /** Move the sprite slowly to the target area. Stop
       * movement if it arrived there.
       * @param tx Target x
       * @param ty Target y
       */
     void setMove(int tx, int ty);

     /** Move the sprite slowly to the target area. Stop
       * movement and hide sprite if it arrived there.
       * @param tx Target x
       * @param ty Target y
       */
     void setRemove(int tx, int ty);

     /** Delay before moving, then move the sprite fast to the
       * target area. Stop movement and depending on the last
       * argument turn backside/frontside sprite if it arrived there.
       * @param tx Target x
       * @param ty Target y
       * @param delay The Move start delay 
       * @param front Shoe frontside(true) or backside
       */
     void setShuffleMove(int tx, int ty, int delay, bool front);

    /** Update the sprites card graphics by the pixmaps given 
     *  as parameter.
     *  @param front  The frontside pixmap.
     *  @param back   The backside pixmap.
     *  @return The generated canvas pixmap array.
     */
    void updateGraphics(QPixmap* front, QPixmap* back);


    /** Factory constructor method for a new card sprite. The sprite
     *  with frontside and backside as well as an animation between
     *  them is generated.
     *  @param canvas The canvas
     *  @param front  The frontside pixmap.
     *  @param back   The backside pixmap.
     *  @return The generated sprite object.
     */
    static CardSprite* create(Q3Canvas* canvas, QPixmap* front, QPixmap* back);
  
  protected:
    /** Load and create card pixmap array. This method create the in-between
     *  sprites necessary.
     *  @param front  The frontside pixmap.
     *  @param back   The backside pixmap.
     *  @return The generated canvas pixmap array.
     */
    static Q3CanvasPixmapArray* createPixmapArray(QPixmap* front, QPixmap* back);

    /** Set target position and calculate moving speed.
     *  @param tx Target x
     *  @param ty Target y
     *  @param time The movement speed [canvas cycles to reach target]
     */
     void calcTargetAndSpeed(int tx, int ty, double time);

     /** Perform a move by a delta given by the sprites velocity.
      *  @param return True if the target position is reached false otherwise.
      */
     bool deltaMove();
    

  private:
    // Type of animation
    AnimationState mAnimationState;  
    // Counter of animation
    int mAnimationCnt;
    // Target point for movment
    QPoint mMoveTarget;
    // Movement speed
    double mMoveSpeedX;
    double mMoveSpeedY;
    // Front/Backside flag
    bool mFrontFlag;
    // Store our pixmap array
    Q3CanvasPixmapArray* mPixmapArray;



};

#endif
