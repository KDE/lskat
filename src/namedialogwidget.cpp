/*
   This file is part of the KDE games lskat program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "namedialogwidget.h"

// Qt includes
#include <QWidget>

// KDE includes
#include <kdebug.h>


// Constructor for the widget
NameDialogWidget::NameDialogWidget(QWidget* parent)
                  : QDialog(parent)
{
  ui.setupUi(this);
}


// Set a default names to the widget
void NameDialogWidget::setName(int pos, const QString &name)
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


