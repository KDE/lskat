#ifndef PLAYER_H
#define PLAYER_H

// Qt includes
#include <QObject>
#include <QVector>

// KDE includes
#include <kdebug.h>
#include <kconfig.h>

// Local includes
#include "deck.h"

// Forward declaration
class AbstractInput;

using namespace CardDeck;


/**
 * The player of the game.
 */
class Player : public QObject
{
  Q_OBJECT

  public:
    /** Constructor for the player
     *  @param parent The parent object
     */
    Player(int id, QObject* parent=0);

    /** Destructor 
     */
     virtual ~Player();

    /** Retrive card value at given logical position
     *  @param playerNumber (0..max number of cards of player)
     *  @return Card number
     */
    int getCard(int playerNumber);

    /** Remove a card from the given position. Typically if the
     *  card was played.
     *  @param cardPosition (0..max number of cards of player)
     */
    void deleteCard(int cardPosition);

    /** Add a card to the given position. Typically if the
     *  card was played back.
     *  @param cardPosition (0..max number of cards of player)
     *  @param card The actual card
     */
    void addCard(int cardPosition, int card);

    /** Set this player to start a turn 
    */
    void startTurn();

    /** Set this player to stop a turn 
    */
    void stopTurn();
    
    /** Deal a number of cards to this player 
     *  @param amount The amount of cards
     */
    void deal(int amount); 

    /** Increases the numebr of moves one for this player.
     *  @param amount Increase by this amount (default 1)
     */
    void increaseMovesWon(int amount = 1);

    /** Retrive the number of won moves for this player.
     * @return The amount of won moves for this player.
     */
    int noOfMovesWon();
    
    /** Adds a card which is one in a move to this player.
     *  Also  increase this players point by the points
     *  of the card. Note, this method has to be called with both
     *  cards of a move!
     * @param card The card to store
     */
    void addWonCard(int card);

    /** Retrieve a card won by this player.
     *  @param no The number of the card. Can be 0..2*noOfMovesWon().
     *  @return The card.
     */
    int getWonCard(int no);

    /** Retrive the amount of points this player has.
     *  @return The amount of points (0-120).
     */
    int points();

    /** Set the points of the player.
     *  Emits the signal signalUpdatePoints(int)
     *  @param points The points.
     */
    void setPoints(int points);
    
    /** Retrive the name of the player.
     *  @return The player's name.
     */
    QString name();

    /** Set the name of the player.
     *  Emits the signal signalUpdateName(int)
     *  @param name The new name.
     */
    void setName(QString name);

    /** Add a number of won games to the over all 
      * player statistic.
      * @param amount The amount of won games
      */
    void addWonGame(int amount);

    /** Add a number of games to the over all 
      * player statistic.
      * @param amount The amount of games
      */
    void addGame(int amount);

    /** Add a score to the over all 
     *  player statistic.
     *  @param amount The score amount
     */
    void addScore(int amount);

    /** Retrieve the input device of the player 
     *  @return The input device.
     */
    AbstractInput* input();

    /** Set the input for this player.
     *  @param input The input device.
     */
     void setInput(AbstractInput* input);

     /** Set the deck for drawing cards.
      *  @param deck The deck
      */
     void setDeck(Deck* deck); 

     /** Refresh GUI by emitting all signals.
     */
     void refresh();

     /** Clear the all time statistics of this player.
      */
     void clear(); 

     /** Set the trump to the player 
      *  @param trump The trump suite
      */
     void setTrump(Suite trump); 

     /** Saves the properties 
      * @param cfg The config object.
      */
     void save(KConfig *cfg);

     /** Read properties.
      * @param cfg The config object.
      */
     void load(KConfig *cfg);

  signals:
    /** Notify change of points.
     *  @param points The new points.
     */
    void signalUpdatePoints(int points);

    /** Notify change of name.
     *  @param name The new name.
     */
    void signalUpdateName(QString name);

    /** Notify change of games.
     *  @param amountWon The amount of won games.
     *  @param amountGames The total amount of games.
     */
    void signalUpdateGames(int amountWon, int amountGames);

    /** Notify change of score.
     *  @param amount The new score.
     */
    void signalUpdateScore(int score);

    /** Notify change of input device.
     *  @param amount The new input.
     */
    void signalUpdateInput(QPixmap* icon);

    /** Notify change of trump icon
     *  @param amount The new trump icon.
     */
    void signalUpdateTrump(QPixmap* icon);

  private:
    /** Players input device */
    AbstractInput* mInput;
    /** Card deck */
    Deck* mDeck;
    /** Our cards */
    QVector<int> mCards;
    /** Our won cards */
    QList<int>   mWonCards;
    /** Our points */
    int mPoints;
    /** Our id */
    int mId;
    /** Number of moves won in the current game */
    int mMovesWon;
    /** The current trump */
    Suite mTrump;

    // These quantities need to be saved
    /** Our name */
    QString mName;
    /** Overall games won for this player */
    int mGamesWon;
    /** Overall score for this player */
    int mScore;
    /** Overall number of games for this player */
    int mNumberOfGames;
};

#endif
