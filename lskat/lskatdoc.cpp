/***************************************************************************
                          lskatdoc.cpp  -  description
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

// include files for Qt
#include <qdir.h>
#include <qfileinfo.h>
#include <qwidget.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>

// application specific includes
#include "lskatdoc.h"
#include "lskat.h"
#include "lskatview.h"
#include <kcarddialog.h>

QList<LSkatView> *LSkatDoc::pViewList = 0L;

LSkatDoc::LSkatDoc(QWidget *parent, const char *name) : QObject(parent, name)
{
  int i;
  if(!pViewList)
  {
    pViewList = new QList<LSkatView>();
  }

  pViewList->setAutoDelete(true);

  isrunning=0;
  wasgame=false;
  initrandom();
  // Allow translation of playernames
  /*
  names[0]=i18n("Alice");
  names[1]=i18n("Bob");
  */

  for (i=0;i<14;i++) cardvalues[i]=0;
  cardvalues[(int)Ace]=11;
  cardvalues[(int)Ten]=10;
  cardvalues[(int)King]=4;
  cardvalues[(int)Queen]=3;
  cardvalues[(int)Jack]=2;

  /*
  computerlevel=2;
  playedby[1]=KG_INPUTTYPE_INTERACTIVE;
  playedby[0]=KG_INPUTTYPE_PROCESS;
  // playedby[1]=KG_INPUTTYPE_INTERACTIVE;
  */


  isintro=1;
  server=false;
  port=7432;
  host="";
  remoteswitch=false;


  ClearStats();
}

LSkatDoc::~LSkatDoc()
{
}

void LSkatDoc::addView(LSkatView *view)
{
  pViewList->append(view);
}

void LSkatDoc::removeView(LSkatView *view)
{
  pViewList->remove(view);
}

void LSkatDoc::setAbsFilePath(const QString &filename)
{
  absFilePath=filename;
}

const QString &LSkatDoc::getAbsFilePath() const
{
  return absFilePath;
}

void LSkatDoc::setTitle(const QString &_t)
{
  title=_t;
}

const QString &LSkatDoc::getTitle() const
{
  return title;
}

void LSkatDoc::InitMove(LSkatView *sender,int player,int x,int y)
{
  LSkatView *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender) w->InitMove(player,x,y);
    }
  }

}
void LSkatDoc::slotUpdateAllViews(LSkatView *sender)
{
  LSkatView *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender)
        w->repaint();
    }
  }
}
void LSkatDoc::UpdateViews(int mode)
{
  LSkatView *w;
  if (IsIntro()) return ;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
       if (mode & UPDATE_STATUS) w->updateStatus();
    }
  }
}

bool LSkatDoc::saveModified()
{
  return true;
}

void LSkatDoc::closeDocument()
{
  deleteContents();
}

bool LSkatDoc::newDocument(KConfig *config,QString path)
{
  int res;
  modified=false;
  absFilePath=QDir::homeDirPath();
  title=i18n("Untitled");
  if (global_debug>1) printf("path=%s\n",path.latin1());
  res=LoadBitmap(path);
  if (res==0) return false;
  return true;
}

bool LSkatDoc::LoadGrafix(QString path)
{
  int res;
  res=LoadCards(cardPath);
  if (res==0) return false;
  res=LoadDeck(deckPath);
  if (res==0) return false;
  return true;
}

bool LSkatDoc::SetCardDeckPath(QString deck,QString card)
{
  bool update=false;
  if (!deck.isNull() && deck!=deckPath)
  {
    update=true;
    deckPath=deck;
    LoadDeck(deckPath);
  }
  if (!card.isNull() && card!=cardPath)
  {
    update=true;
    cardPath=card;
    LoadCards(cardPath);
  }
  return update;
}

bool LSkatDoc::openDocument(const QString &filename, const char *format /*=0*/)
{
  QFileInfo fileInfo(filename);
  title=fileInfo.fileName();
  absFilePath=fileInfo.absFilePath();	
  /////////////////////////////////////////////////
  // TODO: Add your document opening code here
  /////////////////////////////////////////////////
	
  modified=false;
  return true;
}

