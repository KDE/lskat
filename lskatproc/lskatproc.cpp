/***************************************************************************
                          lskatproc.cpp  -  description
                             -------------------
    begin                : Sun Apr 9 2000
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
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lskatproc.h"

#define MIN_TIME 1000  // usec

// ------------ class game ---------------------------------
lgame::lgame()
{
 int i;
 for (i=0;i<14;i++) cardvalues[i]=0;
 cardvalues[(int)Ace]=11;
 cardvalues[(int)Ten]=10;
 cardvalues[(int)King]=4;
 cardvalues[(int)Queen]=3;
 cardvalues[(int)Jack]=2;
 curmove[0]=-1;
 curmove[1]=-1;
 score[0]=0;
 score[1]=0;
 level=0;
 endgame=false;
 for (i=0;i<NO_OF_CARDS;i++) played[i]=0;
}

lgame::lgame(lgame &game)
{
  *this=game;
}

lgame &lgame::operator=(lgame &game)
{
  int i;
  currentplayer=game.currentplayer;
  startplayer=game.startplayer;
  trump=game.trump;
  movenumber=game.movenumber;
  score[0]=game.score[0];
  score[1]=game.score[1];
  curmove[0]=game.curmove[0];
  curmove[1]=game.curmove[1];
  for (i=0;i<NO_OF_CARDS;i++) played[i]=game.played[i];
  for (i=0;i<NO_OF_CARDS;i++) card[i]=game.card[i];
  for (i=0;i<16;i++) cardheight[i]=game.cardheight[i];
  endgame=game.endgame;
  level=game.level;
  return *this;
}

int lgame::MakeMove(int c,int pos)
{
  int h;
  curmove[currentplayer]=c;
  h=GetHeight(currentplayer,pos);
  if (currentplayer==startplayer)
  {
    movenumber++;
    SetHeight(currentplayer,pos,h-1);
    currentplayer=1-startplayer;
  }
  else
  {
    if (!LegalMove(curmove[startplayer],c)) return -1;
    SetHeight(currentplayer,pos,h-1);
    if (WonMove(curmove[startplayer],curmove[1-startplayer]))
    {
      // switch startplayer
      startplayer=1-startplayer;
    }
    currentplayer=startplayer;
    score[startplayer]+=CardValue(curmove[0]);
    score[startplayer]+=CardValue(curmove[1]);

    if (movenumber==NO_OF_TILES)
    {
      endgame=true;
      return 2;
    }
  }
  return 1;
}

void lgame::Init()
{
  int player,i,h,j,card;
  // check what cards are played
  for (player=0;player<2;player++)
  {
    for (i=0;i<8;i++)
    {
      h=GetHeight(player,i);
      for (j=h;j<2;j++)
      {
        card=GetCard(player,i,j);
        played[card]=1;
      }
    }
  }
}

// Add value of both cards to the score of startplayer
void lgame::AddScore(int c1,int c2)
{
  score[startplayer]+=CardValue(c1);
  score[startplayer]+=CardValue(c2);
}
// Switch the startplayer
void lgame::SwitchStartplayer()
{
  startplayer=1-startplayer;
}

// pos=0..7, player=0..1
void lgame::SetHeight(int player, int pos,int h)
{
  int i;
  i=8*player+pos;
  cardheight[i]=h;
}
bool lgame::LegalMove(int p1, int p2)
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
int lgame::CardValue(int card)
{
  int card1;

  card1=card/4;
  return cardvalues[card1];
}
int lgame::WonMove(int c1,int c2)
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
// pos=0..7, height=2..1..(0 no card left), player=0..1
int lgame::GetCard(int player, int pos,int height)
{
  int i;
  if (height==0) return -1;
  height=2-height;

  i=NO_OF_TILES*player+8*height+pos;
  return card[i];
}



// pos=0..7, player=0..1
int lgame::GetHeight(int player, int pos)
{
  int i;
  i=8*player+pos;
  return cardheight[i];
}

// Returns a value for the given side
// applies all rules
int lgame::Subvalue(int side)
{
  int sc,card;
  int i,h,c;
  CCOLOUR col1;
  CCARD card1;
  int trum1;
  int jack1;
  int havecol[4];
  bool haveten[4];
  bool haveace[4];
  bool havejack[4];

  sc=0;
  trum1=0;
  for (i=0;i<4;i++)
  {
    havecol[i]=false;
    haveten[i]=false;
    haveace[i]=false;
    havejack[i]=false;
  }
  jack1=0;
  for (i=0;i<8;i++)
  {
    h=GetHeight(side,i);
    c=GetCard(side,i,h);
    if (c<0) continue;

    card1=(CCARD)((c)/4);
    col1=(CCOLOUR)((c)%4);

    if (col1==trump) trum1++;
    havecol[(int)col1]++;
    if (card1==Ten)
    {
      haveten[(int)col1]=true;
    }
    else if (card1==Ace)
    {
      haveace[(int)col1]=true;
    }
    else if (card1==Jack)
    {
      havejack[(int)col1]=true;
      jack1++;
    }
    if (col1!=trump)
    {
      if (card1==Seven) sc-=60;
      if (card1==Eight) sc-=50;
      if (card1==Nine) sc-=40;
      if (card1==Queen) sc-=10;
    }
  }
  for (i=0;i<4;i++)
  {
    if (havecol[i]==0 && i!=trump) sc+=1000;
    if (havecol[i]>5) sc+=800;

    if (haveten[i]&&havecol[i]<2)
    {
      card=8*i+Ace;
      if (!played[card] && !haveace[i]) sc-=2500; // free ten
    }
    if (haveace[i]) sc+=1500;    // ace
    if (havejack[i])
    {
      if (trump==Grand) sc+=4000+300*(4-i);
      else sc+=2700+100*(4-i);
    }
  }
  // evaluate
  sc+=trum1*2500;
  if (trum1==0) sc-=7000;
  else if (trum1==1) sc-=5000;
  return sc;
}

int lgame::Value(int player)
{
  int sc;
  sc=0;

  // Someone won?
  if (score[0]>90) sc+=90000;
  else if (score[0]>60) sc+=70000;
  else if (score[0]==60) sc+=40000;

  if (score[1]>90) sc-=90000;
  else if (score[1]>60) sc-=70000;
  else if (score[1]==60) sc-=40000;

  // Reward points
  sc+=(score[0]-score[1])*650;

  // Calulate cards
  sc+=Subvalue(0);
  sc-=Subvalue(1);

  // random
  sc+=random(500)-250;

  if (player==1) return -sc;
  return sc;
}


// -------------class lskatproc ----------------------------
lskatproc::lskatproc()
  : KInputChildProcess(4096)
{

 initrandom();
}

lskatproc::~lskatproc(){
}



bool lskatproc::ReceiveMsg(KEMessage* msg,int id)
{
// time_t timee,timea;
short x,y;

  SendDebug("Receiv Msg");
  // end of process
  if (msg->HasKey(QCString("Terminate")))
  {
    Terminate();
  }
  // Init of process
  if (msg->HasKey(QCString("Init")))
  {
    // No init necessary
  }
  // Make a move
  if (msg->HasKey(QCString("Cards")))
  {
    SendDebug("Process HasKey(Cards)");
    // new game object
    lgame game;
    // extract data from message
    game.ExtractGame(msg);
    game.Init();  // must be AFTER ExtractGame

    // Debug stuff only
    sprintf(buf,"Trump=%d move=%d sc1=%d sc2=%d",
      game.trump,game.curmove[1-game.currentplayer],game.score[0],game.score[1]);
    SendDebug(buf);

    if (game.currentplayer==0 && game.startplayer==0)
      sprintf(buf,"+++ Computer ACTS as player ONE\n");
    else if (game.currentplayer==0 && game.startplayer==1)
      sprintf(buf,"+++ Computer REACTS as player ONE\n");
    else if (game.currentplayer==1 && game.startplayer==1)
      sprintf(buf,"+++ Computer ACTS as player TWO\n");
    else
      sprintf(buf,"+++ Computer REACTS as player TWO\n");
    SendDebug(buf);

    // fills data
    x=0;y=0;
    GetComputerMove(game,x,y,0);
    sprintf(buf,"Computer move player=%d x=%d y=%d",game.currentplayer,x,y);
    SendDebug(buf);


    // report move
    msg->RemoveAll();
    msg->AddData(QCString("Move"),game.currentplayer);
    msg->AddData(QCString("MoveX"),x);
    msg->AddData(QCString("MoveY"),y);

    //timee=time(0);
    // Sleep a minimum amount to slow down moves
    //if ( 1000*(timee-timea) < MIN_TIME) usleep((MIN_TIME-1000*(timee-timea)));
    SendDebug("Sending move back to main");

    if (!IsTerminated()) SendMsg(msg);
    fflush(stdout); // I do not know why it is needed..send does it too?
  }

  return true;
}


/* --------------------------------------------------------------------------- */
/* Computer Routinen														   */
/* --------------------------------------------------------------------------- */

