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
#include <QSize>
#include <QPoint>
#include <QPen>
#include <QBrush>
#include <QLinearGradient>

// KDE includes
#include <kdebug.h>
#include <klocalizedstring.h>

// Local includes
#include "display_two.h"
#include "cardsprite.h"
#include "textsprite.h"
#include "rectanglesprite.h"
#include "pixmapsprite.h"


// Board geometry
// Start play cards of player 1/2
#define BOARD_START_1_X       50
#define BOARD_START_1_Y       30
#define BOARD_START_2_X       50
#define BOARD_START_2_Y      300
// Separation betweem two cards on the board
#define BOARD_CARD_SEP_X      20
#define BOARD_CARD_SEP_Y      20
// Separation between stacked cards, to allows seeing the
// lower card
#define BOARD_HEIGHT_SEP_X    10
#define BOARD_HEIGHT_SEP_Y    10

// The play area of player 1/2. Here the cards are
// moved to when a move is done.
#define BOARD_PLAY_1_X       450       
#define BOARD_PLAY_1_Y       200       
#define BOARD_PLAY_2_X       480       
#define BOARD_PLAY_2_Y       250       

// Delta of move sprites to play area
#define BOARD_MOVE_X          25

// Where to remove the cards for player 1/2 after
// a mvoe has be done (the players local pile)
#define BOARD_REMOVE_1_X       460
#define BOARD_REMOVE_1_Y       10
#define BOARD_REMOVE_2_X       460
#define BOARD_REMOVE_2_Y       450

// Where is the card deck before shuffling
#define BOARD_DECK_X         465
#define BOARD_DECK_Y         225

// Delay in canvas update cycle for each card in shuffling
#define TIME_DELAY_SHUFFLE        5 /* in canvas cycles */
#define TIME_DELAY_AFTER_SHUFFLE  5 /* Extra delay after shuffling */


// Constructor for the engine
DisplayTwo::DisplayTwo(QString grafixDir, Deck* deck, Q3Canvas* canvas, QObject* parent)
    : AbstractDisplay(grafixDir, deck, canvas, parent)
{
  // Choose a background color
  canvas->setBackgroundColor(QColor(0,0,128));

  // Load move icon
  QString filename = grafixDir+ QString("moveicon.png");
  mMovePixmap = new QPixmap();
  if (!mMovePixmap->load(filename))
  {
    kError() << "Cannot load file " << filename << endl;
  }
}

// Start display
void DisplayTwo::start()
{
  // Clear display text and rectangles
  clearSprites();
  mMoveSprites.clear();

  // Stop all card sprites
  for (int i=0; i<mCards.size(); i++)
  {
    CardSprite* sprite = mCards[i];
    sprite->stop();
  }

  // Card size
  QSize size   = mDeck->cardSize();

  RectangleSprite* rect;

  rect = new RectangleSprite(3, QColor(20,20,148), mCanvas);
  rect->setSize(size.width()+BOARD_PLAY_2_X-BOARD_PLAY_1_X+6,
                size.height()+BOARD_PLAY_2_Y-BOARD_PLAY_1_Y+6);
  rect->move(BOARD_PLAY_1_X-3, BOARD_PLAY_1_Y-3);
  rect->show();
  mSprites.append(rect);

  rect = new RectangleSprite(3, QColor(20,20,148), mCanvas);
  rect->setSize(4*size.width()+3*BOARD_CARD_SEP_X+4*BOARD_HEIGHT_SEP_X-10,
                2*size.height()+BOARD_CARD_SEP_Y+2*BOARD_HEIGHT_SEP_Y+10);
  rect->move(BOARD_START_1_X-10, BOARD_START_1_Y-10);
  rect->show();
  mSprites.append(rect);

  rect = new RectangleSprite(3, QColor(20,20,148), mCanvas);
  rect->setSize(4*size.width()+3*BOARD_CARD_SEP_X+4*BOARD_HEIGHT_SEP_X-10,
                2*size.height()+BOARD_CARD_SEP_Y+2*BOARD_HEIGHT_SEP_Y+10);
  rect->move(BOARD_START_2_X-10, BOARD_START_2_Y-10);
  rect->show();
  mSprites.append(rect);
  
  PixmapSprite* sprite;
  sprite = PixmapSprite::create(mCanvas, mMovePixmap);
  sprite->setZ(300);
  sprite->move(BOARD_MOVE_X, 
               BOARD_START_1_Y);
  sprite->hide();
  mMoveSprites[0] = sprite;

  sprite = PixmapSprite::create(mCanvas, mMovePixmap);
  sprite->setZ(300);
  sprite->move(BOARD_MOVE_X, 
               BOARD_START_2_Y+2*size.height()+BOARD_CARD_SEP_Y);
  sprite->hide();
  mMoveSprites[1] = sprite;
}

