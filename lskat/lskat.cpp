/*
                          lskat.cpp  -  description
                             -------------------
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

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qprogressdialog.h>

// include files for KDE
#include <kstddirs.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <khelpmenu.h>

// application specific includes
#include "lskat.h"
#include "lskatview.h"
#include "lskatdoc.h"
#include "resource.h"
#include "namedlg.h"
#include "networkdlg.h"
#include "aboutdlg.h"
#include "msgdlg.h"
#include <kcarddialog.h>


#include <stdlib.h>

LSkatApp::LSkatApp() : KMainWindow(0)
{
  config=kapp->config();

   // localise data file
   QString file=QString::fromLatin1("lskat/grafix/1.png");
   mGrafix=kapp->dirs()->findResourceDir("data", file); 
   if (mGrafix.isNull()) mGrafix = QCString("grafix/");
   else mGrafix+=QCString("lskat/grafix/");
   if (global_debug>3) printf("Localised datafile=%s\n",mGrafix.latin1());


  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initMenuBar();
#ifdef USE_TOOLBAR
  initToolBar();
#endif
  initStatusBar();
  initKeyAccel();
  initDocument();
  initView();
	
  readOptions();
  // Needs to be after readOptions as we read in default paths
  doc->LoadGrafix(mGrafix);

  mInput=new KEInput(this);
  doc->SetInputHandler(mInput);
  connect(mInput,SIGNAL(signalPrepareProcessMove(KEMessage *)),
          this,SLOT(slotPrepareProcessMove(KEMessage *)));
  connect(mInput,SIGNAL(signalPrepareRemoteMove(KEMessage *)),
          this,SLOT(slotPrepareRemoteMove(KEMessage *)));
  connect(mInput,SIGNAL(signalPrepareInteractiveMove(KEMessage *)),
          this,SLOT(slotPrepareInteractiveMove(KEMessage *)));
  connect(mInput,SIGNAL(signalReceiveInput(KEMessage *, int)),
          this,SLOT(slotReceiveInput(KEMessage *,int )));

  setMinimumSize(640,480);   
  setMaximumSize(800,600);
  resize( 640, 480 );

}

LSkatApp::~LSkatApp()
{
 delete mInput;
}

void LSkatApp::initKeyAccel()
{
  keyAccel = new KAccel(this);

	
  // fileMenu accelerators
  keyAccel->insertItem( i18n("Start Game"), QCString("StartGame"), Key_F2 );
  keyAccel->connectItem(QCString("StartGame"), this, SLOT(slotFileNew()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_NEW, QCString("StartGame"));
  fileMenu->setAccel( Key_F2, ID_FILE_NEW);

  keyAccel->connectItem(KStdAccel::Close, this, SLOT(slotFileClose()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_CLOSE, KStdAccel::Close);
  fileMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Close)), ID_FILE_CLOSE);

  keyAccel->connectItem(KStdAccel::Quit, this, SLOT(slotFileQuit()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_QUIT, KStdAccel::Quit);
  fileMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Quit)), ID_FILE_QUIT);

  keyAccel->insertItem( i18n("Send Message"), QCString("Send Message"), CTRL+Key_M );
  keyAccel->connectItem(QCString("Send Message"), this, SLOT(slotFileMessage()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_MESSAGE, QCString("Send Message") );
  fileMenu->setAccel( CTRL + Key_M, ID_FILE_MESSAGE);

  // editMenu accelerators
  keyAccel->connectItem(KStdAccel::Undo, this, SLOT(slotEditUndo()));
  keyAccel->changeMenuAccel(editMenu, ID_EDIT_UNDO, KStdAccel::Undo );
  editMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Undo)), ID_EDIT_UNDO);

  keyAccel->connectItem(KStdAccel::Redo, this, SLOT(slotEditRedo()));
  keyAccel->changeMenuAccel(editMenu, ID_EDIT_REDO, KStdAccel::Redo );
  editMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Redo)), ID_EDIT_REDO);

  keyAccel->connectItem(KStdAccel::Help, this, SLOT(appHelpActivated()));
  			
  keyAccel->readSettings();	
}

void LSkatApp::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR
  fileMenu = new QPopupMenu();
//  fileMenu->insertSeparator();
  fileMenu->insertItem(SmallIcon("filenew"), i18n("&New game"), ID_FILE_NEW);
  fileMenu->insertItem(SmallIcon("stop"),i18n("&End game"), ID_FILE_CLOSE);
  fileMenu->insertSeparator();
  fileMenu->insertItem(SmallIcon("flag"),i18n("&Clear statistics..."), ID_FILE_STATISTICS);
  fileMenu->insertItem(SmallIcon("openterm"),i18n("Send &Message..."), ID_FILE_MESSAGE);
  fileMenu->insertSeparator();
  fileMenu->insertSeparator();
  fileMenu->insertItem(SmallIcon("exit"),i18n("E&xit"), ID_FILE_QUIT);

	
  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu = new QPopupMenu();
  editMenu->insertItem(SmallIcon("undo"), i18n("&Undo move"), ID_EDIT_UNDO);
  editMenu->insertItem(SmallIcon("redo"), i18n("&Redo move"), ID_EDIT_REDO);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu = new QPopupMenu();
  viewMenu->setCheckable(true);
#ifdef USE_TOOLBAR
  viewMenu->insertItem(i18n("&Toolbar"), ID_VIEW_TOOLBAR);
#endif
  viewMenu->insertItem(i18n("&Statusbar"), ID_VIEW_STATUSBAR);


  ///////////////////////////////////////////////////////////////////
  // menuBar entry optionsMenu
    QPopupMenu *optionsMenu = new QPopupMenu;

    popStartplayer = new QPopupMenu;
      popStartplayer->setCheckable(TRUE);

      popStartplayer->insertItem(i18n("Player &1"),ID_STARTPLAYER_1);
      popStartplayer->insertItem(i18n("Player &2"),ID_STARTPLAYER_2);

    popPlayer1 = new QPopupMenu;
      popPlayer1->setCheckable(TRUE);

      popPlayer1->insertItem(i18n("&Player"),ID_PLAYER1_PLAYER);
      popPlayer1->insertItem(i18n("&Computer"),ID_PLAYER1_COMPUTER);
      popPlayer1->insertItem(i18n("&Remote"),ID_PLAYER1_REMOTE);

    popPlayer2 = new QPopupMenu;
      popPlayer2->setCheckable(TRUE);

      popPlayer2->insertItem(i18n("&Player"),ID_PLAYER2_PLAYER);
      popPlayer2->insertItem(i18n("&Computer"),ID_PLAYER2_COMPUTER);
      popPlayer2->insertItem(i18n("&Remote"),ID_PLAYER2_REMOTE);


    popLevel = new QPopupMenu;
      popLevel->setCheckable(TRUE);

      popLevel->insertItem(i18n("&normal"),ID_LEVEL_1);
      popLevel->insertItem(i18n("&advanced"),ID_LEVEL_2);
      popLevel->insertItem(i18n("&hard"),ID_LEVEL_3);
      popLevel->insertSeparator();
      popLevel->insertItem(i18n("&experimental"),ID_LEVEL_4);
      /*
      popLevel->insertItem(i18n("&5"),ID_LEVEL_5);
      popLevel->insertItem(i18n("&6"),ID_LEVEL_6);
      popLevel->insertItem(i18n("&7"),ID_LEVEL_7);
      popLevel->insertItem(i18n("&8"),ID_LEVEL_8);
      popLevel->insertItem(i18n("&9"),ID_LEVEL_9);
      popLevel->insertItem(i18n("&10"),ID_LEVEL_10);
      */

      /*
    popDeck = new QPopupMenu;
      popDeck->setCheckable(TRUE);

      for (int i=0;i<NO_OF_DECKS;i++)
        popDeck->insertItem(i18n("Deck &%1").arg(i+1),ID_DECK_1+i);
        */

      optionsMenu->insertItem(i18n("&Startplayer"),popStartplayer);
      optionsMenu->insertItem(i18n("Player &1 played by"),popPlayer1);
      optionsMenu->insertItem(i18n("Player &2 played by"),popPlayer2);
      optionsMenu->insertItem(i18n("&Level"),popLevel);