bool LSkatDoc::saveDocument(const QString &filename, const char *format /*=0*/)
{
  /////////////////////////////////////////////////
  // TODO: Add your document saving code here
  /////////////////////////////////////////////////

  modified=false;
  return true;
}

void LSkatDoc::deleteContents()
{
  /////////////////////////////////////////////////
  // TODO: Add implementation to delete the document contents
  /////////////////////////////////////////////////

}

// Called after game ends..give points to players
void LSkatDoc::EvaluateGame()
{
  if (score[0]+score[1]!=120)
  {
    printf("Warning: Score does not end up to 120\n");
  }
  stat_games[0]++;
  stat_games[1]++;
  if (score[0]==score[1]) // drawn
  {
    stat_points[0]++; 
    stat_points[1]++; 
  }
  else if (score[0]>score[1])
  {
    stat_won[0]++;
    stat_points[0]+=2;
    if (score[0]>=90) stat_points[0]++;  // Schneider
    if (score[0]>=120) stat_points[0]++; // SChwarz
  }
  else
  {
    stat_won[1]++;
    stat_points[1]+=2;
    if (score[1]>=90) stat_points[1]++;  // Schneider
    if (score[1]>=120) stat_points[1]++; // SChwarz
  }
}

void LSkatDoc::EndGame(bool aborted)
{
  if (aborted)
  {
    stat_brk[0]++;
    stat_brk[1]++;
  }
  else
  {
    startplayer=1-began_game;
  }
  isrunning=0;
  wasgame=true;
}

void LSkatDoc::NewGame()
{
   int i,r;
   int num[NO_OF_CARDS];

   isintro=0;

   began_game=startplayer;
   for (i=0;i<NO_OF_TILES;i++)
   {
     cardheight[i]=2;
   }
   trump=(CCOLOUR)(random(4));
   if (global_debug>5) printf("Trump=%d\n",trump);
   if (random(8)==0) trump=Grand; // grand

   // Fast drawing of random cards
   for (i=0;i<NO_OF_CARDS;i++)
   {
     num[i]=i;
   }
   for (i=0;i<NO_OF_CARDS;i++)
   {
     r=random(NO_OF_CARDS-i);      // cards available 32-i (+1)
     card[i]=num[r];
     num[r]=num[NO_OF_CARDS-i-1];  //  shift numbers
   }
   isrunning=1;
   movestatus=-1;
   currentplayer=startplayer;
   laststartplayer=-1;
   moveno=0;
   computerscore=0;
   lock=false;
   for (i=0;i<2;i++)
   {
     score[i]=0;
     curmove[i]=-1;
   }
}

int LSkatDoc::GetMove(int no) {return curmove[no];}

CCOLOUR LSkatDoc::GetTrump()
{
  return trump;
}
void LSkatDoc::SetTrump(CCOLOUR i)
{
  trump=i;
}

// pos=0..7, height=2..1..(0 no card left), player=0..1
int LSkatDoc::GetCard(int player, int pos,int height)
{
  int i;
  if (height==0) return -1;
  height=2-height;

  i=NO_OF_TILES*player+8*height+pos;
  return card[i];
}

// pos=0..7, player=0..1
int LSkatDoc::GetHeight(int player, int pos)
{
  int i;
  i=8*player+pos;
  return cardheight[i];
}
// pos=0..7, player=0..1
void LSkatDoc::SetHeight(int player, int pos,int h)
{
  int i;
  i=8*player+pos;
  cardheight[i]=h;
}


int LSkatDoc::random(int max)
{
double value;
int r;
  r=rand();
  value=(double)((double)r*(double)max)/(double)RAND_MAX;
  return (int)value;
}

void LSkatDoc::initrandom()
{
  srand( (unsigned)time( NULL ) ); // randomize
}

bool LSkatDoc::IsRunning() {return isrunning;}

