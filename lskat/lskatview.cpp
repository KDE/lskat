/***************************************************************************
                          lskatview.cpp  -  description
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
#include <qprinter.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qrect.h>
#include <qpoint.h>
#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <math.h>
#include <stdio.h>


// application specific includes
#include "lskatview.h"
#include "lskatdoc.h"
#include "lskat.h"
#include "KEInput.h"
#include "KEMessage.h"

#define CARD_X_OFFSET 32   // offset fromleft side
#define CARD_Y_OFFSET 5   // offset from top
#define CARD_X_SPACE  10   // space between cards
#define CARD_Y_SPACE  5   // space between cards
#define CARD_Y_BOARD_OFFSET 20  // space between players

#define CARD_X_DECK   5    // shift deck and card
#define CARD_Y_DECK   3    // shift deck and card

#define CARD_X_MOVE1  430  // positon of first moved card
#define CARD_Y_MOVE1  140
#define CARD_X_MOVE2  450  // psoiton of second moved card
#define CARD_Y_MOVE2  190

#define STATUS_X0     15   // pos in status win
#define STATUS_X1     60
#define STATUS_X2     100
#define STATUS_Y0     26
#define STATUS_Y1     44
#define STATUS_Y2     62
#define STATUS_Y3     80
#define STATUS_XT     128
#define STATUS_YT     47
#define STATUS_XTYPE  135
#define STATUS_YTYPE  12

#define FINAL_XT1     15 
#define FINAL_XT2     -55
#define FINAL_YT      15

#define FINAL_X0      140
#define FINAL_Y0      35

#define FINAL_X1      110
#define FINAL_Y1      90

#define FINAL_X2      130
#define FINAL_Y2      65

#define FINAL_X3      30
#define FINAL_X4      90
#define FINAL_X5      170
#define FINAL_X6      230

#define FINAL_Y3      95
#define FINAL_Y4      125

#define FINAL_X7      30  
#define FINAL_Y7      155

//#define COL_STATUSLIGHT  white
#define COL_STATUSBORDER black
//#define COL_STATUSFIELD  QColor(192,192,192)
//#define COL_STATUSDARK   QColor(65,65,65)
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)
#define COL_PLAYER       QColor(255,255,0)

#define DLGBOXTITLE  TITLE

#define MOVECOUNTER  20   // so many moves when playing card


LSkatView::LSkatView(QWidget *parent, const char *name) : QWidget(parent, name)
{
  setBackgroundMode(PaletteBase);
  // setBackgroundMode(NoBackground);

  status_rect1=QRect(400,CARD_Y_OFFSET+5,180,100);
  status_rect2=QRect(400,325,180,100);
  status_rect3=QRect(CARD_X_OFFSET+30,CARD_Y_OFFSET+5+100+15,
                     400,320-100-CARD_Y_OFFSET-30);



  setBackgroundColor(QColor(0,0,128));
  setBackgroundPixmap( getDocument()->mPixBackground );

  moveTimer=new QTimer(this);
  moveTimer->stop();
  connect(moveTimer,SIGNAL(timeout()),this,SLOT(moveTimerReady()));

  introcnt=0;
  introTimer=new QTimer(this);
  introTimer->stop();
  connect(introTimer,SIGNAL(timeout()),this,SLOT(introTimerReady()));
  introTimer->start(75,FALSE);

  for (int i=0;i<NO_OF_CARDS;i++)
  {
    introCards[i]=getDocument()->random(NO_OF_CARDS);
  }
}

LSkatView::~LSkatView()
{
}

LSkatDoc *LSkatView::getDocument() const
{
  LSkatApp *theApp=(LSkatApp *) parentWidget();

  return theApp->getDocument();
}

void LSkatView::print(QPrinter *pPrinter)
{
  QPainter printpainter;
  printpainter.begin(pPrinter);
	
  // TODO: add your printing code here

  printpainter.end();
}

// draw a border around a rect
void LSkatView::drawBorder(QPainter *p,QRect rect,int offset,int width,int mode)
{
QPen graypen;
int i;
QPoint p1,p2;

	if (mode!=0 && mode!=1) return;

	p1=rect.topLeft();
	p2=rect.bottomRight();

	if (mode==0) p->setPen(COL_STATUSLIGHT);
	else if (mode==1) p->setPen(COL_STATUSDARK);
	
	for (i=0;i<width;i++)
	{
		p->moveTo(p1.x()+offset+i,p2.y()-offset-i);
		p->lineTo(p1.x()+offset+i,p1.y()+offset+i);
		p->lineTo(p2.x()-offset-i,p1.y()+offset+i);
	}
	if (mode==1) p->setPen(COL_STATUSLIGHT);
	else if (mode==0) p->setPen(COL_STATUSDARK);
	for (i=0;i<width;i++)
	{
		p->moveTo(p1.x()+offset+i,p2.y()-offset-i);
		p->lineTo(p2.x()-offset-i,p2.y()-offset-i);
		p->lineTo(p2.x()-offset-i,p1.y()+offset+i);
	}
}

void LSkatView::drawMove(QPainter *p)
{
  int m1,m2;
  int below;
  QPoint point1,point2;

  m1=getDocument()->GetMove(0);
  m2=getDocument()->GetMove(1);
  point1=QPoint(CARD_X_MOVE1,CARD_Y_MOVE1);
  point2=QPoint(CARD_X_MOVE2,CARD_Y_MOVE2);

  below=getDocument()->GetLastStartPlayer();
  if (below<0) below=getDocument()->GetStartPlayer();

  if (below==0)
  {
    if (m1>=0) p->drawPixmap(point1,getDocument()->mPixCard[m1]);
    if (m2>=0) p->drawPixmap(point2,getDocument()->mPixCard[m2]);
  }
  else
  {
    if (m2>=0) p->drawPixmap(point2,getDocument()->mPixCard[m2]);
    if (m1>=0) p->drawPixmap(point1,getDocument()->mPixCard[m1]);
  }

  int card;
  QPoint point;
  card=getDocument()->GetMoveStatus();
  if (card>=0)
  {
    point=cardorigin+(cardend-cardorigin)*cardmovecnt/MOVECOUNTER;
    p->drawPixmap(point,getDocument()->mPixCard[card]);
  }
}

void LSkatView::drawIntro(QPainter *p)
{
    int i,c1,c2,x,cnt,y,col,col2,col3,col4;
    QPoint point,point1,p2;
    QString s;
    QFont font(QCString("lucida"),48,QFont::Normal,false);

    p->setFont(font);


    cnt=introcnt;
    if (cnt>NO_OF_CARDS) cnt=NO_OF_CARDS;
    

    i=0;
    point=QPoint(20,20);
    point1=QPoint(550,20);

    for (i=0;i<cnt;i++)
    {
      x=(int)((i+10)*5*sin(1.5*i*M_PI/(float)NO_OF_CARDS));
      c1=introCards[i];
      c2=introCards[(i+17)%NO_OF_CARDS];
      p2=QPoint(i*10+x, i*10);
      p->drawPixmap(point+p2,getDocument()->mPixCard[c1]);
      p2=QPoint(-i*10-x, i*10);
      p->drawPixmap(point1+p2,getDocument()->mPixCard[c2]);
    }

    col=255-8*i;
    if (col<0) col=0;
    col2=2*col/3;
    col3=255-2*i;
    col4=2*col3/3;
    s=TITLE;
    x=160;
    y=-20+(int)(200.0*sin(0.5*M_PI/(float)NO_OF_CARDS*cnt));
    p->setPen(QColor(col4,col2,0));
    p->drawText(x-2,y+2,s);

    p->setPen(QColor(col3,col,0));
    p->drawText(x,y,s);

    s=i18n("by");
    y=270+(NO_OF_CARDS-cnt)*3;
    x=-50+(int)(340.0*sin(0.5*M_PI/(float)NO_OF_CARDS*cnt));
    p->setPen(QColor(col4,col2,0));
    p->drawText(x-2,y+2,s);

    p->setPen(QColor(col3,col,0));
    p->drawText(x,y,s);

    s=QCString("Martin Heni");
    y=350+(NO_OF_CARDS-cnt)*3;
    x=640-(int)(450.0*sin(0.5*M_PI/(float)NO_OF_CARDS*cnt));
    p->setPen(QColor(col4,col2,0));
    p->drawText(x-2,y+2,s);

    p->setPen(QColor(col3,col,0));
    p->drawText(x,y,s);
}

void LSkatView::drawDeck(QPainter *p)
{
  int x,y,card;
  int player,pos,height;
  QPoint point;

  for (y=0;y<4;y++)
  {
    if (y<2) player=0;
    else player=1;

    for (x=0;x<4;x++)
    {
      pos=x+4*(y%2);

      point=calcCardPos(x,y);
      if (getDocument()->IsRunning())
      {
        height=getDocument()->GetHeight(player,pos);
        card=getDocument()->GetCard(player,pos,height);


        if (height==2)
          p->drawPixmap(point-QPoint(CARD_X_DECK,CARD_Y_DECK),getDocument()->mPixDeck[getDocument()->GetDeckNo()]);

        if (cardmovecnt<1 || x!=cardmovex || y!=cardmovey)
        {
          if (height>0 ) p->drawPixmap(point,getDocument()->mPixCard[card]);
        }
        else
        {
          if (height>0 ) p->drawPixmap(point,getDocument()->mPixDeck[getDocument()->GetDeckNo()]);
        }
      }
      else
      {
          p->drawPixmap(point,getDocument()->mPixDeck[getDocument()->GetDeckNo()]);
      }
    }
  }
}
// Draw the winner field
void LSkatView::drawFinal(QPainter *p)
{
  int sc1,sc0,pt0,pt1;
  QString ld;
  QPoint p1,p2;
  int trump;

  QFont font24(QCString("Helvetica"),24,QFont::Normal,false);
  QFont font14(QCString("Helvetica"),14,QFont::Normal,false);
  QFont font12(QCString("Helvetica"),12,QFont::Normal,false);

	p1=status_rect3.topLeft();
	p2=status_rect3.bottomRight();

  sc0=getDocument()->GetScore(0);
  sc1=getDocument()->GetScore(1);

  if (sc0>=120) pt0=4;
  else if (sc0>=90) pt0=3;
  else if (sc0>=60) pt0=2;
  else if (sc0==60) pt0=1;
  else pt0=0;
  if (sc1>=120) pt1=4;
  else if (sc1>=90) pt1=3;
  else if (sc1>=60) pt1=2;
  else if (sc1==60) pt1=1;
  else pt1=0;

  trump=getDocument()->GetTrump();

  p->drawRect(status_rect3);
  drawBorder(p,status_rect3,0,4,0);
  drawBorder(p,status_rect3,10,1,1);

  p->setPen(black);
  p->setFont(font24);
  
  ld=QString(i18n("Game over"));
  p->drawText(p1.x()+FINAL_X0,p1.y()+FINAL_Y0,ld);

  p->drawPixmap(p1+QPoint(FINAL_XT1,FINAL_YT),
    getDocument()->mPixTrump[trump]);
  p->drawPixmap(p1+QPoint(FINAL_XT2-p1.x()+p2.x(),FINAL_YT),
    getDocument()->mPixTrump[trump]);


  if (sc0+sc1!=120)  // aborted
  {
    p->setFont(font14);
    p->setPen(COL_PLAYER);
    ld=QString(i18n("Game was aborted - no winner"));
    p->drawText(p1.x()+FINAL_X1,p1.y()+FINAL_Y1,ld);
  }
  else
  {
    p->setFont(font14);
    p->setPen(COL_PLAYER);
    if (sc0==sc1)
    {
      ld=QString(i18n("      Game is drawn"));
      p->drawText(p1.x()+FINAL_X2,p1.y()+FINAL_Y2,ld);
    }
    else if (sc0>sc1)
    {
      ld=i18n("Player 1 - %1 won ").arg(getDocument()->GetName(0));
      p->drawText(p1.x()+FINAL_X2,p1.y()+FINAL_Y2,ld);
    }
    else
    {
      ld=i18n("Player 2 - %1 won ").arg(getDocument()->GetName(1));
      p->drawText(p1.x()+FINAL_X2,p1.y()+FINAL_Y2,ld);
    }
    p->setFont(font14);
    p->setPen(Qt::black);

    ld=i18n("Score:");
    p->drawText(p1.x()+FINAL_X3,p1.y()+FINAL_Y3,ld);

    ld=getDocument()->GetName(0);
    p->drawText(p1.x()+FINAL_X4,p1.y()+FINAL_Y3,ld);

    ld=getDocument()->GetName(1);
    p->drawText(p1.x()+FINAL_X4,p1.y()+FINAL_Y4,ld);

    ld.sprintf("%d",sc0);
    p->drawText(p1.x()+FINAL_X5,p1.y()+FINAL_Y3,ld);

    ld.sprintf("%d",sc1);
    p->drawText(p1.x()+FINAL_X5,p1.y()+FINAL_Y4,ld);

    ld.sprintf("%d",pt0);
    ld+=i18n(" points");
    p->drawText(p1.x()+FINAL_X6,p1.y()+FINAL_Y3,ld);

    ld.sprintf("%d",pt1);
    ld+=i18n(" points");
    p->drawText(p1.x()+FINAL_X6,p1.y()+FINAL_Y4,ld);

    if (sc0>=120)
    {
      ld=i18n("%1 won to nil.  Congratulations!").arg(getDocument()->GetName(0));
      p->drawText(p1.x()+FINAL_X7,p1.y()+FINAL_Y7,ld);
    }
    else if (sc0>=90)
    {
      ld=i18n("%1 won over 90 points. Super!").arg(getDocument()->GetName(0));
      p->drawText(p1.x()+FINAL_X7,p1.y()+FINAL_Y7,ld);
    }
    if (sc1>=120)
    {
      ld=i18n("%1 won to nil.  Congratulations!").arg(getDocument()->GetName(1));
      p->drawText(p1.x()+FINAL_X7,p1.y()+FINAL_Y7,ld);
    }
    else if (sc1>=90)
    {
      ld=i18n("%1 won over 90 points. Super!").arg(getDocument()->GetName(1));
      p->drawText(p1.x()+FINAL_X7,p1.y()+FINAL_Y7,ld);
    }


  }

}

// Draw the status field at the right side
void LSkatView::drawStatus(QPainter *p)
{
  QPoint p1,p2;
  int trump;

  trump=getDocument()->GetTrump();

  // Draw border and field
  p->setPen(COL_STATUSBORDER);
  p->setBrush(COL_STATUSFIELD);

  p->drawRect(status_rect1);
  drawBorder(p,status_rect1,0,4,0);
	drawBorder(p,status_rect1,10,1,1);

  p->drawRect(status_rect2);
  drawBorder(p,status_rect2,0,4,0);
	drawBorder(p,status_rect2,10,1,1);

  // draw text
  QString ld;

	p->setPen(black);

  // Player 1 -------------------
	p1=status_rect1.topLeft();
	p2=status_rect1.bottomRight();

  if (getDocument()->GetStartPlayer()==0) p->setPen(COL_PLAYER);
  ld=QString(i18n("Player 1"))+QString(QCString(" - "))+getDocument()->GetName(0);
	p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y0,ld);
  p->setPen(black);

  if (getDocument()->IsRunning())
  {
    ld=i18n("Score:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y1,ld);
    ld.sprintf("%3d",getDocument()->GetScore(0));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y1,ld);

    ld=i18n("Move:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y2,ld);
    ld.sprintf("%3d",getDocument()->GetMoveNo());
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y2,ld);

    if (getDocument()->GetStartPlayer()==0)
      p->drawPixmap(p1+QPoint(STATUS_XT,STATUS_YT),getDocument()->mPixTrump[trump]);
  }
  else // draw all time score
  {
    ld=i18n("Points:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y1,ld);
    ld.sprintf("%d",getDocument()->GetStatPoints(0));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y1,ld);

    ld=i18n("Won:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y2,ld);
    ld.sprintf("%d",getDocument()->GetStatWon(0));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y2,ld);

    ld=i18n("Games:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y3,ld);
    ld.sprintf("%d",getDocument()->GetStatGames(0));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y3,ld);

    /*
    ld=QString("1000")+i18n(" aborted");
    p->drawText(p1.x()+STATUS_X2,p1.y()+STATUS_Y3,ld);
    ld.sprintf("%3d",getDocument()->GetMoveNo());
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y3,ld);
    */

  }

  p->drawPixmap(p1+QPoint(STATUS_XTYPE,STATUS_YTYPE),
        getDocument()->mPixType[getDocument()->GetPlayedBy(0)-1]);

  // Player 2 -------------------
	p->setPen(black);
	p1=status_rect2.topLeft();
	p2=status_rect2.bottomRight();

  if (getDocument()->GetStartPlayer()==1) p->setPen(COL_PLAYER);
  ld=QString(i18n("Player 2"))+QString(QCString(" - "))+getDocument()->GetName(1);
	p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y0,ld);
  p->setPen(black);

  if (getDocument()->IsRunning())
  {
    ld=i18n("Score:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y1,ld);
    ld.sprintf("%3d",getDocument()->GetScore(1));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y1,ld);

    ld=i18n("Move:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y2,ld);
    ld.sprintf("%3d",getDocument()->GetMoveNo());
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y2,ld);

    if (getDocument()->GetStartPlayer()==1)
      p->drawPixmap(p1+QPoint(STATUS_XT,STATUS_YT),getDocument()->mPixTrump[trump]);
  }
  else
  {
    ld=i18n("Points:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y1,ld);
    ld.sprintf("%d",getDocument()->GetStatPoints(1));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y1,ld);

    ld=i18n("Won:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y2,ld);
    ld.sprintf("%d",getDocument()->GetStatWon(1));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y2,ld);

    ld=i18n("Games:");
    p->drawText(p1.x()+STATUS_X0,p1.y()+STATUS_Y3,ld);
    ld.sprintf("%d",getDocument()->GetStatGames(1));
    p->drawText(p1.x()+STATUS_X1,p1.y()+STATUS_Y3,ld);
  }

  p->drawPixmap(p1+QPoint(STATUS_XTYPE,STATUS_YTYPE),
        getDocument()->mPixType[getDocument()->GetPlayedBy(1)-1]);

}
// paint function
void LSkatView::Paint(QPainter *p)
{
    // If game is running
    drawStatus(p);
    drawDeck(p);
    if (getDocument()->IsRunning())
    {
      drawMove(p);
    }
    else if (getDocument()->WasRunning() && !getDocument()->IsIntro())
    {
      drawFinal(p);
    }
}

