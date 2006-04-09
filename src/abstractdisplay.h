#ifndef ABSTRACT_DISPLAY_H
#define ABSTRACT_DISPLAY_H

// Qt includes
#include <Q3Canvas>
#include <QList>
#include <QHash>

// KDE includes
#include <kdebug.h>

// local includes

// Forward declaration
class Deck;
class Player;
class CardSprite;
class TextSprite;
class RectangleSprite;


/**
 * Abstract display engine.
 */
class AbstractDisplay : public QObject
{
  Q_OBJECT

  public:
    /** Constructor for the engine
     *  @param grafixDir The grafic directory
     *  @param deck The card deck
     *  @param canvas The parent canvas
     *  @param parent The parent object
     */
    AbstractDisplay(QString grafixDir, Deck* deck, Q3Canvas* canvas, QObject* parent);

    /** Destructor.
    */
    virtual ~AbstractDisplay();

    /** Set a new deck object.
     *  @param deck The deck
     */
    void setDeck(Deck* deck);  

    /** Set the advance period 
     *  @param advancePeriod The advance period
     */
    void setAdvancePeriod(int advancePeriod);

    /** Reset the display. Clear all sprites etc.
    */
    virtual void reset();

    /** Start the display.
     */
    virtual void start() = 0; 

    /** Play a card on the display. The card is moved from
     *  its current position to the card deposit.
     * @param playerNumber  The player number
     * @param height Top (0) or bottm (1) card
     * @param cardNumber The card number (0-7)
     * @param playerNumber The player number (0-1)
     * @param phase Movement phase (1st part, 2nd part, etc) [optional]
     */
    virtual void play(int cardNumber, int playerNumber, int phase) = 0;

    /** Turn a card on the display. The card is flipped backside
     *  to frontside.
     * @param playerNumber  The player number
     * @param height Top (0) or bottm (1) card
     * @param cardNumber The card number (0-7)
     */
    virtual void turn(int cardNumber) = 0;

    /** Remove the given card from the display.
     * @param winnerPosition  The position of the winner (0,1)
     * @param cardNumber The number of the card
     * @param delta Card offset from target position (0,1,2,...)
     */
    virtual void remove(int winnerPosition, int cardNumber, int delta) = 0;

    /** Removes all text/rectangle/etc sprites from the display (except cards).
     */
    virtual void clearSprites();

    /** Reload all card sprite graphics. This must be called if the
     *  deck graphicsh changed.
     */
    virtual void updateSpriteGraphics();

  
  public slots:
    virtual void convertMousePress(QPoint mouse, int& position, int& number) = 0;

  protected:
    /** Create a Canvas sprite for a given card number
     * @param no The sprite card number
    */
    virtual CardSprite* createSprite(int no);

    /** Create the sprites for all possible cards and store them
      * here.
      */
    virtual void createSprites();


  protected:
    /** The card deck */
    Deck* mDeck;
    /** The work canvas */
    Q3Canvas* mCanvas;
    /** Canvas advance period */
    int mAdvancePeriod;
    /** Text sprite list */
    QList<Q3CanvasItem*> mSprites;
    /** Store the card sprite indexed by the card value */
    static QHash<int,CardSprite*> mCards;
    /** Grafic directory */
    QString mGrafixDir;

};

#endif
