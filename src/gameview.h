#ifndef GAMEVIEW_H
#define GAMEVIEW_H

// Qt includes
#include <QWidget>
#include <QPushButton>
#include <QSize>
#include <QHash>
#include <QGridLayout>

// KDE includes
#include <kdebug.h>

// Local includes
#include "canvasview.h"

// Forward declaration
class Deck;
class Q3Canvas;
class PlayerStatusWidget;


/**
 * The view object for the game.
 * This class servers as a container for the actual
 * canvas view and the control buttons.
 */
class GameView : public QWidget
{
  Q_OBJECT

  public:
    /** Constructor for the main view.
     *  @param size The canvas view's fixed size
     *  @param advancePeriod The canvas advance period
     *  @param parent The parent window
     */
    GameView(QSize size, int advancePeriod, QWidget* parent = 0);
    
    /** Destructor
    */
    virtual ~GameView();

    /** Retrive the canvas view.
      * @return The canvas view.
      */
    CanvasView* canvasView() {return mCanvasView;}

    /** Retrive the canvas.
      * @return The canvas.
      */
    Q3Canvas* canvas() {return mCanvasView->canvas();}

    /** Retrive the status widget of a player 
     *  @param pos The widget position number (0,1,...)
     *  @return The widget.
     */
    PlayerStatusWidget* statusWidget(int pos);

    /** Store and display a new status widget at the given position.
     *  Old widgets on this position are removed and destroyed.
     *  @param pos The position on the screen (0,1,2)
     *  @param widget The new widget
     */
    void setStatusWidget(int pos, PlayerStatusWidget* widget);

  public slots:

  private:
    // The canvas view
    CanvasView* mCanvasView;
    // The status widgets
    QHash<int,PlayerStatusWidget*> mPlayerWidgets;
    // The layout grid
    QGridLayout* mGrid;
};

#endif
