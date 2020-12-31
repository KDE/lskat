/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