bool LSkatDoc::LegalMove(int p1, int p2)
{
  CCOLOUR col1,col2,col3;
  CCARD card1,card2,card3;
  card1=(CCARD)((p1)/4);
  col1=(CCOLOUR)((p1)%4);
  card2=(CCARD)((p2)/4);
  col2=(CCOLOUR)((p2)%4);

  // force trump colour
  if (card1==Jack) col1=trump;
  if (card2==Jack) col2=trump;

  // same colour always ok
  if (col1==col2) return true;

  // Search for same colour
  bool flag=true;
  for (int i=0;i<8;i++)
  {
    int h,c;
    h=GetHeight(1-startplayer,i);
    if (h==0) continue;
    c=GetCard(1-startplayer,i,h);
    card3=(CCARD)((c)/4);
    col3=(CCOLOUR)((c)%4);
    if (card3==Jack) col3=trump;

    if (col3==col1)
    {
      flag=false;
      break;
    }
  }
  if (flag) return true;

  return false;
}

int LSkatDoc::PrepareMove(int player,int pos)
{
  int h,card;
  h=GetHeight(player,pos);
  if (h==0) return -1; // not possible
  if (player!=currentplayer) return -2; // wrong player

  card=GetCard(player,pos,h);

  // New round
  if (currentplayer==startplayer)
  {
    curmove[0]=-1;
    curmove[1]=-1;
  }
  else
  {
    if (!LegalMove(curmove[startplayer],card))
    {
      printf("Illegal move\n");
      return -3;
    }
  }
  lock=true;

  movestatus=card;
  SetHeight(player,pos,h-1);
  return 1;
}
int LSkatDoc::MakeMove()
{
  lock=false;
  curmove[currentplayer]=movestatus;
  if (currentplayer==startplayer)
  {
    moveno++;
    currentplayer=1-startplayer;
  }
  else
  {
    laststartplayer=startplayer;

    if (WonMove(curmove[startplayer],curmove[1-startplayer]))
    {
      // switch startplayer
      startplayer=1-startplayer;
    }
    else
    {
    }
    currentplayer=startplayer;
    score[startplayer]+=CardValue(curmove[0]);
    score[startplayer]+=CardValue(curmove[1]);
    if (moveno==NO_OF_TILES)
    {
      EndGame(false);
      return 2;
    }
  }
  movestatus=-1;
  return 1;
}
int LSkatDoc::CardValue(int card)
{
  int card1;

  card1=card/4;
  return cardvalues[card1];
}

int LSkatDoc::WonMove(int c1,int c2)
{
  CCOLOUR col1,col2;
  CCARD card1,card2;

  card1=(CCARD)((c1)/4);
  col1=(CCOLOUR)((c1)%4);
  card2=(CCARD)((c2)/4);
  col2=(CCOLOUR)((c2)%4);

  // Two jacks
  if (card1==Jack && card2==Jack)
  {
    if (col1<col2) return 0;
    else return 1;
  }
  // One Jack wins always
  if (card1==Jack) return 0;
  if (card2==Jack) return 1;

  // higher one wins if same colour
  if (col1==col2)
  {
    if (card1==Ten)
    {
      if (card2==Ace) return 1;
      else return 0;
    }
    if (card2==Ten)
    {
      if (card1==Ace) return 0;
      return 1;
    }

    if ((int)card1<(int)card2) return 0;
    return 1;
  }
  // trump wins
  if (col1==trump) return 0;
  if (col2==trump) return 1;

  // first one wins
  return 0;

}

int LSkatDoc::GetMoveStatus() {return movestatus;}
void LSkatDoc::SetMoveStatus(int i) { movestatus=i; }
int LSkatDoc::GetCurrentPlayer() {return currentplayer;}
void LSkatDoc::SetCurrentPlayer(int i) {currentplayer=i;}
int LSkatDoc::GetStartPlayer() {return startplayer;}
void LSkatDoc::SetStartPlayer(int i) {startplayer=i;}

void LSkatDoc::SetName(int no, QString n) { names[no]=n; }
QString LSkatDoc::GetName(int no)  {return names[no];}
int LSkatDoc::GetScore(int no) {return score[no];}