// extract game from msg
int lgame::ExtractGame(KEMessage *msg)
{
  int i;
  short tmp;
  char *p;
  int size;

    msg->GetData(QCString("Startplayer"),startplayer);
    msg->GetData(QCString("CurrentPlayer"),currentplayer);
    msg->GetData(QCString("Cards"),p,size);
    msg->GetData(QCString("Level"),level);
    level--; // start with level 0
    for (i=0;i<NO_OF_CARDS;i++)
    {
      card[i]=((int *)p)[i];
    }
    msg->GetData(QCString("Height"),p,size);
    for (i=0;i<NO_OF_TILES;i++)
    {
      cardheight[i]=((int *)p)[i];
    }
    msg->GetData(QCString("Trump"),tmp);
    trump=(CCOLOUR)tmp;
    short mm;
    msg->GetData(QCString("CurrentMove"),mm);
    curmove[1-currentplayer]=(int)mm;
    curmove[currentplayer]=-1;
    msg->GetData(QCString("No"),movenumber);
    msg->GetData(QCString("Sc1"),score[0]);
    msg->GetData(QCString("Sc2"),score[1]);
    return 1;
}

long lgame::random(long max)
{
double value;
int r;
  r=rand();
  value=(double)((double)r*(double)max)/(double)RAND_MAX;
  return (long)value;
}

