// Qt includes
#include <qpoint.h>
#include <qfont.h>

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "canvasview.h"



// Constructor for the view
CanvasView::CanvasView(QSize size, int advancePeriod, QWidget* parent)
          : Q3CanvasView(parent)
{
  // We do not need scrolling so switch it off
  setResizePolicy(Q3ScrollView::Manual);
  setVScrollBarMode(AlwaysOff);
  setHScrollBarMode(AlwaysOff);

  // Create a new canvas for this view
  Q3Canvas *canvas=new Q3Canvas(this);

  // Remove flicker by double buffering
  canvas->setDoubleBuffering(true);
  // Update/advance every 25ms
  canvas->setAdvancePeriod(advancePeriod);
  // assign the canvas to the view
  setCanvas(canvas);

  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  setFixedSize(size);
  canvas->resize(this->width(), this->height()); 
}


// Slot called by the framework when the window is
// resized.
void CanvasView::resizeEvent (QResizeEvent* e)
{
  // Adapt the canvas size to the window size
  if (canvas()) canvas()->resize(e->size().width(),
                                 e->size().height());
}

// Standard destructor
CanvasView::~CanvasView()
{
}


// mouse click event
void CanvasView::mousePressEvent(QMouseEvent *ev)
{
  if (ev->button() != Qt::LeftButton) return ;

  QPoint point = ev->pos();
  emit signalLeftMousePress(point);
}