int LSkatDoc::GetMoveNo() {return moveno;}
/*
void LSkatDoc::SetDeckNo(int no) {deckno=no;}
int LSkatDoc::GetDeckNo() {return deckno;}
*/


int LSkatDoc::GetLastStartPlayer() {return laststartplayer;}

KG_INPUTTYPE LSkatDoc::GetPlayedBy(int no) {return playedby[no];}
void LSkatDoc::SetPlayedBy(int no,KG_INPUTTYPE type) {playedby[no]=type;}
int LSkatDoc::GetComputerLevel() {return computerlevel;}
void LSkatDoc::SetComputerLevel(int lev) {computerlevel=lev;}

void LSkatDoc::SetComputerScore(int sc) {computerscore=sc;}
int LSkatDoc::GetComputerScore() {return computerscore;}
void LSkatDoc::SetLock() {lock=true;}
void LSkatDoc::ReleaseLock() {lock=false;}
bool LSkatDoc::IsLocked() {return lock;}
bool LSkatDoc::IsIntro() {return isintro;}
bool LSkatDoc::WasRunning() {return wasgame;}
void LSkatDoc::SetIntro(bool b) {isintro=b;}

int LSkatDoc::LoadBitmap(QString path)
{
  int i;
  QString buf;
  if (global_debug>5) printf("Loading bitmaps\n");
  for (i=0;i<NO_OF_TRUMPS;i++)
  {
    buf.sprintf("%st%d.png",path.latin1(),i+1);
 		if(!mPixTrump[i].load(buf))
    {
		    printf("Fatal error: bitmap %s not found \n",buf.latin1());
    }
  }


  for (i=0;i<3;i++)
  {
    buf.sprintf("%stype%d.png",path.latin1(),i+1);
    if (!mPixType[i].load(buf))
    {
        printf("Fatal error: bitmap %s not found \n",buf.latin1());
    }
  }

  buf.sprintf("%sbackground.png",path.latin1());

  mPixBackground.load(buf);
  if (!mPixBackground.load(buf))
  {
      printf("Fatal error: bitmap %s not found \n",buf.latin1());
  }

  for (i=0;i<NO_OF_ANIM;i++)
  {
    buf.sprintf("%s4000%02d.png",path.latin1(),i);
 		if(!mPixAnim[i].load(buf))
    {
		    printf("Fatal error: bitmap %s not found \n",buf.latin1());
    }
  }
  return 1;
}


int LSkatDoc::LoadCards(QString path)
{
  QString buf;
  for (int i=0;i<NO_OF_CARDS;i++)
  {
    buf.sprintf("%s%d.png",path.latin1(),i+1);
 		if(!mPixCard[i].load(buf))
    {
		    printf("Fatal error: bitmap %s not found \n",buf.latin1());
        return 0;
    }
  }
  cardsize=mPixCard[0].size();
  return 1;
}

int LSkatDoc::LoadDeck(QString path)
{
  if (!mPixDeck.load(path))return 0;
  return 1;
}

void LSkatDoc::SetInputHandler(KEInput *i)
{
  inputHandler=i;
}
KEInput *LSkatDoc::QueryInputHandler()
{
  return inputHandler;
}

int LSkatDoc::SwitchStartPlayer()
{
  startplayer=1-startplayer;
  return startplayer;
}

bool LSkatDoc::IsServer()
{
  return server;
}
void LSkatDoc::SetServer(bool b)
{
  server=b;
}
void LSkatDoc::SetHost(QString h)
{
  host=h;
}
void LSkatDoc::SetPort(short p)
{
  port=p;
}
QString LSkatDoc::QueryHost()
{
  return host;
}
short LSkatDoc::QueryPort()
{
  return port;
}
int *LSkatDoc::GetCardP()
{
  return card;
}
int *LSkatDoc::GetCardHeightP()
{
  return cardheight;
}

void LSkatDoc::SetCard(int no,int c)
{
  card[no]=c;
}
void LSkatDoc::SetRemoteSwitch(bool b)
{
  remoteswitch=b;
}
bool LSkatDoc::IsRemoteSwitch()
{
  return remoteswitch;
}