//      optionsMenu->insertItem(i18n("Select &carddeck"),popDeck);
      optionsMenu->insertItem(i18n("Select &carddeck..."),ID_OPTIONS_CARDDECK);
      optionsMenu->insertItem(i18n("Change &Names..."),ID_OPTIONS_NAMES);
 

  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
 KHelpMenu *mHelpMenu = new KHelpMenu( this,0,false );
 helpMenu_=mHelpMenu->menu();

 //helpMenu_ = helpMenu();
 connect( mHelpMenu, SIGNAL(showAboutApplication()),
      this, SLOT(slotHelpAbout()));

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  // insert your popup menus with the according menubar entries in the order
  // they will appear later from left to right
  menuBar()->insertItem(i18n("&File"), fileMenu);
//  menuBar()->insertItem(i18n("&Edit"), editMenu);
  menuBar()->insertItem(i18n("&View"), viewMenu);
  menuBar()->insertItem(i18n("&Options"), optionsMenu);

  menuBar()->insertSeparator();
  menuBar()->insertItem(i18n("&Help"), helpMenu_);

  ///////////////////////////////////////////////////////////////////
  // CONNECT THE MENU SLOTS WITH SIGNALS
  // for execution slots and statusbar messages

  connect(fileMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(fileMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(fileMenu, SIGNAL(aboutToShow()), SLOT(slotFileToShow()));

  connect(editMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(editMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(editMenu, SIGNAL(aboutToShow()), SLOT(slotEditToShow()));

  connect(viewMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(viewMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));

  connect(optionsMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(optionsMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(optionsMenu, SIGNAL(aboutToShow()), SLOT(slotOptionsToShow()));

  connect(popStartplayer, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popStartplayer, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popStartplayer, SIGNAL(aboutToShow()), SLOT(slotStartplayerToShow()));

  connect(popPlayer1, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popPlayer1, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popPlayer1, SIGNAL(aboutToShow()), SLOT(slotPlayer1ToShow()));

  connect(popPlayer2, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popPlayer2, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popPlayer2, SIGNAL(aboutToShow()), SLOT(slotPlayer2ToShow()));

  connect(popLevel, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popLevel, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popLevel, SIGNAL(aboutToShow()), SLOT(slotLevelToShow()));

  /*
  connect(popDeck, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popDeck, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popDeck, SIGNAL(aboutToShow()), SLOT(slotDeckToShow()));
  */

}

#ifdef USE_TOOLBAR
void LSkatApp::initToolBar()
{

  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  toolBar()->insertButton(BarIcon("filenew"), ID_FILE_NEW, true, i18n("New File"));
  toolBar()->insertButton(BarIcon("fileopen"), ID_FILE_OPEN, true, i18n("Open File"));
  toolBar()->insertButton(BarIcon("filesave"), ID_FILE_SAVE, true, i18n("Save File"));
  toolBar()->insertButton(BarIcon("fileprint"), ID_FILE_PRINT, true, i18n("Print"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(BarIcon("editcut"), ID_EDIT_CUT, true, i18n("Cut"));
  toolBar()->insertButton(BarIcon("editcopy"), ID_EDIT_COPY, true, i18n("Copy"));
  toolBar()->insertButton(BarIcon("editpaste"), ID_EDIT_PASTE, true, i18n("Paste"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(BarIcon("help"), ID_HELP_CONTENTS, SIGNAL(clicked()),
  				this, SLOT(appHelpActivated()), true,i18n("Help"));

  ///////////////////////////////////////////////////////////////////
  // INSERT YOUR APPLICATION SPECIFIC TOOLBARS HERE WITH toolBar(n)


  ///////////////////////////////////////////////////////////////////
  // CONNECT THE TOOLBAR SLOTS WITH SIGNALS - add new created toolbars by their according number
  // connect for invoking the slot actions
  connect(toolBar(), SIGNAL(clicked(int)), SLOT(commandCallback(int)));
  // connect for the status help on holing icons pressed with the mouse button
  connect(toolBar(), SIGNAL(pressed(int)), SLOT(statusCallback(int)));
}
#endif

void LSkatApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
 // statusBar()->setInsertOrder(KStatusBar::RightToLeft);
  statusBar()->insertItem(i18n("This leaves space for the mover"),ID_STATUS_MOVER,0,true);
  statusBar()->insertItem(i18n("23:45"),ID_STATUS_TIME,0,true);
  statusBar()->insertItem(i18n("Ready"), ID_STATUS_MSG);

  slotStatusTime();
  slotStatusMover(i18n("(c) Martin Heni   "));
  slotStatusMsg(i18n("Welcome to Lieutnant Skat"));

  // status bar clock
  statusTimer=new QTimer(this);
  connect(statusTimer,SIGNAL(timeout()),this,SLOT(slotStatusTimer()));
  statusTimer->start(10000,false);
  // computer move timer
  procTimer=new QTimer(this);
  connect(procTimer,SIGNAL(timeout()),this,SLOT(slotProcTimer()));
}

void LSkatApp::initDocument()
{
  doc = new LSkatDoc(this);
  // TODO check for return false !!!
  doc->newDocument(config,mGrafix);
}

void LSkatApp::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new LSkatView(this);
  doc->addView(view);
  setCentralWidget(view);	
  setCaption(i18n("Lieutnant Skat"));

}

void LSkatApp::enableCommand(int id_)
{
  ///////////////////////////////////////////////////////////////////
  // enable menu and toolbar functions by their ID's
  menuBar()->setItemEnabled(id_, true);
#ifdef USE_TOOLBAR
  toolBar()->setItemEnabled(id_, true);
#endif
}

void LSkatApp::disableCommand(int id_)
{
  ///////////////////////////////////////////////////////////////////
  // disable menu and toolbar functions by their ID's
  menuBar()->setItemEnabled(id_, false);
#ifdef USE_TOOLBAR
  toolBar()->setItemEnabled(id_, false);
#endif
}


LSkatDoc *LSkatApp::getDocument() const
{
  return doc;
}

void LSkatApp::saveOptions()
{	
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
#ifdef USE_TOOLBAR
  config->writeEntry("Show Toolbar", toolBar()->isVisible());
  config->writeEntry("ToolBarPos", (int) toolBar()->barPos());
  config->writeEntry("Recent Files", recentFiles);
#endif
  config->writeEntry("Show Statusbar",statusBar()->isVisible());
  doc->WriteConfig(config);
}


void LSkatApp::readOptions()
{
	
  config->setGroup("General Options");

#ifdef USE_TOOLBAR
  // bar status settings
  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
  viewMenu->setItemChecked(ID_VIEW_TOOLBAR, bViewToolbar);
  if(!bViewToolbar)
    toolBar()->hide();
#endif	
  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  viewMenu->setItemChecked(ID_VIEW_STATUSBAR, bViewStatusbar);
  if(!bViewStatusbar)
    statusBar()->hide();

#ifdef USE_TOOLBAR
  // bar position settings
  KToolBar::BarPosition toolBarPos;
  toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
  toolBar()->setBarPos(toolBarPos);
#endif	

  QSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }
  doc->ReadConfig(config);
}

void LSkatApp::saveProperties(KConfig *_cfg)
{
  if(doc->getTitle()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    QString filename=doc->getAbsFilePath();	
    _cfg->writeEntry("filename", filename);
    _cfg->writeEntry("modified", doc->isModified());
		
    QString tempname = kapp->tempSaveName(filename);
    doc->saveDocument(tempname);
  }
}


void LSkatApp::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readEntry("filename", QCString(""));
  bool modified = _cfg->readBoolEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
  	
    if(canRecover)
    {
      doc->openDocument(tempname);
      doc->setModified();
      QFileInfo info(filename);
      doc->setAbsFilePath(info.absFilePath());
      doc->setTitle(info.fileName());
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(filename);
    }
  }

  //QString caption=kapp->caption();	
  setCaption(i18n("Lieutnant Skat"));
}		

bool LSkatApp::queryClose()
{
  return true;
  return doc->saveModified();
}

bool LSkatApp::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void LSkatApp::slotEditUndo()
{


  slotStatusMsg(i18n("Undo move..."));
  slotStatusMsg(i18n("Ready"));
}

void LSkatApp::slotEditRedo()
{

  slotStatusMsg(i18n("Redo move..."));

  slotStatusMsg(i18n("Ready"));
}


void LSkatApp::slotFileStatistics()
{
  slotStatusMsg(i18n("Delete all time statistics..."));
   QString message;

   message=i18n("Do you really want to clear the all time\n"
                "statistical data?\n");

  if (KMessageBox::Yes==KMessageBox::questionYesNo(this,message))
  {
    doc->ClearStats();
    doc->slotUpdateAllViews(0);
  }
  slotStatusMsg(i18n("Ready"));
}
/** send message */
void LSkatApp::slotFileMessage()
{
  int res;
  slotStatusMsg(i18n("Sending message to remote player..."));

  MsgDlg *dlg=new MsgDlg(this,QCString("Send message..."));
  res=dlg->exec();
  if (res==QDialog::Accepted)
  {
    QString s;
    s=dlg->GetMsg();
    if (!s || s.length()<1) s=QCString("...");
    KEMessage *msg=new KEMessage;

    // printf("Msg: %s\n",(char *)msg);

    msg->AddData(QCString("Message"),(char *)(s.latin1()));
     if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
      mInput->SendMsg(msg,0);
     if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
      mInput->SendMsg(msg,1);
    delete msg;
  }
	
  slotStatusMsg(i18n("Ready"));
}



void LSkatApp::slotFileNew()
{
  slotStatusMsg(i18n("Starting a new game..."));
  NewGame();

  slotStatusMsg(i18n("Ready"));
}

void LSkatApp::slotFileClose()
{
  slotStatusMsg(i18n("Ending game..."));
  doc->EndGame(true);	
  doc->slotUpdateAllViews(0);
  slotStatusMsg(i18n("Game ended...start a new one..."));

  KEMessage *msg=new KEMessage;
  msg->AddData(QCString("EndGame"),(short)1);
  if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
    mInput->SendMsg(msg,0);
  if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
    mInput->SendMsg(msg,1);

  msg->RemoveAll();
  msg->AddData(QCString("Terminate"),(short)1);
  if (mInput->QueryType(0)==KG_INPUTTYPE_PROCESS)
    mInput->SendMsg(msg,0);
  if (mInput->QueryType(1)==KG_INPUTTYPE_PROCESS)
    mInput->SendMsg(msg,1);
  delete msg;
  slotStatusNames();
}


void LSkatApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(w && !w->close())
          break;
    }
  }	
//  slotStatusMsg(i18n("Ready"));
}


#ifdef USE_TOOLBAR
void LSkatApp::slotViewToolBar()
{
  slotStatusMsg(i18n("Toggle the toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if( viewMenu->isItemChecked(ID_VIEW_TOOLBAR))
  {
    viewMenu->setItemChecked(ID_VIEW_TOOLBAR, false);
    toolBar()->hide();
  }
  else
  {
    viewMenu->setItemChecked(ID_VIEW_TOOLBAR, true);
    toolBar()->show();
  }		

  slotStatusMsg(i18n("Ready"));
}
#endif

void LSkatApp::slotStartplayer(int i)
{
  slotStatusMsg(i18n("Changing startplayer..."));
  doc->SetStartPlayer(i);
  doc->UpdateViews(UPDATE_STATUS);
  slotStatusMsg(i18n("Ready"));
}
void LSkatApp::slotPlayer1(KG_INPUTTYPE i)
{
  slotStatusMsg(i18n("Changing who plays player 1..."));
  doc->SetPlayedBy(0,i);
  if (doc->IsRunning())
  {
    MakeInputDevice(0);
  }
  doc->UpdateViews(UPDATE_STATUS);
  slotStatusMsg(i18n("Ready"));
}
void LSkatApp::slotPlayer2(KG_INPUTTYPE i)
{
  slotStatusMsg(i18n("Changing who plays player 2..."));
  doc->SetPlayedBy(1,i);
  if (doc->IsRunning())
  {
    MakeInputDevice(1);
  }
  doc->UpdateViews(UPDATE_STATUS);
  slotStatusMsg(i18n("Ready"));
}
void LSkatApp::slotOptionsNames()
{
  slotStatusMsg(i18n("Configure player names..."));

  NameDlg *dlg=new NameDlg(this,QCString("Enter your name..."));
  dlg->SetNames(doc->GetName(0),doc->GetName(1));
  if (dlg->exec()==QDialog::Accepted)
  {
    QString n1,n2;
    dlg->GetNames(n1,n2);
    doc->SetName(0,n1);
    doc->SetName(1,n2);
    doc->UpdateViews(UPDATE_STATUS);
    slotStatusNames();
  }

  	
  slotStatusMsg(i18n("Ready"));
}

void LSkatApp::slotOptionsCardDeck()
{
  slotStatusMsg(i18n("Configure carddecks..."));

  QString s1,s2;
  int result;
  s1=doc->GetDeckpath();
  s2=doc->GetCardpath();
  // This is for debug and testing as you can run it from the CVS without
  // installing the carddecks !
  result=KCardDialog::getCardDeck(s1,s2,0,KCardDialog::ProbeDefaultDir,
           "../../carddecks/decks/","../../carddecks/");
  // release version
  // result=KCardDialog::getCardDeck(s1,s2);
  if (result==QDialog::Accepted)
  {
    doc->SetCardDeckPath(s1,s2);
    doc->slotUpdateAllViews(0);
  }
  	
  slotStatusMsg(i18n("Ready"));
}

void LSkatApp::slotLevel(int i)
{
  slotStatusMsg(i18n("Change level..."));
  doc->SetComputerLevel(i);
  doc->UpdateViews(UPDATE_STATUS);
  slotStatusMsg(i18n("Ready"));
}
void LSkatApp::slotDeck(int i)
{
  /*
  slotStatusMsg(i18n("Change carddeck..."));
  doc->SetDeckNo(i);
  doc->slotUpdateAllViews(0);
  slotStatusMsg(i18n("Ready"));
  */
}

void LSkatApp::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if( viewMenu->isItemChecked(ID_VIEW_STATUSBAR))
  {
    viewMenu->setItemChecked(ID_VIEW_STATUSBAR, false);
    statusBar()->hide();
  }
  else
  {
    viewMenu->setItemChecked(ID_VIEW_STATUSBAR, true);
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready"));
}
void LSkatApp::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

void LSkatApp::slotStatusMover(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status mover permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MOVER);
}

void LSkatApp::slotStatusTime()
{
  ///////////////////////////////////////////////////////////////////
  // change status time permanently
  QString s;
  QTime now;

  now=QTime::currentTime();
  s=now.toString();
  s.truncate(5);

  statusBar()->clear();
  statusBar()->changeItem(s, ID_STATUS_TIME);
}


void LSkatApp::slotStatusHelpMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 2000);
}




