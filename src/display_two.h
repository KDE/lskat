#ifndef DISPLAY_TWO_H
#define DISPLAY_TWO_H

// Qt includes
#include <Q3Canvas>
#include <QPixmap>
#include <QHash>

// KDE includes
#include <kdebug.h>

// local includes
#include "abstractdisplay.h"
#include "deck.h"
#include "player.h"

// Forward declaration
class PixmapSprite;


/**
 * The display engine for a two player game.
 */
class DisplayTwo : public AbstractDisplay
{
  Q_OBJECT

  public:
    /** Constructor for the engine
     *  @param grafixDir The grafic directory
     *  @param deck The card deck
     *  @param canvas The parent canvas
     *  @param parent The parent object
     */
    DisplayTwo(QString grafixDir, Deck* deck, Q3Canvas* canvas,QObject* parent);

    /** Start the display.
     */
    void start(); 


    /* Init a player on a given position. Create sprites etc.
     * @param player The player object
     * @param position The position to place the player (0,1)
     */
    void deal(Player* player, int position);

    /** Play a card on the display. The card is moved from
     *  its current position to the card deposit.
     * @param cardNumber The card number (0-7)
     * @param playerNumber The player number (0-1)
     * @param phase Movement phase (1st part, 2nd part, etc) [optional]
     */
    void play(int cardNumber, int playerNumber, int phase);

    /** Turn a card on the display. The card is flipped backside
     *  to frontside.
     * @param cardNumber The card number (0-7)
     */
    void turn(int cardNumber);

    /** Remove the given card from the display.
     * @param winnerPosition  The position of the winner (0,1)
     * @param cardNumber The number of the card
     * @param delta Card offset from target position (0,1,2,...)
     */
    void remove(int winnerPosition, int cardNumber, int delta);
  
    /** Returns the time in milli seconds used for shuffling the
     *  inital cards.
     *  @return The time in milliseconds.
     */
    int shuffleTime();

    /** Display the score on the game board.
     * @param position Which player position
     * @param score The score to display
     */
    void showScore(int position, int score);

    /** Display a text on the game board.
     * @param text The text to display
     */
    void showText(QString text);

    /** Show the move icon for the given player
     *  @param no The player number (-1: show none)
     */
    void showMove(int no = -1);

  public slots:
    /** Convert the position of a mouse click to a logical
      * game position, that is position (up/down) and a 
      * card number (0-7)
      */
    void convertMousePress(QPoint mouse, int& playerNumber, int& cardNumber);

  protected:
    void calcXYFromNumber(int cardNumber, int& x, int& y);
    CardSprite* getCardSprite(int cardValue);
    CardSprite* getCardSprite(int position, int h, int cardNumber);
    CardSprite* getCardSprite(int position, int h, int y, int x);


  private:
    /** Pixmap for movement sprite */
    QPixmap* mMovePixmap;
    QHash<int,PixmapSprite*> mMoveSprites;
};

#endif