int LSkatDoc::GetStatWon(int no)
{
  return stat_won[no];
}
int LSkatDoc::GetStatGames(int no)
{
  return stat_games[no];
}
int LSkatDoc::GetStatAborted(int no)
{
  return stat_brk[no];
}
int LSkatDoc::GetStatPoints(int no)
{
  return stat_points[no];
}

QString LSkatDoc::GetProcess()
{
  return procfile;
}

void LSkatDoc::ClearStats()
{
  for (int j=0;j<2;j++)
  {
    stat_won[j]=0;
    stat_points[j]=0;
    stat_games[j]=0;
    stat_brk[j]=0;
  }
}
void LSkatDoc::ReadConfig(KConfig *config)
{
  config->setGroup("Parameter");
  host=config->readEntry("host",QCString(""));
  port=(unsigned short)config->readNumEntry("port",7432);
  procfile=config->readEntry("process",QCString("lskatproc"));
  names[0]=config->readEntry("Name1",i18n("Alice"));
  names[1]=config->readEntry("Name2",i18n("Bob"));


  // This is for debug and testing as you can run it from the CVS without
  // installing the carddecks !
  // For the release version you can remove the aruments to the following two
  // functions !!!!
  cardPath=config->readEntry("cardpath", KCardDialog::getDefaultCardDir());
  deckPath=config->readEntry("deckpath", KCardDialog::getDefaultDeck());

  // Debug only
  if (global_debug>3)
    printf("cardPath=%s\ndeckPath=%s\n",cardPath.latin1(),deckPath.latin1());

  startplayer=config->readNumEntry("Startplayer",0);
  if (startplayer>1 || startplayer<0) startplayer=0;
  began_game=startplayer;
  computerlevel=config->readNumEntry("Level",2);
  playedby[0]=(KG_INPUTTYPE)config->readNumEntry("Player1",
                                (int)KG_INPUTTYPE_PROCESS);
  playedby[1]=(KG_INPUTTYPE)config->readNumEntry("Player2",
                                (int)KG_INPUTTYPE_INTERACTIVE);

  stat_won[0]=config->readNumEntry("Stat1W",0);
  stat_won[1]=config->readNumEntry("Stat2W",0);
  stat_brk[0]=config->readNumEntry("Stat1B",0);
  stat_brk[1]=config->readNumEntry("Stat2B",0);
  stat_points[0]=config->readNumEntry("Stat1P",0);
  stat_points[1]=config->readNumEntry("Stat2P",0);
  stat_games[0]=config->readNumEntry("Stat1G",0);
  stat_games[1]=config->readNumEntry("Stat2G",0);
}

/** write config file */
void LSkatDoc::WriteConfig(KConfig *config)
{
  config->setGroup("Parameter");
  config->writeEntry("host",host);
  config->writeEntry("port",port);
  config->writeEntry("process",procfile);
  config->writeEntry("tmppath",picpath);
  config->writeEntry("delpath",delpath);
  config->writeEntry("Name1",names[0]);
  config->writeEntry("Name2",names[1]);

  config->writeEntry("Startplayer",startplayer);
  config->writeEntry("Level",computerlevel);
  config->writeEntry("Player1",(int)playedby[0]);
  config->writeEntry("Player2",(int)playedby[1]);

  config->writeEntry("Stat1W",stat_won[0]);
  config->writeEntry("Stat2W",stat_won[1]);
  config->writeEntry("Stat1B",stat_brk[0]);
  config->writeEntry("Stat2B",stat_brk[1]);
  config->writeEntry("Stat1P",stat_points[0]);
  config->writeEntry("Stat2P",stat_points[1]);
  config->writeEntry("Stat1G",stat_games[0]);
  config->writeEntry("Stat2G",stat_games[1]);

  config->writeEntry("cardpath",cardPath);
  config->writeEntry("deckpath",deckPath);

  config->sync();
}

#include "lskatdoc.moc"
