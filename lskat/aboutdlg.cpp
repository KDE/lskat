/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Sun Apr 16 2000
    copyright            : (C) |1995-2000 by Martin Heni
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
#include "aboutdlg.h"
#include "lskat.h"

aboutDlg::aboutDlg(EMovie *movie,QWidget *parent, const char *name) : QDialog(parent,name,true)
{
  mMovie=movie;
	initDialog();
	setCaption(i18n("Lieutnant Skat"));
  connect( QPushButton_1, SIGNAL(clicked()), SLOT(accept()) );
}

aboutDlg::~aboutDlg(){
}