void LSkatApp::commandCallback(int id_)
{
  switch (id_)
  {
    case ID_FILE_NEW:
    	 slotFileNew();
         break;

    case ID_FILE_CLOSE:
         slotFileClose();
         break;

    case ID_FILE_STATISTICS:
         slotFileStatistics();
         break;

    case ID_FILE_MESSAGE:
         slotFileMessage();
         break;

    case ID_FILE_QUIT:
         slotFileQuit();
         break;

     case ID_EDIT_UNDO:
         slotEditUndo();
         break;

    case ID_EDIT_REDO:
         slotEditRedo();
         break;

#ifdef USE_TOOLBAR
    case ID_VIEW_TOOLBAR:
         slotViewToolBar();
         break;
#endif

    case ID_VIEW_STATUSBAR:
         slotViewStatusBar();
         break;
    case ID_LEVEL_1:
    case ID_LEVEL_2:
    case ID_LEVEL_3:
    case ID_LEVEL_4:
    case ID_LEVEL_5:
    case ID_LEVEL_6:
    case ID_LEVEL_7:
    case ID_LEVEL_8:
    case ID_LEVEL_9:
    case ID_LEVEL_10:
         slotLevel(id_-ID_LEVEL_1+1);
         break;
   
    case ID_DECK_1:
    case ID_DECK_2:
    case ID_DECK_3:
    case ID_DECK_4:
    case ID_DECK_5:
    case ID_DECK_6:
    case ID_DECK_7:
    case ID_DECK_8:
    case ID_DECK_9:
         slotDeck(id_-ID_DECK_1);
         break;
    case ID_STARTPLAYER_1:
        slotStartplayer(0);
    break;
    case ID_STARTPLAYER_2:
        slotStartplayer(1);
    break;

    case ID_PLAYER1_PLAYER:
        slotPlayer1(KG_INPUTTYPE_INTERACTIVE);
    break;
    case ID_PLAYER1_COMPUTER:
        slotPlayer1(KG_INPUTTYPE_PROCESS);
    break;
    case ID_PLAYER1_REMOTE:
        slotPlayer1(KG_INPUTTYPE_REMOTE);
    break;

    case ID_PLAYER2_PLAYER:
        slotPlayer2(KG_INPUTTYPE_INTERACTIVE);
    break;
    case ID_PLAYER2_COMPUTER:
        slotPlayer2(KG_INPUTTYPE_PROCESS);
    break;
    case ID_PLAYER2_REMOTE:
        slotPlayer2(KG_INPUTTYPE_REMOTE);
    break;

    case ID_OPTIONS_NAMES:
        slotOptionsNames();
    break;
    case ID_OPTIONS_CARDDECK:
        slotOptionsCardDeck();
    break;

    default:
         break;
  }
}