// paint event
void LSkatView::paintEvent( QPaintEvent * e)
{
  if (getDocument()->IsIntro())
  {
    QPixmap pm(this->rect().size());
    QPainter p;
    QBrush brush;
    p.begin(&pm,this);
     brush.setPixmap( getDocument()->mPixBackground );
    p.fillRect(0,0,this->rect().width(),this->rect().height(),brush);
    drawIntro(&p);
    p.end();
    bitBlt(this,0,0,&pm);
  }
  else
  {
    QPainter paint( this );
    paint.setClipRect(e->rect());
    Paint( &paint );			
  }
}


// x=0..4, y=0..4
QPoint LSkatView::calcCardPos(int x,int y)
{
  QPoint point;
    point=QPoint(CARD_X_OFFSET+x*(CARD_X_SPACE+getDocument()->cardsize.width()),
                 CARD_Y_OFFSET+y*(CARD_Y_SPACE+getDocument()->cardsize.height()));
    if (y>=2) point+=QPoint(0,CARD_Y_BOARD_OFFSET);
    point+=QPoint(CARD_X_DECK,CARD_Y_DECK);
  return point;
}

// mouse click event
void LSkatView::mousePressEvent( QMouseEvent *mouse )
{
  if (mouse->button()!=LeftButton) return ;
  if (!getDocument()->IsRunning()) return ;

  if (getDocument()->GetMoveStatus()!=-1) return ;

  QPoint point;
  int mx,my,player;

  point=mouse->pos();
  point-=QPoint(CARD_X_OFFSET,CARD_Y_OFFSET);
  if (point.y()>2*(getDocument()->cardsize.height()+CARD_Y_SPACE))
  {
    point-=QPoint(0,CARD_Y_BOARD_OFFSET+2*(getDocument()->cardsize.height()+CARD_Y_SPACE));
    player=1;
  }
  else
  {
     player=0;
  }
  if (point.x()<0 || point.y()<0) return ;
  mx=point.x()/(getDocument()->cardsize.width()+CARD_X_SPACE);
  my=point.y()/(getDocument()->cardsize.height()+CARD_Y_SPACE);
  if (mx>3 || my>1) return ;

  if (global_debug>10)
    printf("Type=%d Next=%d player=%d\n",
      getDocument()->QueryInputHandler()->QueryType(player),
      getDocument()->QueryInputHandler()->QueryNext(),player);
  if (getDocument()->QueryInputHandler()->IsInteractive(player) &&
      getDocument()->QueryInputHandler()->QueryNext()==player &&
      !getDocument()->IsLocked())
  {
    KEMessage *msg=new KEMessage;
    msg->AddData(QCString("Move"),(short)player);
    msg->AddData(QCString("MoveX"),(short)mx);
    msg->AddData(QCString("MoveY"),(short)my);
    getDocument()->QueryInputHandler()->SetInput(msg);
    delete msg;
  }
  else
  {
    QString m;
    switch(getDocument()->random(4))
    {
      case 0:
        m=i18n("Hold on..the other was not yet gone...");
      break;
      case 1:
        m=i18n("Hold your horses...");
      break;
      case 2:
        m=i18n("Ah ah ah...only one go at a time...");
      break;
      default:
        m=i18n("Please wait... it is not your turn.");
    }
     KMessageBox::information(this,m, TITLE);
  }
}


