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
//#include <qprinter.h>
//#include <qpainter.h>
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
#include <kstdaction.h>
#include <kaction.h>


// application specific includes
#include "lskat.h"
#include "lskatview.h"
#include "lskatdoc.h"
#include "namedlg.h"
#include "networkdlg.h"
#include "aboutdlg.h"
#include "movielabel.h"
#include "msgdlg.h"
#include <kcarddialog.h>


#include <stdlib.h>

#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG               1003
#define ID_STATUS_MOVER              1002
#define ID_STATUS_TIME               1001

LSkatApp::LSkatApp() : KMainWindow(0)
{
  config=kapp->config();

   // localise data file
   QString file=QString::fromLatin1("lskat/grafix/t1.png");
   mGrafix=kapp->dirs()->findResourceDir("data", file);
   if (mGrafix.isNull()) mGrafix = QCString("grafix/");
   else mGrafix+=QCString("lskat/grafix/");
   if (global_debug>3) printf("Localised datafile=%s\n",mGrafix.latin1());


  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initGUI();
  initStatusBar();
  setHelpMenuEnabled(false);
  createGUI(QString::null, false);

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

  // better be last in init
  checkMenus();

}

LSkatApp::~LSkatApp()
{
 delete mInput;
}

void LSkatApp::checkMenus(int menu)
{
  if (!menu || (menu&CheckFileMenu))
  {
    if (doc->IsRunning()) disableAction("new_game");
    else enableAction("new_game");

    if (!doc->IsRunning()) disableAction("end_game");
    else enableAction("end_game");

    if (doc->GetPlayedBy(0)==KG_INPUTTYPE_REMOTE ||
        doc->GetPlayedBy(0)==KG_INPUTTYPE_REMOTE )
    {
      enableAction("send_message");
    }
    else
    {
      disableAction("send_message");
    }

  }
  if (!menu || (menu&CheckViewMenu))
  {
    if (statusBar()->isHidden()) ((KToggleAction*)ACTION("show_statusbar"))->setChecked(false);
    else ((KToggleAction*)ACTION("show_statusbar"))->setChecked(true);
  }

  if (!menu || (menu&CheckOptionsMenu))
  {
    ((KSelectAction *)ACTION("startplayer"))->setCurrentItem(doc->GetStartPlayer());

    if (doc->IsRunning()) disableAction("startplayer");
    else enableAction("startplayer");

    if (doc->GetPlayedBy(0)==KG_INPUTTYPE_INTERACTIVE)
      ((KSelectAction *)ACTION("player1"))->setCurrentItem(0);
    else if (doc->GetPlayedBy(0)==KG_INPUTTYPE_PROCESS)
      ((KSelectAction *)ACTION("player1"))->setCurrentItem(1);
    else
      ((KSelectAction *)ACTION("player1"))->setCurrentItem(2);

    /*
    if (doc->IsRunning()) disableAction("player1");
    else enableAction("player1");
    */

    if (doc->GetPlayedBy(1)==KG_INPUTTYPE_INTERACTIVE)
      ((KSelectAction *)ACTION("player2"))->setCurrentItem(0);
    else if (doc->GetPlayedBy(1)==KG_INPUTTYPE_PROCESS)
      ((KSelectAction *)ACTION("player2"))->setCurrentItem(1);
    else
      ((KSelectAction *)ACTION("player2"))->setCurrentItem(2);

    /*
    if (doc->IsRunning()) disableAction("player2");
    else enableAction("player2");
    */

    ((KSelectAction *)ACTION("choose_level"))->setCurrentItem(doc->GetComputerLevel()-1);
  }

}