void LSkatApp::statusCallback(int id_)
{
  QString s;
  switch (id_)
  {
    case ID_FILE_NEW:
         slotStatusHelpMsg(i18n("Starts a new game"));
         break;

    case ID_FILE_STATISTICS:
         slotStatusHelpMsg(i18n("Clear all time statistics..."));
         break;

    case ID_FILE_MESSAGE:
         slotStatusHelpMsg(i18n("Sends a message to a remote player"));
         break;

    case ID_FILE_CLOSE:
         slotStatusHelpMsg(i18n("Ends the current game"));
         break;

    case ID_FILE_QUIT:
         slotStatusHelpMsg(i18n("Quits the application"));
         break;

    case ID_EDIT_UNDO:
         slotStatusHelpMsg(i18n("Undoes a move"));
         break;

    case ID_EDIT_REDO:
         slotStatusHelpMsg(i18n("Redoes a previously undone move"));
         break;

#ifdef USE_TOOLBAR
    case ID_VIEW_TOOLBAR:
         slotStatusHelpMsg(i18n("Enables/disables the toolbar"));
         break;
#endif

    case ID_VIEW_STATUSBAR:
         slotStatusHelpMsg(i18n("Enables/disables the statusbar"));
         break;

    case ID_LEVEL_1:
    case ID_LEVEL_2:
    case ID_LEVEL_3:
    case ID_LEVEL_4:
    case ID_LEVEL_5:
    case ID_LEVEL_6:
    case ID_LEVEL_7:
    case ID_LEVEL_8:
    case ID_LEVEL_9:
    case ID_LEVEL_10:
         s.setNum((int)(id_-ID_LEVEL_1+1));
         s=i18n("Set the level of the computer player to %1").arg(s);
         slotStatusHelpMsg(s);
         break;

    case ID_DECK_1:
    case ID_DECK_2:
    case ID_DECK_3:
    case ID_DECK_4:
    case ID_DECK_5:
    case ID_DECK_6:
    case ID_DECK_7:
    case ID_DECK_8:
    case ID_DECK_9:
         s.setNum((int)(id_-ID_DECK_1+1));
         s=i18n("Set the carddeck to %1").arg(s);
         slotStatusHelpMsg(s);
         break;

    case ID_STARTPLAYER_1:
         slotStatusHelpMsg(i18n("Player 1 starts the next game"));
    break;
    case ID_STARTPLAYER_2:
         slotStatusHelpMsg(i18n("Player 2 starts the next game"));
    break;

    case ID_PLAYER1_PLAYER:
         slotStatusHelpMsg(i18n("Player 1 played interactively"));
    break;
    case ID_PLAYER1_COMPUTER:
         slotStatusHelpMsg(i18n("Player 1 played by the computer"));
    break;
    case ID_PLAYER1_REMOTE:
         slotStatusHelpMsg(i18n("Player 1 played over network"));
    break;

    case ID_PLAYER2_PLAYER:
         slotStatusHelpMsg(i18n("Player 2 played interactively"));
    break;
    case ID_PLAYER2_COMPUTER:
         slotStatusHelpMsg(i18n("Player 2 played by the computer"));
    break;
    case ID_PLAYER2_REMOTE:
         slotStatusHelpMsg(i18n("Player 2 played over network"));
    break;

    case ID_OPTIONS_NAMES:
         slotStatusHelpMsg(i18n("Change the names of the players"));
    break;
    case ID_OPTIONS_CARDDECK:
         slotStatusHelpMsg(i18n("Change the carddeck"));
    break;
    default:
         break;
  }
}

