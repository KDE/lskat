/***************************************************************************
                          Lskat
                          -----
    begin                : March 2000 
    copyright            : (C) 1995-2006 by Martin Heni
    email                : kde@heni-online.de
 ***************************************************************************/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "mainwindow.h"

#define LSKAT_VERSION "v1.10"

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
  KAboutData aboutData( "lskat", I18N_NOOP("LSkat"),
                        LSKAT_VERSION,
                        I18N_NOOP("Lskat: A desktop card game"),
                        KAboutData::License_GPL,
                        "(c) 1995-2006, Martin Heni");
  aboutData.addAuthor("Martin Heni",0, "martin@heni-online.de");
  aboutData.addCredit("KDE", I18N_NOOP("KDE"), 0);
  aboutData.addAuthor("Benjamin Meyer", I18N_NOOP("Code Improvements"), 0);
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->isSet("debug"))
  {
   // global_debug=QString(args->getOption("debug")).toInt();
   // kdDebug(12010) << "Debug level set to " << global_debug << endl;
  }
  
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

