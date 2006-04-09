#ifndef ABSTRACT_INPUT_H
#define ABSTRACT_INPUT_H

// Qt includes
#include <QObject>

// KDE includes
#include <kdebug.h>

// local includes

// Forward declaration
class QPixmap;

namespace InputDevice
{
    /** Determine the type of input to use for the player */
    enum InputDeviceType {TypeMouseInput = 0, TypeAiInput = 1};
}

/**
 * Abstract input device
 */
class AbstractInput : public QObject
{
  Q_OBJECT

  public:

    /** Constructor for the input
     *  @param parent The parent object
     */
    AbstractInput(QObject* parent);

    /** Allow or disallow input with this device 
      * @param allowed True if input is allowed
      */
    virtual void setInputAllowed(bool allowed);  

    /** Retrieve whether input is allowed or not.
     *  @return True if input from this device is allowed.
     */
    virtual bool inputAllowed();

    /** Set the player id number to whom this input
      * belongs to.
      * @param id The id number
      */
    virtual void setId(int id);

    /** Retrieve the icon associated with this device 
     *  @return The input icon.
     */
    QPixmap* inputIcon() {return mInputIcon;}

  signals:
    /** Signals the availability of player input
    */
   void signalPlayerInput(int id, int position, int number);

  protected:
    /** Player id to whom this input belongs */
    int mId;
    /** Is input allowed ? */
    bool mInputAllowed;
    /** The input icon */
    QPixmap* mInputIcon;

};

#endif
