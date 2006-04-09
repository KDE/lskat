#ifndef NAMEDIALOG_WIDGET_H
#define NAMEDIALOG_WIDGET_H

// Qt includes
#include <QDialog>
#include <QSize>

// KDE includes
#include <kdebug.h>

// Local includes
#include "namedialog.h"

using namespace Ui;

/**
 * The player status widget.
 */
class NameDialogWidget : public QDialog
{
  Q_OBJECT

  public:
    /** Constructor for the widget.
     *  @param parent The parent window
     */
    NameDialogWidget(QWidget* parent);

    /** Set a default names to the widget
     *  @param pos The name position (0,1)
     *  @param name The name to set
     */
    void setName(int pos, QString name);

    /** Retrieve the name from the widget.
     *  @param pos The name position (0,1)
     *  @return The new name.
     */
    QString name(int pos); 


  public slots:  


  private:
    // The UI dialog
    NameDialog ui;
};

#endif