/** Triggers the processmove timer */
void LSkatApp::slotProcTimer(void)
{
  mInput->Unlock();
  mInput->Next(doc->GetCurrentPlayer());
  /*
  printf("Delayed setting Next=%d ->%d\n",
    doc->GetCurrentPlayer(),mInput->QueryNext());
  */
}

/** Triggers the status timer */
void LSkatApp::slotStatusTimer(void){
  slotStatusTime();
}
/** Set the names in the mover field */
void LSkatApp::slotStatusNames(){

  QString msg;
  if (!doc->IsRunning()) msg=i18n("No game running");
  else
  {
    msg=i18n("%1 to move ...").arg(doc->GetName(doc->GetCurrentPlayer()));
  }
  slotStatusMover(msg);
}
/** Check a menuitem */
void LSkatApp::checkCommand(int id){
  menuBar()->setItemChecked(id, true);
}
/** Uncheck Menuitem */
void LSkatApp::uncheckCommand(int id){
  menuBar()->setItemChecked(id, false);
}
/** Is the menuitem enabled? */
bool LSkatApp::isEnabled(int id){
  return menuBar()->isItemEnabled(id);

}
void LSkatApp::slotFileToShow()
{
  if (doc->IsRunning())
  {
    disableCommand(ID_FILE_NEW);
    enableCommand(ID_FILE_CLOSE);
  }
  else
  {
    disableCommand(ID_FILE_CLOSE);
    enableCommand(ID_FILE_NEW);
  }
  if (!doc->IsServer() &&
      (
       (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE &&
        doc->GetPlayedBy(0)==KG_INPUTTYPE_REMOTE)
       ||
       (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE &&
        doc->GetPlayedBy(1)==KG_INPUTTYPE_REMOTE)
       )
     )
  {
    disableCommand(ID_FILE_NEW);
  }
  if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE ||
      mInput->QueryType(0)==KG_INPUTTYPE_REMOTE )
  {
    enableCommand(ID_FILE_MESSAGE);
  }
  else
  {
    disableCommand(ID_FILE_MESSAGE);
  }

}

void LSkatApp::slotEditToShow()
{

}
void LSkatApp::slotOptionsToShow()
{
}

