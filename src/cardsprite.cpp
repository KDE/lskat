// General includes
#include <math.h>

// Qt includes
#include <Q3ValueList>
#include <Q3PointArray>
#include <QPoint>
#include <QMatrix>

// KDE includes
#include <kdebug.h>

// Local includes
#include "cardsprite.h"

// How many card frames for turning a card (2*this)
#define STEPS_TURN_CARD 10
// Delay for card turning animation
#define ANIM_CNT_TURNING  1
// Time in update cycles for a move
#define MOVE_TIME 20.0  /* 0.5s */        
// Time in update cycles for a remove
#define REMOVE_TIME 5.0  /* 0.1s */        
// Time in update cycles for a shuffle move
#define SHUFFLEMOVE_TIME 5.0  /* 0.025s */        


// Constructor for the view
CardSprite::CardSprite(Q3CanvasPixmapArray* a, Q3Canvas* canvas)
    : Q3CanvasSprite(a, canvas)
{
  mAnimationState = Idle;
  mPixmapArray = a;
}

// Stop all movment and animation
void CardSprite::stop()
{
  mAnimationState = Idle;
  mAnimationCnt = 0;
  setBackside();
}

// Set the current aniamtion mode of this sprite
void CardSprite::setTurning(bool front)
{
  mFrontFlag = front;
  mAnimationState = Turning;
  mAnimationCnt   = 0;
}

// Set target position and calculate moving speed.
void CardSprite::calcTargetAndSpeed(int tx, int ty, double time)
{
  mMoveTarget     = QPoint(tx, ty);
  // Calculate move speed so that the duration for the move
  // is fixed
  double dx    = mMoveTarget.x()-x();
  double dy    = mMoveTarget.y()-y();
  double angle = atan2(dy,dx);
  mMoveSpeedX = cos(angle)* sqrt(dx*dx + dy*dy) / time;
  mMoveSpeedY = sin(angle)*sqrt(dx*dx + dy*dy) / time;
}

// Move the sprite slowly to the target area. Stop
// movement if it arrived there.
void CardSprite::setMove(int tx, int ty)
{
  mAnimationState = Moving;
  mAnimationCnt   = 0;
  calcTargetAndSpeed(tx, ty, MOVE_TIME);
}

// Move the sprite slowly to the target area. Stop
// movement and remove sprite if it arrived there.
void CardSprite::setRemove(int tx, int ty)
{
  calcTargetAndSpeed(tx, ty, REMOVE_TIME);
  mAnimationState = Removing;
}

// Delay before moving, then move the sprite fast to the
// target area. Stop movement and depending on the last
// argument turn backside/frontside sprite if it arrived there.
void CardSprite::setShuffleMove(int tx, int ty, int delay, bool front)
{
  calcTargetAndSpeed(tx, ty, SHUFFLEMOVE_TIME);
  mAnimationState = ShuffleMove;
  mAnimationCnt   = delay;
  mFrontFlag      = front;
}

// Display the card front pixmap image
void CardSprite::setFrontside()
{
  // Choose card front frame
  setFrame(0);
}

// Display the card back pixmap image
void CardSprite::setBackside()
{
  // Choose card back frame (last one in the animation sequence)
  setFrame(frameCount()-1);
}


// Perform a move by a delta given by the sprites velocity.
// Returns true if the target position is reached
bool CardSprite::deltaMove()
{
  // Calculate difference vector
  double dx    = mMoveTarget.x()-x();
  double dy    = mMoveTarget.y()-y();

  // Check arrival at target
  if (dx*dx + dy*dy < mMoveSpeedX*mMoveSpeedX + mMoveSpeedY*mMoveSpeedY)
  {
    move(mMoveTarget.x(), mMoveTarget.y());
    return true;
  }
  // Move towards target by given velocity
  else
  {
    setX( x() + mMoveSpeedX );
    setY( y() + mMoveSpeedY );
    return false;
  }
}


