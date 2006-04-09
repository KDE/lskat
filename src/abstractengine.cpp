// Qt includes

// KDE includes
#include <kdebug.h>

// Local includes
#include "abstractengine.h"


// Constructor for the game document/engine
AbstractEngine::AbstractEngine(QWidget* parent)
               : QObject(parent)
{
  mGameStatus = Stopped;
  mPlayers.clear();
}


// Add a player to the game 
void AbstractEngine::addPlayer(int no, Player* player)
{
  mPlayers[no] = player;
}

// Retrieve the player of the given number.
Player* AbstractEngine::player(int no)
{
  return mPlayers[no];
}





