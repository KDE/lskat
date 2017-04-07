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

                          Lskat
                          -----
    begin                : March 2000
    copyright            : (C) 1995-2007 by Martin Heni
    email                : kde@heni-online.de
*/

/**
 * \mainpage LSkat API Documentation
 *
 * \section intro_sec Introduction
 *
 * This is the API documentation for the KDE game 'lskat'.
 *
 * \section design_sec Design
 *
 * The design diagram shows the dependencies for the key classes of
 * the Lskat program.
 * The coloring of the classes shows roughly their function in the
 * groups (program, document and engine, display, QGraphics).
 *
 * \image html lskatclasses.png "Class diagram for LSkat"
 */



#include "lskat_debug.h"
#include "lskatglobal.h"
#include "mainwindow.h"

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <kglobal.h>
#include <KDebug>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#define LSKAT_VERSION "v1.40"

// Debug level for the program
int global_debug = 0;
// Skip intro
bool global_skip_intro = false;
// Demo (autoplay mode)
bool global_demo_mode  = false;

int main(int argc, char *argv[])
{
    global_debug = 0;

    QApplication application(argc, argv);
    KLocalizedString::setApplicationDomain("lskat");

    KAboutData aboutData("lskat", i18n("LSkat"),
                          LSKAT_VERSION,
                          i18n("LSkat: A desktop card game"),
                          KAboutLicense::GPL,
                          i18n("(c) 1995-2007, Martin Heni"),
                          QString(),
                          QStringLiteral("https://games.kde.org/game.php?game=lskat"));

    // I18N: These are the same strings as in kwin4, you can copy the translations
    aboutData.addAuthor(i18n("Martin Heni"), i18n("Game design and code"), "kde@heni-online.de");
    aboutData.addAuthor(i18n("Eugene Trounev"), i18n("Graphics"), "eugene.trounev@gmail.com");
    // end I18N
    aboutData.addAuthor(i18n("Benjamin Meyer"), i18n("Code Improvements"));
    // 'Thanks to' aboutData.addCredit(i18n("KDE"), i18n("KDE"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();

    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("d") << QLatin1String("debug"), i18n("Enter debug level"), QLatin1String("level")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("skipintro"), i18n("Skip intro animation")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("demo"), i18n("Run game in demo (autoplay) mode")));

    aboutData.setupCommandLine(&parser);
    parser.process(application);
    aboutData.processCommandLine(&parser);

    /* command line handling */

    KCrash::initialize();

    // Check for debug command line option
    if (parser.isSet("debug"))
    {
        global_debug = QString(parser.value("debug")).toInt();
        kDebug(12010) << "Debug level set to" << global_debug;
    }
    // Check for debug command line option
    if (parser.isSet("skipintro"))
    {
        global_skip_intro = true;
        kDebug(12010) << "Skip intro cmd line chosen" << global_skip_intro;
    }
    // Check for debug command line option
    if (parser.isSet("demo"))
    {
        global_demo_mode = true;
        kDebug(12010) << "Running in demo mode" << global_demo_mode;
    }

    if (application.isSessionRestored())
    {
        RESTORE(Mainwindow);
    }
    else
    {
        Mainwindow *mainwindow = new Mainwindow();
        mainwindow->show();
    }

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("lskat")));

    return application.exec();
}
