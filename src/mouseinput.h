#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

// Qt includes
#include <QPoint>

// KDE includes
#include <kdebug.h>

// local includes
#include "abstractinput.h"

// Forward declaration


/**
 * Mouse input device
 */
class MouseInput : public AbstractInput
{
  Q_OBJECT

  public:
    /** Constructor for the input
     *  @param dir  The grafix icon directory
     *  @param parent The parent object
     */
    MouseInput(QString dir, QObject* parent);

  public slots:  
    /** Recevied a mouse press event 
     *  @param x x position
     *  @param y y position
     */
    void mousePress(QPoint point);

  signals:  
    /** Convert mouse coordinate *
    */
    void signalConvertMousePress(QPoint mouse, int& position, int &number);
};

#endif