void LSkatApp::initGUI()
{
  QStringList list;
  
  (void)KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection(), "new_game");
  ACTION("new_game")->setStatusText(i18n("Starting a new game..."));
  ACTION("new_game")->setWhatsThis(i18n("Starting a new game..."));
  (void)new KAction(i18n("&End Game"),"stop", 0, this, SLOT(slotFileEnd()),
                      actionCollection(), "end_game");
  ACTION("end_game")->setStatusText(i18n("Ending the current game..."));
  ACTION("end_game")->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));
  (void)new KAction(i18n("&Clear Statistics..."),"flag", 0, this, SLOT(slotFileStatistics()),
                      actionCollection(), "clear_statistics");
  ACTION("clear_statistics")->setStatusText(i18n("Delete all time statistics..."));
  ACTION("clear_statistics")->setWhatsThis(i18n("Clears the all time statistics which is kept in all sessions."));
  (void)new KAction(i18n("Send &Message..."), CTRL+Key_M, this, SLOT(slotFileMessage()),
                      actionCollection(), "send_message");
  ACTION("send_message")->setStatusText(i18n("Sending message to remote player..."));
  ACTION("send_message")->setWhatsThis(i18n("Allows you to talk with a remote player."));
  (void)KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection(), "game_exit");
  ACTION("game_exit")->setStatusText(i18n("Exiting..."));
  ACTION("game_exit")->setWhatsThis(i18n("Quits the program."));

  (void)new KToggleAction(i18n("&Statusbar"),0,this,SLOT(slotViewStatusBar()),
                      actionCollection(), "show_statusbar");
  ACTION("show_statusbar")->setStatusText(i18n("Toggle the statusbar..."));
  ACTION("show_statusbar")->setWhatsThis(i18n("Toggle the statusbar..."));

  (void)new KSelectAction(i18n("Startplayer"),0,this,SLOT(slotStartplayer()),
                      actionCollection(), "startplayer");
  ACTION("startplayer")->setStatusText(i18n("Changing startplayer..."));
  ACTION("startplayer")->setWhatsThis(i18n("Chooses which player begins the next game."));
  list.clear();
  list.append(i18n("Player &1"));
  list.append(i18n("Player &2"));
  ((KSelectAction *)ACTION("startplayer"))->setItems(list);

  (void)new KSelectAction(i18n("Player &1 played by"),0,this,SLOT(slotPlayer1By()),
                      actionCollection(), "player1");
  ACTION("player1")->setStatusText(i18n("Changing who plays player 1..."));
  ACTION("player1")->setWhatsThis(i18n("Changing who plays player 1..."));
  list.clear();
  list.append(i18n("&Player"));
  list.append(i18n("&Computer"));
  list.append(i18n("&Remote"));
  ((KSelectAction *)ACTION("player1"))->setItems(list);
  (void)new KSelectAction(i18n("Player &2 played by"),0,this,SLOT(slotPlayer2By()),
                      actionCollection(), "player2");
  ACTION("player1")->setStatusText(i18n("Changing who plays player 2..."));
  ACTION("player1")->setWhatsThis(i18n("Changing who plays player 2..."));
  ((KSelectAction *)ACTION("player2"))->setItems(list);

  (void)new KSelectAction(i18n("&Level"),0,this,SLOT(slotLevel()),
                      actionCollection(), "choose_level");
  ACTION("choose_level")->setStatusText(i18n("Change level..."));
  ACTION("choose_level")->setWhatsThis(i18n("Change the strength of the computer player."));
  list.clear();
  list.append(i18n("&Normal"));
  list.append(i18n("&Advanced"));
  list.append(i18n("&Hard"));
  ((KSelectAction *)ACTION("choose_level"))->setItems(list);
 
  (void)new KAction(i18n("Select &Carddeck..."), 0, this, SLOT(slotOptionsCardDeck()),
                      actionCollection(), "select_carddeck");
  ACTION("select_carddeck")->setStatusText(i18n("Configure carddecks..."));
  ACTION("select_carddeck")->setWhatsThis(i18n("Choose how the cards should look like."));

  (void)new KAction(i18n("Change &Names..."), 0, this, SLOT(slotOptionsNames()),
                      actionCollection(), "change_names");
  ACTION("change_names")->setStatusText(i18n("Configure player names..."));
  ACTION("change_names")->setWhatsThis(i18n("Configure player names..."));

  actionCollection()->setHighlightingEnabled(true);
  connect(actionCollection(), SIGNAL(actionStatusText(const QString &)), SLOT(slotStatusMsg(const QString &)));
  connect(actionCollection(), SIGNAL(clearStatusText()), SLOT(slotClearStatusMsg()));

  KHelpMenu *helpMenu = new KHelpMenu(this, 0, true, actionCollection());
  connect( helpMenu, SIGNAL(showAboutApplication()), this, SLOT(slotHelpAbout()));

  
}


void LSkatApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
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

void LSkatApp::enableAction(const char *s)
{
  if (s)
  {
    KAction *act=actionCollection()->action(s);
    if (act) act->setEnabled(true);
  }
    
}
void LSkatApp::disableAction(const char *s)
{
  if (s)
  {
    KAction *act=actionCollection()->action(s);
    if (act) act->setEnabled(false);
  }
}

LSkatDoc *LSkatApp::getDocument() const
{
  return doc;
}

void LSkatApp::saveOptions()
{
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
  config->writeEntry("Show Statusbar",statusBar()->isVisible());
  doc->WriteConfig(config);
}


