// Qt includes
#include <QLabel>
#include <QPalette>

// KDE includes
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "gameview.h"
#include "canvasview.h"
#include "playerstatuswidget.h"

using namespace Ui;

// Constructor for the view
GameView::GameView(QSize size, int advancePeriod, QWidget* parent)
        : QWidget(parent)
{
  // Make a grid layout
  mGrid = new QGridLayout(this);

  // Create and place the canvas view
  mCanvasView = new CanvasView(size, advancePeriod, this);
  mGrid->addWidget(mCanvasView, 0, 0, 3, 3, Qt::AlignLeft|Qt::AlignTop);

  mPlayerWidgets.clear();

  QPalette palette;
  palette.setColor(QPalette::Active, QPalette::Background, QColor(0,0,128));
  palette.setColor(QPalette::Inactive, QPalette::Background, QColor(0,0,128));
  palette.setColor(QPalette::Disabled, QPalette::Background, QColor(0,0,128));
  this->setPalette(palette);

  updateGeometry();
  adjustSize();
}

// Destructor
GameView::~GameView()
{
  // Widgets are destroyed by Qt
}

void GameView::setStatusWidget(int pos, PlayerStatusWidget* widget)
{
  // Remove old widget
  if (mPlayerWidgets.contains(pos))
  {
    PlayerStatusWidget* old = mPlayerWidgets[pos];
    mGrid->removeWidget(old);
    delete old;
  }
  mGrid->addWidget(widget, pos, 3 );
  mPlayerWidgets[pos] = widget;
  updateGeometry();
  adjustSize();
}


// Retrive the status widget of a player 
PlayerStatusWidget* GameView::statusWidget(int pos)
{
  if (!mPlayerWidgets.contains(pos))
  {
    kdFatal() << "Player status widget " << pos << "does not exists" << endl;
    return 0;
  }
  return mPlayerWidgets[pos];
}

