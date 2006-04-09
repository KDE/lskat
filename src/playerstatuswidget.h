#ifndef PLAYERSTATUSWIDGET_H
#define PLAYERSTATUSWIDGET_H

// Qt includes
#include <QWidget>
#include <QSize>

// KDE includes
#include <kdebug.h>

// Local includes
#include "playerstatusform.h"

using namespace Ui;

class Player;

/**
 * The player status widget.
 */
class PlayerStatusWidget : public QWidget
{
  Q_OBJECT

  public:
    /** Constructor for the widget.
     *  @param player The parent player
     *  @param parent The parent window
     */
    PlayerStatusWidget(Player* player, QWidget* parent);

    /**
     * Reimplement Qt Size hint for widgets.
     * @return The size hint.
     */
    virtual QSize sizeHint() const; 

  public slots:  
    /** Set the player name into the widget.
     * @param name The player's name
     */
    void setName(QString name);

    /** Set the player points into the widget.
     * @param points The player's points
     */
    void setPoints(int points);

    /** Set the player score into the widget.
     * @param score The player's score
     */
    void setScore(int score);

    /** Set the player won games and overall games 
     * @param wonGames The player's won games
     * @param overallGames The player's overall games
     */
    void setGames(int wonGames, int overallGames);

    /** Set the trump icon into the widget.
     * @param icon The trump icon
     */
    void setTrumpIcon(QPixmap* icon);

    /** Set the input icon into the widget.
     * @param icon The input icon
     */
    void setInputIcon(QPixmap* icon);


  private:
    // The UI widget
    PlayerStatusForm ui;
};

#endif
