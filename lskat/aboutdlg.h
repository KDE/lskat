/***************************************************************************
                          About dialog
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ABOUTDLG_H
#define _ABOUTDLG_H

#include <stdio.h>
//Generated area. DO NOT EDIT!!!(begin)
#include <qwidget.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Generated area. DO NOT EDIT!!!(end)
#include <klocale.h>
#include <qdialog.h>
#include <qasyncio.h>

/**
  *@author Martin Heni
  */

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

class aboutDlg : public QDialog  {
   Q_OBJECT
public: 
	aboutDlg(EMovie *movie,QWidget *parent=0, const char *name=0);
	~aboutDlg();

protected: 
	void initDialog();
	//Generated area. DO NOT EDIT!!!(begin)
	QGroupBox *QGroupBox_1;
	QLabel *QLabel_1;
	QLabel *QLabel_7;
	QMovieLabel *QLabel_2;
	QLabel *QLabel_8;
	QLabel *QLabel_5;
	QLabel *QLabel_9;
	QLabel *QLabel_11;
	QLabel *QLabel_10;
	QPushButton *QPushButton_1;
	//Generated area. DO NOT EDIT!!!(end)

private: 
  EMovie *mMovie;
};


#endif
