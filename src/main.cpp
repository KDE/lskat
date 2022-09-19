/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later

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
#include "lskat_version.h"
#include "lskatglobal.h"
#include "mainwindow.h"

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>


// Debug level for the program
int global_debug = 0;
// Skip intro
bool global_skip_intro = false;
// Demo (autoplay mode)
bool global_demo_mode  = false;

int main(int argc, char *argv[])
{
    global_debug = 0;
    // Fixes blurry icons with fractional scaling
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication application(argc, argv);
    KLocalizedString::setApplicationDomain("lskat");

    KAboutData aboutData(QStringLiteral("lskat"), i18n("LSkat"),
                          QStringLiteral(LSKAT_VERSION_STRING),
                          i18n("LSkat: A desktop card game"),
                          KAboutLicense::GPL,
                          i18n("(c) 1995-2007, Martin Heni"),
                          QString(),
                          QStringLiteral("https://apps.kde.org/lskat"));

    // I18N: These are the same strings as in kwin4, you can copy the translations
    aboutData.addAuthor(i18n("Martin Heni"), i18n("Game design and code"), QStringLiteral("kde@heni-online.de"));
    aboutData.addAuthor(i18n("Eugene Trounev"), i18n("Graphics"), QStringLiteral("eugene.trounev@gmail.com"));
    // end I18N
    aboutData.addAuthor(i18n("Benjamin Meyer"), i18n("Code Improvements"));
    // 'Thanks to' aboutData.addCredit(i18n("KDE"), i18n("KDE"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);

    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("d") << QStringLiteral("debug"), i18n("Enter debug level"), QStringLiteral("level")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("skipintro"), i18n("Skip intro animation")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("demo"), i18n("Run game in demo (autoplay) mode")));

    aboutData.setupCommandLine(&parser);
    parser.process(application);
    aboutData.processCommandLine(&parser);

    /* command line handling */

    KCrash::initialize();

    // Check for debug command line option
    if (parser.isSet(QStringLiteral("debug")))
    {
        global_debug = QString(parser.value(QStringLiteral("debug"))).toInt();
        qCDebug(LSKAT_LOG) << "Debug level set to" << global_debug;
    }
    // Check for debug command line option
    if (parser.isSet(QStringLiteral("skipintro")))
    {
        global_skip_intro = true;
        qCDebug(LSKAT_LOG) << "Skip intro cmd line chosen" << global_skip_intro;
    }
    // Check for debug command line option
    if (parser.isSet(QStringLiteral("demo")))
    {
        global_demo_mode = true;
        qCDebug(LSKAT_LOG) << "Running in demo mode" << global_demo_mode;
    }

    if (application.isSessionRestored())
    {
        kRestoreMainWindows<Mainwindow>();
    }
    else
    {
        Mainwindow *mainwindow = new Mainwindow();
        mainwindow->show();
    }

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("lskat")));

    return application.exec();
}