// CanvasItem advance method (see Q3CanvasItem)
void CardSprite::advance(int phase)
{
  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    Q3CanvasSprite::advance(phase);
    return ;
  }

  // Turn a card
  if (mAnimationState == Turning)
  {
    mAnimationCnt++;
    // Turn delay counter
    if (mAnimationCnt >= ANIM_CNT_TURNING)
    {
      mAnimationCnt = 0;
      // Check whether animation is over
      if ( (mFrontFlag && frame() == 0) ||
           (!mFrontFlag && frame() == frameCount()-1) )
      {
        mAnimationState = Idle;
      }
      else
      {
        if (mFrontFlag) setFrame(frame()-1);
        else setFrame(frame()+1);
      }
    }
  }// end if Turning

  // Move a card
  else if (mAnimationState == Moving)
  {
    // Perform a move by a delta given by the sprites velocity.
    if (deltaMove())
    {
      mAnimationState = Idle;
    }
  }// end if Moving

  // Move a card
  else if (mAnimationState == Removing)
  {
    // Perform a move by a delta given by the sprites velocity.
    if (deltaMove())
    {
      // Turn to backside
      setTurning(false);
    }
  }// end if Removing

  // Shuffle move a card
  else if (mAnimationState == ShuffleMove)
  {
    // First delay move until counter is run down
    if (mAnimationCnt > 0)
    {
      mAnimationCnt--;
      if (mAnimationState == 0)
       kdDebug() << "Start moving sprite " << this << endl;
    }
    // Then move to target position
    else
    {
      // Perform a move by a delta given by the sprites velocity.
      if (deltaMove())
      {
        if (mFrontFlag) mAnimationState = Turning;
        else mAnimationState = Idle;
        mAnimationCnt = 0;
      }
    }
  }// end if ShuffleMove

  Q3CanvasSprite::advance(phase);
}


// Load and create card pixmap array
Q3CanvasPixmapArray* CardSprite::createPixmapArray(QPixmap* front, QPixmap* back)
{
  // List necessary to construct pixmap array
  Q3ValueList<QPixmap> spriteList;
  Q3PointArray hotspots;

  // Append front card
  spriteList.append(front->copy());
  hotspots.append(QPoint(0,0));

  // Turn the frontside of the card 0..90 degree
  for (int i=1;i<STEPS_TURN_CARD;i++)
  {
    QMatrix m;
    // Get an angle eps..90 deg for the values i is running
    double angle = (double)i/(double)STEPS_TURN_CARD*90.0;
    // Conversion to rad
    angle = angle/180.0*M_PI;
    // Scale pixmap to simulate rotating card
    m.scale(cos(angle), 1.0);
    QPixmap pm = front->transformed(m, Qt::SmoothTransformation);
    // Find translation (difference in sizes) to compensate with hotspot
    int dx = front->width()-pm.width();
    // Append to lists
    spriteList.append(pm);
    hotspots.append(QPoint(-dx/2,0));
  }

  // Turn the backside of the card 90..eps degree
  for (int i=1;i<STEPS_TURN_CARD;i++)
  {
    QMatrix m;
    // Get an angle 0..90 deg for the values i is running
    double angle = 90.0-((double)i/(double)STEPS_TURN_CARD*90.0);
    // Conversion to rad
    angle = angle/180.0*M_PI;
    // Scale pixmap to simulate rotating card
    m.scale(cos(angle), 1.0);
    QPixmap pm = back->transformed(m, Qt::SmoothTransformation);
    // Find translation (difference in sizes) to compensate with hotspot
    int dx = front->width()-pm.width();
    // Append to lists
    spriteList.append(pm);
    hotspots.append(QPoint(-dx/2,0));
  }

  // Append back card
  spriteList.append(back->copy());
  hotspots.append(QPoint(0,0));

  Q3CanvasPixmapArray* a = new Q3CanvasPixmapArray(spriteList, hotspots);
  return a;
}

// Factory constructor method for a new card sprite.
CardSprite* CardSprite::create(Q3Canvas* canvas, QPixmap* front, QPixmap* back)
{
  Q3CanvasPixmapArray* a = CardSprite::createPixmapArray(front, back);
  CardSprite* sprite = new CardSprite(a,canvas);
  sprite->setAnimated(true);
  sprite->hide();
  return sprite;
}

// Update the sprites card graphics by the pixmaps 
void CardSprite::updateGraphics(QPixmap* front, QPixmap* back)
{
  // Get rid of old graphics
  bool vis = isVisible();
  hide();
  if (mPixmapArray) delete mPixmapArray;
  Q3CanvasPixmapArray* a = createPixmapArray(front, back);
  setSequence(a);
  mPixmapArray = a;
  setVisible(vis);
}

