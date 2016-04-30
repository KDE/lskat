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

#ifndef NAMEDIALOG_WIDGET_H
#define NAMEDIALOG_WIDGET_H

// Qt includes
#include <QDialog>

// KDE includes
#include "lskat_debug.h"

// Local includes
#include "ui_namedialog.h"

using namespace Ui;

/**
 * The player status widget.
 */
class NameDialogWidget : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor for the widget.
     * @param parent The parent window
     */
    explicit NameDialogWidget(QWidget *parent);

    /**
     * Set a default names to the widget
     * @param pos The name position (0,1)
     * @param name The name to set
     */
    void setName(int pos, const QString &name);

    /**
     * Retrieve the name from the widget.
     * @param pos The name position (0,1)
     * @return The new name.
     */
    QString name(int pos);

private:
    // The UI dialog
    NameDialog ui;
};

#endif
