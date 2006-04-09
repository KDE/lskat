// Qt includes
#include <QPixmap>

// KDE includes
#include <kdebug.h>

// Local includes
#include "mouseinput.h"


// Constructor for the engine
MouseInput::MouseInput(QString dir, QObject* parent)
    : AbstractInput(parent)
{
  QString filename = dir+ QString("mouseinput.png");
  mInputIcon = new QPixmap();
  if (!mInputIcon->load(filename))
  {
    kdError() << "Cannot load file " << filename << endl;
  }
}

// Mouse press received
void MouseInput::mousePress(QPoint point)
{
  int playerNumber;
  int cardNumber;

  // Do only process input if it is our turn
  if (!mInputAllowed) return;
  
  emit signalConvertMousePress(point, playerNumber, cardNumber);
  // Check whether the move is for the right player
  if (playerNumber == mId)
  {
    kdDebug() << "MouseInput:: Convert to " << playerNumber << ", " << cardNumber << endl;
    emit signalPlayerInput(mId, playerNumber, cardNumber);
  }
}