void LSkatApp::slotLevelToShow()
{
  int i;
  for (i=0;i<10;i++)
  {
    uncheckCommand(ID_LEVEL_1+i);
  }
 checkCommand(ID_LEVEL_1-1+doc->GetComputerLevel());
}
void LSkatApp::slotDeckToShow()
{
  /*
  int i;
  for (i=0;i<NO_OF_DECKS;i++)
  {
    if (doc->HasDeck(i)) enableCommand(ID_DECK_1+i);
    else disableCommand(ID_DECK_1+i);
  }
  for (i=0;i<NO_OF_DECKS;i++)
  {
    uncheckCommand(ID_DECK_1+i);
  }
  checkCommand(ID_DECK_1+doc->GetDeckNo());
  */
}
void LSkatApp::slotStartplayerToShow()
{
  if (doc->GetStartPlayer()==0)
  {
     checkCommand(ID_STARTPLAYER_1 );
     uncheckCommand(ID_STARTPLAYER_2 );
  }
  else
  {
    uncheckCommand(ID_STARTPLAYER_1 );
    checkCommand(ID_STARTPLAYER_2 );
  }
  if (doc->IsRunning())
  {
    disableCommand(ID_STARTPLAYER_1);
    disableCommand(ID_STARTPLAYER_2);
  }
  else
  {
    enableCommand(ID_STARTPLAYER_1);
    enableCommand(ID_STARTPLAYER_2);
  }
}
void LSkatApp::slotPlayer1ToShow()
{
  uncheckCommand(ID_PLAYER1_PLAYER );
  uncheckCommand(ID_PLAYER1_COMPUTER );
  uncheckCommand(ID_PLAYER1_REMOTE );
  if (doc->GetPlayedBy(0)==KG_INPUTTYPE_INTERACTIVE)
  {
     checkCommand(ID_PLAYER1_PLAYER );
  }
  else if (doc->GetPlayedBy(0)==KG_INPUTTYPE_PROCESS)
  {
    checkCommand(ID_PLAYER1_COMPUTER );
  }
  else
  {
    checkCommand(ID_PLAYER1_REMOTE );
  }
  if (doc->IsRunning())
  {
    disableCommand(ID_PLAYER1_PLAYER);
    disableCommand(ID_PLAYER1_COMPUTER);
    disableCommand(ID_PLAYER1_REMOTE);
  }
  else
  {
    enableCommand(ID_PLAYER1_PLAYER);
    enableCommand(ID_PLAYER1_COMPUTER);
    enableCommand(ID_PLAYER1_REMOTE);
  }
  if (doc->GetPlayedBy(1)==KG_INPUTTYPE_REMOTE)
  {
    disableCommand(ID_PLAYER1_REMOTE);
  }
}
void LSkatApp::slotPlayer2ToShow()
{
  uncheckCommand(ID_PLAYER2_PLAYER );
  uncheckCommand(ID_PLAYER2_COMPUTER );
  uncheckCommand(ID_PLAYER2_REMOTE );
  if (doc->GetPlayedBy(1)==KG_INPUTTYPE_INTERACTIVE)
  {
     checkCommand(ID_PLAYER2_PLAYER );
  }
  else if (doc->GetPlayedBy(1)==KG_INPUTTYPE_PROCESS)
  {
    checkCommand(ID_PLAYER2_COMPUTER );
  }
  else
  {
    checkCommand(ID_PLAYER2_REMOTE );
  }
  if (doc->IsRunning())
  {
    disableCommand(ID_PLAYER2_PLAYER);
    disableCommand(ID_PLAYER2_COMPUTER);
    disableCommand(ID_PLAYER2_REMOTE);
  }
  else
  {
    enableCommand(ID_PLAYER2_PLAYER);
    enableCommand(ID_PLAYER2_COMPUTER);
    enableCommand(ID_PLAYER2_REMOTE);
  }
  if (doc->GetPlayedBy(0)==KG_INPUTTYPE_REMOTE)
  {
    disableCommand(ID_PLAYER2_REMOTE);
  }
}

void LSkatApp::NewGame()
{
  bool res;
  // doc->SetIntro(0);
  doc->NewGame();
  doc->slotUpdateAllViews(0);
  res=MakeInputDevice(0);
  if (!res) 
  {
    KMessageBox::error(this,
      i18n("Cannot start player 1. Maybe the network connection\n"
           "failed or the computer player process file is not\n"
           "found.\n"));
    return ;
  }
  res=MakeInputDevice(1);
  if (!res) 
  {
    KMessageBox::error(this,
      i18n("Cannot start player 2. Maybe the network connection\n"
           "failed or the computer player process file is not\n"
           "found.\n"));
    return ;
  }
  // Remote game is started when receiving the start mesage .. not here!
  if (mInput->QueryType(0)!=KG_INPUTTYPE_REMOTE &&
      mInput->QueryType(1)!=KG_INPUTTYPE_REMOTE )
  {
    mInput->Unlock();
    mInput->Next(doc->GetStartPlayer());
  }
  // Connected 0 and we are server
  else if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE &&
           mInput->QueryStatus()>0 && doc->IsServer())
  {
    mInput->Unlock();
    mInput->Next(doc->GetStartPlayer());
  }
  // Connected 1 and we are server
  else if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE &&
           mInput->QueryStatus()>1 && doc->IsServer())
  {
    mInput->Unlock();
    mInput->Next(doc->GetStartPlayer());
  }
  slotStatusNames();
}

bool LSkatApp::MakeInputDevice(int no)
{
  bool res=true;
  KEMessage *msg;
  KG_INPUTTYPE type=doc->GetPlayedBy(no);
  if (type==KG_INPUTTYPE_INTERACTIVE)
  {
    if (mInput->QueryType(no)!=type)
      res=mInput->SetInputDevice(no,type);
  }
  else if (type==KG_INPUTTYPE_REMOTE)
  {
      QString host;
      short port;
      msg=new KEMessage;
      PrepareGame(msg);
      // Build new connection
      if ( mInput->QueryType(no)!=type || mInput->QueryStatus(no)<=0) 
      {
        OptionsNetwork();
        port=doc->QueryPort();
        host=doc->QueryHost();
        msg->AddData(QCString("Port"),(short)port);
        msg->AddData(QCString("IP"),(char *)(host.latin1()));
        res=mInput->SetInputDevice(no,type,msg);
        if (!res)
        {
          QProgressDialog *progress;
          QString s;
          int tim,j;
          tim=10000;
          if (!host.isEmpty())
          {
            s=i18n("Remote connection to %1:%2...").arg(host).arg(port);
          }
          else
          {
            s=i18n("Offering remote connection on port %1 ...").arg(port);
          }
          progress=new QProgressDialog(s, i18n("Abort"), tim, this,0,true );
          progress->setCaption(i18n("Lieutnant Skat"));
          for (j=0; j<tim; j++)
          {
            progress->setProgress( j );
            if ( progress->wasCancelled() ) break;
            if (mInput->QueryStatus(no)>0) break;
            usleep(100);
          }
          // progress.setProgress(tim);
          delete progress;
        }
      }
      else // we are already connected
      {
        // force the other one to be client
        usleep(1000);
        if (doc->IsServer())
        {
          if (global_debug>10)
            printf("We want theother one to be client\n");
          msg->AddData(QCString("Client"),(short)-1); // force client
          mInput->SendMsg(msg,no);
        }
        // resp server
        else
        {
          if (global_debug>10)
            printf("We want theother one to be server\n");
          msg->AddData(QCString("Server"),(short)-1); // force server
          mInput->SendMsg(msg,no);
        }
      }
      delete msg;
      if (mInput->QueryStatus(no)>0)
      {
        res=true;
      }
      else res=false;

  }
  else if (type==KG_INPUTTYPE_PROCESS)
  {
    if (mInput->QueryType(no)!=type)
    {
      // QString path=kapp->dirs()->findExe(doc->QueryProcessName());
        QString path=doc->GetProcess();
        if (global_debug>5)
        {
          printf("Make Process %d\n",no);
          printf("Exe file found: %s\n",path.latin1());
        }
      if (path.isNull())  return false;
      msg=new KEMessage;
      msg->AddData(QCString("ProcessName"),(char *)(path.latin1()));
      // msg->AddData("ProcessName",doc->QueryProcessName());
      res=mInput->SetInputDevice(no,KG_INPUTTYPE_PROCESS,msg);
      delete msg;
    }
 
  }
  return res;
}

