/***************************************************************************
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
 ***************************************************************************/

/** \mainpage LSkat API Documentation
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
  *
  * 
  */

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kglobal.h>

#include "mainwindow.h"
#include "lskatglobal.h"

#define LSKAT_VERSION "v1.40"


// Debug level for the program
int global_debug = 0;
// Skip intro
bool global_skip_intro = false;
// Demo (autoplay mode)
bool global_demo_mode  = false;


int main(int argc, char *argv[])
{
  global_debug=0;
  KAboutData aboutData( "lskat", 0, ki18n("LSkat"),
                        LSKAT_VERSION,
                        ki18n("Lskat: A desktop card game"),
                        KAboutData::License_GPL,
                        ki18n("(c) 1995-2007, Martin Heni"));
  aboutData.addAuthor(ki18n("Martin Heni"),KLocalizedString(), "kde@heni-online.de");
  aboutData.addCredit(ki18n("KDE"), ki18n("KDE"));
  aboutData.addAuthor(ki18n("Benjamin Meyer"), ki18n("Code Improvements"));
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("d");
  options.add("debug <level>", ki18n("Enter debug level"));
  options.add("skipintro", ki18n("Skip intro animation"));
  options.add("demo", ki18n("Run game in demo (autoplay) mode"));
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();


  // Check for debug command line option
  if (args->isSet("debug"))
  {
    global_debug=QString(args->getOption("debug")).toInt();
    kDebug(12010) << "Debug level set to" << global_debug;
  }
  // Check for debug command line option
  if (args->isSet("skipintro"))
  {
    global_skip_intro = true;
    kDebug(12010) << "Skip intro cmd line chosen" << global_skip_intro;
  }
  // Check for debug command line option
  if (args->isSet("demo"))
  {
    global_demo_mode = true;
    kDebug(12010) << "Running in demo mode" << global_demo_mode;
  }
  args->clear();  
  KApplication application(true);
  KGlobal::locale()->insertCatalog("libkdegames");

  if (application.isSessionRestored())
  {
    RESTORE(Mainwindow);
  }
  else
  {
    Mainwindow *mainwindow = new Mainwindow();
    mainwindow->show();
  }

  return application.exec();
}

