#ifndef CANVAS_VIEW_H
#define CANVAS_VIEW_H

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <Q3Canvas>
#include <QSize>
#include <QPoint>
#include <QResizeEvent>


/**
 * The view object which shows the graphics in a
 * canvas view.
 */
class CanvasView : public Q3CanvasView
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param size The canvas fixed size
     *  @param advancePeriod The canvas advance period
     *  @param parent The parent window
     */
    CanvasView(QSize size, int advancePeriod, QWidget* parent = 0);

    /** 
     * Destructor for the canvas view 
     */
    ~CanvasView();

     void mousePressEvent(QMouseEvent *ev);


  public slots:  

  signals:
    void signalLeftMousePress(QPoint point);

  protected:
    /**
     * Will be called by the Qt CanvasView when its contents
     * are resized. We adapt the canvas then.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);

};

#endif
