/***************************************************************************
                          movielabel.h  -  description
                             -------------------
    begin                : November 2000
    copyright            : (C) 2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

#ifndef MOVIELABEL_H
#define MOVIELABEL_H

#include <qwidget.h>
#include <qlabel.h>
#include <qasyncio.h>

class EMovie : public QObject
{
  Q_OBJECT
  public:
  QPixmap *frameArray;
  unsigned short frameCnt;
  unsigned short frameDelay;
};
class QMovieLabel : public QLabel
{
  Q_OBJECT
  public:
  QMovieLabel(QWidget *parent,const char *name=0);
  ~QMovieLabel();
  void paintEvent( QPaintEvent * p);
  void showEvent(QShowEvent *event);
  void Paint(QPainter *p);
  void setEMovie(EMovie *movie);
  public slots:
  void slotTimer(void);
  protected:
  EMovie *mMovie;
  QTimer *mTimer;
  unsigned short cnt;
};


#endif 
