// Qt includes

// KDE includes
#include <kdebug.h>

// Local includes
#include "abstractinput.h"


// Constructor for the engine
AbstractInput::AbstractInput(QObject* parent)
    : QObject(parent)
{
  mInputAllowed = false;
  mId = -1;
}


// Set player id number
void AbstractInput::setId(int id)
{
  mId = id;
}


// Allow or disallow input with this device 
void AbstractInput::setInputAllowed(bool allowed)
{
  mInputAllowed = allowed;
}


// Retrieve whether input is allowed or not.
bool AbstractInput::inputAllowed()
{
  return mInputAllowed;
}
