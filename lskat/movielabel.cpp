
//#include <kapp.h>
#include <qpixmap.h>
//#include <qmovie.h>
#include <qpainter.h>
#include "movielabel.h"


QMovieLabel::QMovieLabel(QWidget *parent,const char *name) :
  QLabel(parent,name)
{
  mTimer=0;
  cnt=0;
}
// paint function
void QMovieLabel::Paint(QPainter *p)
{
  if (mMovie && mMovie->frameArray)
  {
    QRect r=p->window();
    QPixmap pm(mMovie->frameArray[cnt]);
    if (r.size()!=pm.size())
    {
      QWMatrix m;
      m.scale((double)r.width()/(double)pm.width(),
              (double)r.height()/(double)pm.height());
      pm=pm.xForm(m);
    }
    p->drawPixmap(QPoint(0,0),pm);
  }
}

// paint event
void QMovieLabel::paintEvent( QPaintEvent * )
{
    QPixmap pm(this->rect().size());
    QPainter p;
    QBrush brush(Qt::black);
    p.begin(&pm,this);
    p.fillRect(0,0,this->rect().width(),this->rect().height(),brush);
    Paint(&p);
    p.end();
    bitBlt(this,0,0,&pm);
}
void QMovieLabel::setEMovie(EMovie *movie)
{
  mMovie=movie;
  if (mMovie)
  {
    mTimer=new QTimer(this);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(slotTimer()));
    // mTimer->start(movie->frameDelay,FALSE);
  }
  cnt=0;
}
void QMovieLabel::slotTimer(void)
{
  cnt++;
  if (mMovie)
  {
    if (cnt>=mMovie->frameCnt) cnt=0;
    if (isVisible()) repaint(false);
    else mTimer->stop();
  }
  else
  {
    mTimer->stop();
  }
}
void QMovieLabel::showEvent(QShowEvent *)
{
  if (mMovie)
  {
    mTimer->start(mMovie->frameDelay,FALSE);
  }
  cnt=0;
}
QMovieLabel::~QMovieLabel()
{
  delete mTimer;
}

#include "movielabel.moc"
