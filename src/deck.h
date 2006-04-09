#ifndef DECK_H
#define DECK_H

// Qt includes
#include <QObject>
#include <QList>
#include <QPixmap>
#include <QSize>
#include <QHash>

// KDE includes
#include <kdebug.h>
#include <krandomsequence.h>

// Forward declaration

namespace CardDeck
{
    /** The suite and trump names 
    */
    enum Suite    {Club=0,Spade=1,Heart=2,Diamond=3,Grand=4};
    enum CardType {Ace=0,King=1,Queen=2,Jack=3,Ten=4,Nine=5,Eight=6,Seven=7};
}

using namespace CardDeck;

/**
 * The view object for the game.
 * This class servers as a container for the actual
 * canvas view and the control buttons.
 */
class Deck : public QObject
{
  Q_OBJECT

  public:

    /** Constructor for the main view.
     *  @param seed The random seed
     *  @param parent The parent window
     */
    Deck(long seed, QObject* parent);

    /** Destructor 
    */
    virtual ~Deck();

    /** Load a set of cards from the given path
     *  @param cardPath The path to the card files
     */
    bool loadCards(QString cardPath);

    /** Load the card backside from a given file
      * @param filename The filename to load
      * @return True if the loading suceeded.
      */
    bool loadCardBackside(QString filename);

    /** Load the trump icons a given directory
      * @param filename The directory to load from
      * @return True if the loading suceeded.
      */
    bool loadTrump(QString dir);

    /** Retrieve the backside pixmap
     *  @return The pixmap.
     */
     QPixmap* backsidePixmap();

    /** Retrieve a card pixmap
     *  @param  The card number
     *  @return The pixmap.
     */
     QPixmap* cardPixmap(int no);

     /** Retrieve the size of a card pixmap.
      *  @return Tghe card size.
      */
     QSize cardSize();

     /** Retrieve number of cards in this deck.
       * @return Number of cards.
       */
     int cardNumber();

     /** Shuffle the cards and reset the pile.
      */
     void shuffle();

     /** Draw a random trump from all cards. This is
      *  done by drawing a random card and chosing its
      *  suite as trump. In case a Jack is drawn a Grand
      * is made trump.
      */
     Suite randomTrump(); 

     /** Draw a random card out of the pile of existing ones.
      *  @return A random card
      */
      int drawCard();

      /** Get the suite of a given card (number)
       *  @param card The card number
       *  @return The suite.
       */
      static Suite getSuite(int card);

      /** Get the card type (ace, king, ...) of a given card (number)
       *  @param card The card number
       *  @return The card type (ace, king, ...).
       */
      static CardType getCardType(int card);

      /** Get the value in points of the given card (number).
       *  @param card The card number
       *  @return The card value (0, 2, 3, 4, 10, 11).
       */
      static int getCardValue(int card);

      /** Returns a verbose name for a card
       *  @param suite The card suite
       *  @param type  The card type
       *  @return A descriptive string.
       */
      static QString name(Suite suite, CardType type);

      /** Returns a verbose name for a card
       *  @param card The card as integer
       *  @return A descriptive string.
       */
      static QString name(int card);

      /** Retrive the pixmap icon belonging to trump
       *  @param trump The trump suite
       *  @return The pixmap belonging to the trump.
       */
      QPixmap* trumpIcon(Suite trump);

  public slots:

  private:
    // Card pixmaps
    QList<QPixmap*> mCardPixmaps;
    // Suite pixmaps
    QHash<Suite,QPixmap*> mTrumpIcons;
    // Suite (card backside) pixmap
    QPixmap* mCardBackside;
    // Maximum size of a card in pixel
    QSize maxCardSize;
    // Array of card numbers
    QList<int> mCards;
    // Random generator
    KRandomSequence mRandom;
};

#endif
