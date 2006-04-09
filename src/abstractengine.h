#ifndef ABSTGRACT_ENGINE_H
#define ABSTGRACT_ENGINE_H

// Qt includes
#include <QString>
#include <QWidget>
#include <QHash>

// KDE includes
#include <kdebug.h>

#include "deck.h"

// Forward declaration
class Player;

using namespace CardDeck;

/**
 * The document/data object for the game.
 */
class AbstractEngine : public QObject
{
  Q_OBJECT

  public:
    /** Constructor for the game engine.
     *  @param parent The parent window
     */
    AbstractEngine(QWidget* parent);

    /** Stati of the game */
    enum GameStatus {Running, Stopped};

    /** Start a new game.
     *  @param trump The trump suite.
     *  @param startPlayer The start player
     */
    virtual void startGame(Suite trump, int startPlayer) = 0;

    /** Stop a game.
     */
    virtual void stopGame() = 0;

    /** Is the game still running?
     *  @return True if the game is running.
     */
     virtual bool isGameRunning() {return mGameStatus == Running;}

    /** Add a player to the game 
      * @param no Player number
      * @param player Player to add
      */
    virtual void addPlayer(int no, Player* player);

    /** Retreive the player of the given number.
     *  @param no The player number.
     *  @return The player.
     */
    virtual Player* player(int no);

    /** Retrieve the current player id 
     *  @return The current player.
     */
   virtual int currentPlayer() {return mCurrentPlayer;}  

  public slots:

    /** Player input available 
    */
    virtual void playerInput(int id, int position, int number) = 0;


  protected:
    /** Current game status */
    GameStatus mGameStatus;
    /** Whose turn is it */
    int mCurrentPlayer;

    /** Player in the game */
    QHash<int, Player*> mPlayers;
    
};

#endif
