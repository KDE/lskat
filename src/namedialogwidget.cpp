// Qt includes
#include <QWidget>

// KDE includes
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "namedialogwidget.h"

// Constructor for the view
NameDialogWidget::NameDialogWidget(QWidget* parent)
                  : QDialog(parent)
{
  ui.setupUi(this);
}

// Set a default names to the widget
void NameDialogWidget::setName(int pos, QString name)
{
  if (pos == 0)
  {
    ui.editPlayer1->setText(name);
  }
  else
  {
    ui.editPlayer2->setText(name);
  }
}

// Retrieve the name from the widget.
QString NameDialogWidget::name(int pos)
{
  if (pos == 0)
  {
    return ui.editPlayer1->text();
  }
  else
  {
    return ui.editPlayer2->text();
  }
}



