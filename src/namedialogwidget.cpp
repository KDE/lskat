/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "namedialogwidget.h"

// Qt includes
#include <QWidget>

// KF includes

// Constructor for the widget
NameDialogWidget::NameDialogWidget(QWidget *parent)
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