// Init a player on a given screen/board position (0,1)
void DisplayTwo::deal(Player* player, int position)
{
  if (position != 0 && position != 1)
  {
    kFatal() << "Wrong player position " << position << endl;
    return ;
  }
  if (!player)
  {
    kFatal() << "No player given " << endl;
    return ;
  }

  // Retrieve size of a card
  QSize size   = mDeck->cardSize();
  // Start offset for display
  QPoint offset = QPoint(BOARD_START_1_X, BOARD_START_1_Y);
  // Offset for second player
  if (position == 1)
  {
    offset =  QPoint(BOARD_START_2_X, BOARD_START_2_Y);
  }


  // Two height level of cards
  for (int h=0; h<2; h++)
  {
    // Two rows of cards
    for (int y=0; y<2; y++)
    {
      // Four columns of cards
      for (int x=0; x<4; x++)
      {
        // Get playerNumber on game board (0 1 2 3)
        //                                (4 5 6 7)
        int cardPos = x + 4*y + 8*h;
        // Get card of player belonging to this playerNumber
        int cardNo  = player->getCard(cardPos);
        // Create sprite with card correct card image
        CardSprite* sprite = mCards[cardNo];
        // Move sprite to correct board playerNumber
        QPoint pos = offset + QPoint(size.width()*x  + BOARD_CARD_SEP_X*x,
                                     size.height()*y + BOARD_CARD_SEP_Y*y);
        // Add shift for stacked cards
        pos += h*QPoint(BOARD_HEIGHT_SEP_X, BOARD_HEIGHT_SEP_Y);
        sprite->setZ(50-10*h);
        sprite->move(BOARD_DECK_X, BOARD_DECK_Y);
        sprite->show();
        int delay = position + 2*x + 8*y + 16*(1-h);
        delay *=  TIME_DELAY_SHUFFLE; // [canvas cycles]
        // Move to the target position. The moves is started with
        // a little delay and depending on the last argument the
        // backside or frontside is shown after the move
        sprite->setShuffleMove(pos.x(), pos.y(), delay, h==0);
        // Store sprite
      }// next x
    }// next y
  }// next h
}// end initPlayer


// Returns the time in milli seconds used for shuffling the inital cards
int DisplayTwo::shuffleTime()
{
  return 32*TIME_DELAY_SHUFFLE*mAdvancePeriod + TIME_DELAY_AFTER_SHUFFLE*mAdvancePeriod;
}


// Convert a mouse coordinate back to card numbers
void DisplayTwo::convertMousePress(QPoint mouse, int& playerNumber, int& cardNumber)
{
  // Card size
  QSize size   = mDeck->cardSize();
  // Resulting coordinates
  int x = -1;
  int y = -1;

  // Out of board at top
  if (mouse.y() < BOARD_START_1_Y)
  {
    playerNumber = -1;
    return;
  }
  // Upper set
  else if (mouse.y() < BOARD_START_2_Y)
  {
    playerNumber = 0;
    // Substract offset
    x = mouse.x() - BOARD_START_1_X;
    y = mouse.y() - BOARD_START_1_Y;
    // Divide by cardsize and spacing
    x /= size.width()  + BOARD_CARD_SEP_X; 
    y /= size.height() + BOARD_CARD_SEP_Y; 
  }
  // Lower set
  else
  {
    playerNumber = 1;
    // Substract offset
    x = mouse.x() - BOARD_START_2_X;
    y = mouse.y() - BOARD_START_2_Y;
    // Divide by cardsize and spacing
    x /= size.width()  + BOARD_CARD_SEP_X; 
    y /= size.height() + BOARD_CARD_SEP_Y; 
  }

  // Out of board 
  if (y<0 || y>1 || x<0 || x>3)
  {
    playerNumber = -1;
    return;
  }

  // Resulting card number
  cardNumber = x + 4*y;
}