void LSkatView::InitMove(int player,int mx,int my)
{
    cardmovex=mx;
    cardmovey=my+2*player;
    introTimer->stop();
    moveTimer->start(7,FALSE);
    cardmovecnt=0;
    cardorigin=calcCardPos(mx,my+2*player);
    if (getDocument()->GetCurrentPlayer()==0)
      cardend=QPoint(CARD_X_MOVE1,CARD_Y_MOVE1);
    else cardend=QPoint(CARD_X_MOVE2,CARD_Y_MOVE2);
    update(QRect(
            cardorigin-QPoint(CARD_X_DECK,CARD_Y_DECK),
            getDocument()->cardsize+QSize(CARD_X_DECK,CARD_Y_DECK)));

    QPoint point1=QPoint(CARD_X_MOVE1,CARD_Y_MOVE1);
    QPoint point2=QPoint(CARD_X_MOVE2,CARD_Y_MOVE2);
    update(QRect(point1,getDocument()->cardsize));
    update(QRect(point2,getDocument()->cardsize));
}
void LSkatView::introTimerReady()
{
  if (!getDocument()->IsIntro())
  {
    introTimer->stop();
    return ;
  }
  introcnt++;
  if (introcnt>NO_OF_CARDS*4)
  {  
    introcnt=0;
    for (int i=0;i<NO_OF_CARDS;i++)
    {
      introCards[i]=getDocument()->random(NO_OF_CARDS);
    }
  }
  else if (introcnt<NO_OF_CARDS)
  {
    repaint(false);
  }
}

void LSkatView::moveTimerReady()
{
  QPoint pos;
  QString ld,s;
  if (cardmovecnt>=MOVECOUNTER)
  {
    LSkatApp *m=(LSkatApp *) parentWidget();
    moveTimer->stop();
    cardmovecnt=0;
    update(QRect(cardend,getDocument()->cardsize));
    update(QRect(cardorigin,getDocument()->cardsize));
    update(status_rect1);
    update(status_rect2);
    m->MoveFinished();

  }
  else
  {
    pos=cardorigin+(cardend-cardorigin)*cardmovecnt/MOVECOUNTER;
     update(QRect(pos,getDocument()->cardsize));
    // repaint(QRect(pos,getDocument()->cardsize));
    cardmovecnt++;
    pos=cardorigin+(cardend-cardorigin)*cardmovecnt/MOVECOUNTER;
    update(QRect(pos,getDocument()->cardsize));
    //repaint(QRect(pos,getDocument()->cardsize));
  }
}

void LSkatView::updateStatus()
{
  update(status_rect1);
  update(status_rect2);
}
