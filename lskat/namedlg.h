/***************************************************************************
                          Lskat
                           -------------------
    begin                : March 2000 
    copyright            : (C) 2000 by Martin Heni
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
#ifndef __NAMEDLG_H_
#define __NAMEDLG_H_
#include <qlined.h>
//#include <ktabctl.h>
//#include <qkeycode.h>
//#include <qmlined.h>
//#include <unistd.h>
#include <kapp.h>
//#include <qlabel.h>
//#include <qtooltip.h>
//#include <kmsgbox.h>
//#include <ktopwidget.h>
//nclude <kprocess.h>
//#include <kstdaccel.h>
#include <qstring.h>
#include <qdialog.h>

class NameDlg : public QDialog
{
  Q_OBJECT

  public:
   NameDlg (QWidget* parent = NULL,const char* name = NULL,const char *sufi=NULL);
   void SetNames(QString n1,QString n2);
   void GetNames(QString &n1,QString &n2);

protected slots:

protected:
    KConfig *config;
    KApplication *app;

    QLineEdit* NameEdit1;
    QLineEdit* NameEdit2;

};
#endif
