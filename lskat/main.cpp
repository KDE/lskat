/***************************************************************************
    begin                : Tue May  2 15:47:11 CEST 2000
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
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>
#include "lskat.h"

static KCmdLineOptions options[] =
{
  { "d", 0, 0},
  { "debug <level>", I18N_NOOP("Enter debug level"), 0 },
  KCmdLineLastOption
};

int global_debug;

int main(int argc, char *argv[])
{

  global_debug=0;
  KAboutData aboutData( "lskat", I18N_NOOP("Lieutenant Skat"),
                        "1.0",
                        I18N_NOOP("Card Game"),
                        KAboutData::License_GPL,
                        "(c) 2000, Martin Heni");
  aboutData.addAuthor("Martin Heni",0, "martin@heni-online.de");
  aboutData.addCredit("Laura", I18N_NOOP("Beta testing"), 0);
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->isSet("debug"))
  {
    QString s = args->getOption("debug");
    global_debug = s.toInt();
    qDebug("Debug level set to %d\n", global_debug);
  }
  args->clear();

  KApplication app;

  if (app.isRestored())
  {
    RESTORE(LSkatApp);
  }
  else
  {
    LSkatApp *lskat = new LSkatApp();
    lskat->show();
  }

  return app.exec();
}