void lskatproc::initrandom()
{
	srand( (unsigned)time( NULL ) ); // randomize
}


int lskatproc::GetComputerMove(lgame game,short &x,short &y,int rek)
{
  int i,maxvalue,maxmove,h,c;
  //short oldscore;
  bool startflag;
  int startplayer;
  int value;
  lgame cgame;
  char sbuf[100];
  short mx,my;


  for (i=0;i<2*rek;i++) sbuf[i]=' ';
  sbuf[2*rek]=0;

  x=0;
  y=0;
  if (game.currentplayer==game.startplayer) startflag=true;
  else startflag=false;

  startplayer=game.startplayer;

  maxmove=0;
  maxvalue=LOWERT;

  sprintf(buf,"%s:Prepareing computer move (cur=%d) startflag=%d",
       sbuf,game.currentplayer,startflag);
  //SendDebug(buf);
  for (i=0;i<8;i++)
  {
    sprintf(buf,"%s:Checking for card %d of player %d\n",sbuf,i,game.currentplayer);
   // SendDebug(buf);
    cgame=game;
    h=cgame.GetHeight(cgame.currentplayer,i);
    if (h<1)
    {
      sprintf(buf,"%s:i=%d:: no cards left",sbuf,i);
    //  SendDebug(buf);
      continue;  // no cards left
    }
    c=cgame.GetCard(cgame.currentplayer,i,h);
    if (cgame.MakeMove(c,i)<0)
    {
      sprintf(buf,"%s:i=%d:: no legal move c1=%d c2=%d",
        sbuf,i,cgame.curmove[cgame.startplayer],c);
     // SendDebug(buf);
      continue; // wrong card
    }
    if (!startflag) // we are second
    {
      sprintf(buf,"LEVEL %d %d",cgame.level,rek);
      SendDebug(buf);
      // Still recursion necessary and game not yet ended?
      if (rek<2*cgame.level && !cgame.endgame)
      {
        // If we have the same startplayer the movesequence
        // is not switched and we can take the negativ value
        // otherwise we play again, and have to take the poitiv value
        if (cgame.startplayer==startplayer)
        {
          value=-GetComputerMove(cgame,mx,my,rek+1);
          // if (value==-LOWERT) value=LOWERT; // no move possible
        }
        else
          value=GetComputerMove(cgame,mx,my,rek+1);
      }
      else // evaluate position
      {
        value=cgame.Value(1-startplayer);
      }
    }
    else // we are first player
    {
      // Alwayss the other player moves now
      value=-GetComputerMove(cgame,mx,my,rek+1);
    }

    sprintf(buf,"%s:i=%d:: Value=%d",sbuf,i,value);
    SendDebug(buf);

    if (value>maxvalue)
    {
      maxvalue=value;
      maxmove=i;
    }
  }
  x=maxmove%4;
  y=maxmove/4;
  return maxvalue;
}

void lskatproc::SendDebug(const char *s)
{
  KEMessage *msg=new KEMessage;
  msg->AddData(QCString("Debug"),s);
//  msg->AddData("KLogSendMsg","debug.log");
// DEBUG
//  SendMsg(msg);
//  printf("%s\n",s);

  delete msg;
}