void LSkatApp::slotHelpAbout()
{
  slotStatusMsg(i18n("About the program..."));

  EMovie *movie=new EMovie;
  movie->frameDelay=70;
  movie->frameArray=doc->mPixAnim;
  movie->frameCnt=NO_OF_ANIM;
  aboutDlg *dlg=new aboutDlg(movie,this);
  dlg->exec();
  delete movie;
  slotStatusMsg(i18n("Ready"));
}


void LSkatApp::OptionsNetwork()
{
  int res;

  NetworkDlg *dlg=new NetworkDlg(this,QCString("Configure a network game..."));
  dlg->SetPort(doc->QueryPort());
  dlg->SetHost(doc->QueryHost());
  res=dlg->exec();
  doc->SetPort(dlg->QueryPort());
  doc->SetHost(dlg->QueryHost());
  delete dlg;
}

void LSkatApp::slotPrepareProcessMove(KEMessage *msg)
{
  if (global_debug>3)
    printf("+++ main should prepare process move\n");
  slotStatusMsg(i18n("Waiting for the computer to move..."));

  msg->AddData(QCString("Hint"),(short)0);
  msg->AddData(QCString("Move"),(short)1);

  if (global_debug>3)
    printf("PREPARE GAME in processmove\n");
  if (global_debug>1)
    msg->AddData(QCString("KLogSendMsg"),"process.log");
  PrepareGame(msg);
}
void LSkatApp::slotPrepareRemoteMove(KEMessage *msg)
{
  if (global_debug>3)
    printf("+++ main should prepare remote move\n");
  slotStatusMsg(i18n("Waiting for remote player..."));
}
void LSkatApp::slotPrepareInteractiveMove(KEMessage *msg)
{
  if (global_debug>3)
    printf("+++ main should prepare interactive move\n");
  slotStatusMsg(i18n("Please make your move..."));
}
void LSkatApp::slotReceiveInput(KEMessage *msg,int id)
{
  /*
  if (global_debug>=0)
  {
    QStrList *keys=msg->QueryKeys();
    char *it;
    printf("  MESSAGESIZE=%u\n",msg->QueryNumberOfKeys());
    for (it=keys->first();it!=0;it=keys->next())
    {
      printf("    Key '%s' type=%d\n",it,msg->QueryType(it));
    }
  }
  */
  short move;
  QString message;
  short x,y,player;
  bool remotesend;
  int size;

  if (msg->HasKey(QCString("Debug")))
  {
    char *debug;
    msg->GetData(QCString("Debug"),debug,size);
    printf("Received Debug: %d <%s>\n",size,debug);
  }
  if (msg->HasKey(QCString("ConnectionLost")))
  {
    if (msg->GetData(QCString("ConnectionLost"),move))
    {
      if (move==0)
      {
        message=i18n("Remote connection lost for player 1...");
        KMessageBox::information(this,message);
        slotStatusMsg(message);
        slotPlayer1(KG_INPUTTYPE_INTERACTIVE);
      }
      else
      {
        message=i18n("Remote connection lost for player 2...");
        KMessageBox::information(this,message);
        slotStatusMsg(message);
        slotPlayer2(KG_INPUTTYPE_INTERACTIVE);
      }
    }
  }
  if (msg->HasKey(QCString("Message")))
  {
    char *p;
    if (msg->GetData(QCString("Message"),p,size))
    {
      message=i18n("Message from remote player:\n")+p;
      KMessageBox::information(this,message);
      if (global_debug>3)
        printf("MESSAGE **** %s ****\n",message.latin1());
    }
  }
  if (msg->HasKey(QCString("EndGame")))
  {
    KEMessage *msg2=new KEMessage;

    msg2->AddData(QCString("Terminate"),(short)1);
    if (mInput->QueryType(0)==KG_INPUTTYPE_PROCESS)
      mInput->SendMsg(msg2,0);
    if (mInput->QueryType(1)==KG_INPUTTYPE_PROCESS)
      mInput->SendMsg(msg2,1);
    delete msg2;

    msg->GetData(QCString("EndGame"),move);
    message=i18n("Remote player ended game...");
    KMessageBox::information(this,message);
    slotStatusMsg(message);

    doc->EndGame(true);	
    doc->slotUpdateAllViews(0);
    slotStatusNames();
  }

  if (msg->HasKey(QCString("Move")))
  {
     slotStatusMsg(i18n("Ready"));
     msg->GetData(QCString("Move"),player);
     msg->GetData(QCString("MoveX"),x);
     msg->GetData(QCString("MoveY"),y);
     remotesend=msg->HasKey(QCString("RemoteMove"));
     if (remotesend && doc->IsRemoteSwitch())
       player=1-player;
     Move((int)x,(int)y,(int)player,remotesend);
  }
  // Client key is automatically added by message system !!!
  if (msg->HasKey(QCString("Client")))
  {
    if (global_debug>5)
      printf("We are client and extracting game data now.\n");
    slotStatusMsg(i18n("You are network client...loading remote game..."));
    doc->NewGame();
    ExtractGame(msg);
    doc->SetServer(false);
    doc->slotUpdateAllViews(0);
    mInput->Unlock();
    mInput->Next(doc->GetStartPlayer());
  }
  // Server key is automatically added by message system !!!
  if (msg->HasKey(QCString("Server")))
  {
    if (global_debug>5)
      printf("We are server now.\n");
    slotStatusMsg(i18n("You are network server..."));
    doc->SetServer(true);
    mInput->Unlock();
    mInput->Next(doc->GetStartPlayer());
  }
}