void LSkatApp::readOptions()
{
  config->setGroup("General Options");
  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  ((KToggleAction *)ACTION("show_statusbar"))->setChecked(bViewStatusbar);
  if(!bViewStatusbar) statusBar()->hide();

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


void LSkatApp::slotFileStatistics()
{
   QString message;

   message=i18n("Do you really want to clear the all time\n"
                "statistical data?\n");

  if (KMessageBox::Yes==KMessageBox::questionYesNo(this,message))
  {
    doc->ClearStats();
    doc->slotUpdateAllViews(0);
  }
}
/** send message */
void LSkatApp::slotFileMessage()
{
  int res;

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
}



void LSkatApp::slotFileNew()
{
  NewGame();
  checkMenus(CheckFileMenu|CheckOptionsMenu);
}

void LSkatApp::slotFileEnd()
{
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
  checkMenus(CheckFileMenu|CheckOptionsMenu);
  slotStatusNames();
}


void LSkatApp::slotFileQuit()
{
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
}


void LSkatApp::slotStartplayer()
{
  int i=((KSelectAction *)ACTION("startplayer"))->currentItem();
  doc->SetStartPlayer(i);
  doc->UpdateViews(UPDATE_STATUS);
}
void LSkatApp::slotPlayer1By()
{
  switch(((KSelectAction *)ACTION("player1"))->currentItem())
  {
    case 0: 
        slotPlayer1(KG_INPUTTYPE_INTERACTIVE);
    break;
    case 1:
        slotPlayer1(KG_INPUTTYPE_PROCESS);
    break;
    case 2:
        slotPlayer1(KG_INPUTTYPE_REMOTE);
    break;
  }
}
void LSkatApp::slotPlayer2By()
{
  switch(((KSelectAction *)ACTION("player2"))->currentItem())
  {
    case 0: 
        slotPlayer2(KG_INPUTTYPE_INTERACTIVE);
    break;
    case 1:
        slotPlayer2(KG_INPUTTYPE_PROCESS);
    break;
    case 2:
        slotPlayer2(KG_INPUTTYPE_REMOTE);
    break;
  }
}
void LSkatApp::slotPlayer1(KG_INPUTTYPE i)
{
  doc->SetPlayedBy(0,i);
  if (doc->IsRunning())
  {
    MakeInputDevice(0);
    // New: Start computer when switched during game
    if (mInput->QueryType(0)!=KG_INPUTTYPE_REMOTE &&
        doc->GetCurrentPlayer()==0 )
    {
      mInput->Unlock();
      mInput->Next(doc->GetCurrentPlayer());
    }
  }
  doc->UpdateViews(UPDATE_STATUS);
}
void LSkatApp::slotPlayer2(KG_INPUTTYPE i)
{
  doc->SetPlayedBy(1,i);
  if (doc->IsRunning())
  {
    MakeInputDevice(1);
    // New: Start computer when switched during game
    if (mInput->QueryType(0)!=KG_INPUTTYPE_REMOTE &&
        doc->GetCurrentPlayer()==1 )
    {
      mInput->Unlock();
      mInput->Next(doc->GetCurrentPlayer());
    }
  }
  doc->UpdateViews(UPDATE_STATUS);
}
void LSkatApp::slotOptionsNames()
{
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
}

void LSkatApp::slotOptionsCardDeck()
{
  QString s1,s2;
  int result;
  s1=doc->GetDeckpath();
  s2=doc->GetCardpath();

  result=KCardDialog::getCardDeck(s1,s2);
  if (result==QDialog::Accepted)
  {
    doc->SetCardDeckPath(s1,s2);
    doc->slotUpdateAllViews(0);
  }

}

void LSkatApp::slotLevel()
{
  int i=((KSelectAction *)ACTION("choose_level"))->currentItem();
  i++; // we start at 1
  doc->SetComputerLevel(i);
  doc->UpdateViews(UPDATE_STATUS);
  printf("Level set to %d\n",i);
}

void LSkatApp::slotViewStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if (statusBar()->isVisible()) 
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }
}
void LSkatApp::slotClearStatusMsg()
{
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
  disableAction("send_message");
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
      enableAction("send_message");
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
  aboutDlg *dlg=new aboutDlg(this);
  dlg->SetMovie(movie);
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
void LSkatApp::slotPrepareRemoteMove(KEMessage *)
{
  if (global_debug>3)
    printf("+++ main should prepare remote move\n");
  slotStatusMsg(i18n("Waiting for remote player..."));
}
void LSkatApp::slotPrepareInteractiveMove(KEMessage *)
{
  if (global_debug>3)
    printf("+++ main should prepare interactive move\n");
  slotStatusMsg(i18n("Please make your move..."));
}
void LSkatApp::slotReceiveInput(KEMessage *msg,int )
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
      checkMenus(CheckFileMenu|CheckOptionsMenu);
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