// Get x (0-3) y(0-1) board coordinates from card number (0-7)
void DisplayTwo::calcXYFromNumber(int cardNumber, int& x, int& y)
{
  x = cardNumber % 4;
  y = cardNumber / 4;
}

// Get a cardsprite given the card value
CardSprite* DisplayTwo::getCardSprite(int cardValue)
{
  CardSprite* sprite =  mCards[cardValue];
  if (!sprite)
  {
    kFatal() << "Could not find cardsprite for card value " << cardValue 
              << " Stored are " << mCards.size() << " sprites " 
              << endl;
    return 0;
  }
  return sprite;
}


// Play a frontside card to the play area
void DisplayTwo::play(int cardNumber, int playerNumber, int phase)
{
  CardSprite* sprite = getCardSprite(cardNumber);
  // Set z coordinate depending on move phase, that is latter cards will
  // be put on top
  sprite->setZ(100 + 5*phase);
  if (playerNumber == 0)
  {
    sprite->setMove(BOARD_PLAY_1_X, BOARD_PLAY_1_Y);
  }
  else
  {
    sprite->setMove(BOARD_PLAY_2_X, BOARD_PLAY_2_Y);
  }
}


// Turn a backside card to the front
void DisplayTwo::turn(int cardNumber)
{
  CardSprite* sprite = getCardSprite(cardNumber);
  sprite->setTurning(true);
}


// Remove the given card from the display.
void DisplayTwo::remove(int winnerPosition, int cardNumber, int delta)
{
  CardSprite* sprite = getCardSprite(cardNumber);
  // Pile cards on top of each other
  sprite->setZ(100 + delta);
  if (winnerPosition == 0)
  {
    sprite->setRemove(BOARD_REMOVE_1_X + delta*3, BOARD_REMOVE_1_Y+ delta*1);
  }
  else
  {
    sprite->setRemove(BOARD_REMOVE_2_X + delta*3, BOARD_REMOVE_2_Y+ delta*1);
  }
}

// Display a text on the game board.
void DisplayTwo::showText(QString s)
{
  double x;
  double y;
  x = BOARD_START_1_X;
  y = BOARD_START_1_Y;

  QSize size   = mDeck->cardSize();

  x += 2.5*size.width();
  y += 2.5*size.height();
  

  // Display text sprite
  TextSprite* text = new TextSprite(mCanvas);
  QFont font;
  font.setPixelSize(20);
  text->setText(s);
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(255, 128, 0));
  text->move(x, y);
  text->show();
  mSprites.append(text);
}


// Display the score on the game board
void DisplayTwo::showScore(int position, int score)
{
  double x;
  double y;
  if (position == 0)
  {
    x = BOARD_START_1_X;
    y = BOARD_START_1_Y;
  }
  else
  {
    x = BOARD_START_2_X;
    y = BOARD_START_2_Y;
  }

  QSize size   = mDeck->cardSize();

  x += 2.5*size.width();
  y += size.height();
  

  // Display text sprite
  TextSprite* text = new TextSprite(mCanvas);
  QFont font;
  font.setPixelSize(24);
  text->setText(i18n("%1 points", score));
  text->setFont(font);
  text->setTextFlags(Qt::AlignCenter);
  text->setColor(QColor(255, 128, 0));
  text->move(x, y);
  text->show();
  mSprites.append(text);
}


// Show the move icon for the given player
void DisplayTwo::showMove(int no)
{
  QHashIterator<int,PixmapSprite*> it(mMoveSprites);
  while(it.hasNext())
  {
    it.next();
    PixmapSprite* sprite = it.value();
    sprite->hide();
  }

  kDebug() << "Display start sprite to " << no << endl;
  if (no>=0) mMoveSprites[no]->show();
}

#include "display_two.moc"