// Called after the move animation done
void LSkatApp::MoveFinished()
{
    int res=doc->MakeMove();
    QString ld,s;
    if (res==2) // end game
    {
      doc->EvaluateGame();
      doc->slotUpdateAllViews(0);
      // m->SetStatusBar(i18n("Game over"),3);
      //doc->SwitchStartPlayer();
      doc->slotUpdateAllViews(0);
    }
    else if (doc->IsRunning()) // continue game
    {
      // Delay fast playing of the computer !
      /*
      printf("*** next=%d current=%d playedby=%d\n",
          mInput->QueryNext(),doc->GetCurrentPlayer() ,
          doc->GetPlayedBy(doc->GetCurrentPlayer()));
      */
      // Maybe we do not need to distinquish these two cases?
      // Computer was last and is next
      if (doc->GetPlayedBy(mInput->QueryNext())==KG_INPUTTYPE_PROCESS &&
          doc->GetPlayedBy(doc->GetCurrentPlayer())==KG_INPUTTYPE_PROCESS)
      {
        procTimer->start(1000,true);
      }
      // Computer is next
      else if (doc->GetPlayedBy(doc->GetCurrentPlayer())==KG_INPUTTYPE_PROCESS)
      {
        procTimer->start(1000,true);
      }
      else
      {
        mInput->Unlock();
        mInput->Next(doc->GetCurrentPlayer());
      }
    }
    slotStatusNames();
}
void LSkatApp::Move(int x,int y,int player,bool remote)
{
  KEMessage *msg;
  if (global_debug>1)
    printf("Move of %d to x=%d y=%d\n",player,x,y);
  if (x<0 || y<0 || x>7 || y>1 || player <0 || player>1)
  {
     KMessageBox::error(this, 
       i18n("Severe internal error. Move to illegal position.\n"
            "Restart game and report bug to the developer.\n"));
    return ;
  }
  int res=doc->PrepareMove(player,y*4+x);
  if (res>0)
  {
    doc->InitMove(0,player,x,y);
    if (mInput->QueryType()!=KG_INPUTTYPE_REMOTE && !remote )
    {
      msg=new KEMessage;
      if (doc->IsRemoteSwitch()) player=1-player;
      msg->AddData(QCString("Move"),(short)player);
      msg->AddData(QCString("MoveX"),(short)x);
      msg->AddData(QCString("MoveY"),(short)y);
      msg->AddData(QCString("RemoteMove"),(short)1);
      if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
        mInput->SendMsg(msg,0);
      if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
        mInput->SendMsg(msg,1);
      delete msg;
    }
  }
  else if (res==-3)
  {
     KMessageBox::information(this, 
       i18n("Ehm, this move would not follow the rulebook.\n"
            "Better think again!\n"));
    return ;
  }
  else if (res==-2)
  {
     KMessageBox::information(this,i18n("It is not your turn.\n"));
    return ;
  }
  else
  {
     KMessageBox::information(this, i18n("This move is not possible.\n"));
    return ;
  }
}
void LSkatApp::PrepareGame(KEMessage *msg)
{
  if (!msg) return ;
  msg->AddData(QCString("Cards"),(char *)doc->GetCardP(),NO_OF_CARDS*sizeof(int));
  msg->AddData(QCString("Startplayer"),(short)doc->GetStartPlayer());
  msg->AddData(QCString("CurrentPlayer"),(short)doc->GetCurrentPlayer());
  if (doc->GetPlayedBy(0)==KG_INPUTTYPE_REMOTE)
    msg->AddData(QCString("RemoteIs"),(short)0);
  else if (doc->GetPlayedBy(1)==KG_INPUTTYPE_REMOTE)
    msg->AddData(QCString("RemoteIs"),(short)1);
  msg->AddData(QCString("Trump"),(short)doc->GetTrump());
  // For computer player
  // -1 or the current played card
  msg->AddData(QCString("CurrentMove"),(short)doc->GetMove(doc->GetStartPlayer()));
  msg->AddData(QCString("Height"),(char *)doc->GetCardHeightP(),NO_OF_CARDS/2*sizeof(int));
  msg->AddData(QCString("No"),(short)doc->GetMoveNo());
  msg->AddData(QCString("Sc1"),(short)doc->GetScore(0));
  msg->AddData(QCString("Sc2"),(short)doc->GetScore(1));
  msg->AddData(QCString("Level"),(short)doc->GetComputerLevel());
}
void LSkatApp::ExtractGame(KEMessage *msg)
{
  if (!msg) return ;
  // Do we have to switch players?
  bool switchit;
  short remote;
  msg->GetData(QCString("RemoteIs"),remote);
  if (doc->GetPlayedBy(remote)==KG_INPUTTYPE_REMOTE) switchit=true;
  else switchit=false;

  short start;
  int i,size;
  int *cards;
  char *p;
  short trump;
  msg->GetData(QCString("Startplayer"),start);
  msg->GetData(QCString("Cards"),p,size);
  cards=(int *)p;
  msg->GetData(QCString("Trump"),trump);
  if (size!=NO_OF_CARDS*sizeof(int))
  {
    printf("Error: Transmission of cards failed..wrong sizes\n");
  }

  doc->SetRemoteSwitch(switchit);
  if (switchit)
  {
    start=1-start;
    for (i=0;i<NO_OF_CARDS;i++)
    {
      if (i>=NO_OF_CARDS/2)
        doc->SetCard(i-NO_OF_CARDS/2,cards[i]);
      else
        doc->SetCard(i+NO_OF_CARDS/2,cards[i]);
    }
  }
  else
  {
    for (i=0;i<NO_OF_CARDS;i++)
    {
      doc->SetCard(i,cards[i]);
    }
  }
  
  doc->SetStartPlayer(start);
  doc->SetCurrentPlayer(start);
  doc->SetTrump((CCOLOUR)trump);
}


void LSkatApp::SetGrafix(QString s)
{
  mGrafix=s;
}

#include "lskat.moc"
