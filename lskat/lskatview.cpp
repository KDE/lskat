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
#include <qpixmap.h>
#include <qcolor.h>
#include <qrect.h>
#include <qpoint.h>
#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <kmessagebox.h>
#include <kdebug.h>
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

// Probably most unnecessary and unused now
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

#define MOVECOUNTER  20      // so many moves when playing card
#define MOVE_TIMER_DELAY 7   // timer in milllisec default 7

LSkatView::LSkatView(QWidget *parent, const char *name) : QWidget(parent, name)
{
  setBackgroundMode(PaletteBase);
  // setBackgroundMode(NoBackground);

  status_rect1=QRect(412,CARD_Y_OFFSET+5,180,95+20);
  status_rect2=QRect(412,310,180,95+20);
  status_rect3=QRect(CARD_X_OFFSET+60,CARD_Y_OFFSET+5+100+15+20,
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

    if (cardmoveunder>=0)
    {
      // turn new card
      if ((double)cardmovecnt/(double)MOVECOUNTER>0.5) 
      {
        QPixmap pix1(getDocument()->mPixCard[cardmoveunder]);
        int wid=pix1.width();
        QWMatrix m;
        m.scale(2.0*((double)cardmovecnt/(double)MOVECOUNTER-0.5),1.0);
        pix1=pix1.xForm(m);
        point=QPoint((wid-pix1.width())/2,0);
        p->drawPixmap(cardorigin+point,pix1);
      }
      // turn deck
      else
      {
        QPixmap pix1(getDocument()->mPixDeck);
        int wid=pix1.width();
        QWMatrix m;
        m.scale(1.0-2.0*((double)cardmovecnt/(double)MOVECOUNTER),1.0);
        pix1=pix1.xForm(m);
        point=QPoint((wid-pix1.width())/2,0);
        p->drawPixmap(cardorigin+point,pix1);
      }
    } /* end turn card */

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
    font.setPixelSize(48);

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
    s=i18n("Lieutnant Skat");
    x=160;
    y=-20+(int)(200.0*sin(0.5*M_PI/(float)NO_OF_CARDS*cnt));
    p->setPen(QColor(col4,col2,0));
    p->drawText(x-2,y+2,s);

    p->setPen(QColor(col3,col,0));
    p->drawText(x,y,s);

    s=i18n("for");
    y=270+(NO_OF_CARDS-cnt)*3;
    x=-50+(int)(340.0*sin(0.5*M_PI/(float)NO_OF_CARDS*cnt));
    p->setPen(QColor(col4,col2,0));
    p->drawText(x-2,y+2,s);

    p->setPen(QColor(col3,col,0));
    p->drawText(x,y,s);

    s=i18n("K D E");
    y=350+(NO_OF_CARDS-cnt)*3;
    x=640-(int)(380.0*sin(0.5*M_PI/(float)NO_OF_CARDS*cnt));
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
          p->drawPixmap(point-QPoint(CARD_X_DECK,CARD_Y_DECK),getDocument()->mPixDeck);

        if (cardmovecnt<1 || x!=cardmovex || y!=cardmovey)
        {
          if (height>0 ) p->drawPixmap(point,getDocument()->mPixCard[card]);
        }
        else
        {
           // moved to drawMove
           // if (height>0 ) p->drawPixmap(point,getDocument()->mPixDeck);
        }
      }
      else
      {
          p->drawPixmap(point,getDocument()->mPixDeck);
      }
    }
  }
}
// Draw the winner field
void LSkatView::drawFinal(QPainter *p)
{
  int sc1,sc0,pt0,pt1;
  QPoint p1,p2;
  int trump;
  QRect r;
  QString ld;
  int ts[10];

  QFont font24(QCString("Helvetica"),24,QFont::Normal,false);
  QFont font14(QCString("Helvetica"),14,QFont::Normal,false);
  //QFont font12(QCString("Helvetica"),12,QFont::Normal,false);
  font24.setPixelSize(24);
  font14.setPixelSize(14);

	//p1=status_rect3.topLeft();
	//p2=status_rect3.bottomRight();

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


  
  QString line1,line2,line3,line4,line5;
  QString col1_3,col2_3,col3_3,col4_3;
  QString col1_4,col2_4,col3_4,col4_4;
  QRect sumrect;
  QRect rect;
  QRect brect1,brect2,brect3,brect4,brect5;
  QRect brect1_3,brect2_3,brect3_3,brect4_3;
  QRect brect1_4,brect2_4,brect3_4,brect4_4;

  // Calculate geometry
  line1=QString(i18n("Game over"));
  rect=p->window();
  //rect1.moveBy(0,FINAL_Y0-24);
  p->setFont(font24);
  brect1=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line1);
  QRect wrect=p->window();
  sumrect=brect1;

  if (sc0+sc1!=120)  // aborted
  {
    line2=i18n("Game was aborted - no winner");
    int hp=getDocument()->mPixTrump[trump].height()+5;
    rect=QRect(0,hp>sumrect.height()?hp:sumrect.height(),p->window().width(),p->window().height());
    p->setFont(font14);
    brect2=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line2);
    sumrect|=brect2;
  }
  else
  {
    if (sc0==sc1)
    {
      line2=i18n("      Game is drawn");
    }
    else if (sc0>sc1)
    {
      line2=i18n("Player 1 - %1 won ").arg(getDocument()->GetName(0));
    }
    else
    {
      line2=i18n("Player 2 - %1 won ").arg(getDocument()->GetName(1));
    }
    int hp=getDocument()->mPixTrump[trump].height()+5;
    rect=QRect(0,hp>sumrect.height()?hp:sumrect.height(),p->window().width(),p->window().height());
    p->setFont(font14);
    brect2=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line2);
    sumrect|=brect2;

    p->setFont(font14);
    col1_3=i18n("Score:");
    col1_4=QString(" ");
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect1_3=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col1_3);
    ts[0]=brect1_3.width()+10;

    col2_3=getDocument()->GetName(0);
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect2_3=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col2_3);

    col2_4=getDocument()->GetName(1);
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect2_4=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col2_4);
    rect=brect2_3|brect2_4;
    ts[1]=ts[0]+rect.width()+10;

    
    col3_3.sprintf("%d",sc0);
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect3_3=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col3_3);

    col3_4.sprintf("%d",sc1);
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect3_4=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col3_4);
    rect=brect3_3|brect3_4;
    ts[2]=ts[1]+rect.width()+30;

    col4_3=i18n("%1 points").arg(pt0);
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect4_3=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col4_3);

    col4_4=i18n("%1 points").arg(pt1);
    rect=QRect(0,0,p->window().width(),p->window().height());
    brect4_4=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,col4_4);

    rect=brect4_3|brect4_4;
    ts[3]=ts[2]+rect.width()+10;
    ts[4]=0;

    line3=col1_3+QString("\t")+col2_3+QString("\t")+col3_3+QString("\t")+col4_3;
    line4=col1_4+QString("\t")+col2_4+QString("\t")+col3_4+QString("\t")+col4_4;
    brect3=QRect(sumrect.left(),sumrect.bottom()+10,ts[3],brect4_3.height());
    brect4=QRect(sumrect.left(),sumrect.bottom()+10+brect4_3.height()+6,ts[3],brect4_4.height());
    sumrect|=brect3;
    sumrect|=brect4;


    p->setFont(font14);
    if (sc0>=120)
    {
      line5=i18n("%1 won to nil.  Congratulations!").arg(getDocument()->GetName(0));
      rect=QRect(0,sumrect.height()+10,p->window().width(),p->window().height());
      brect5=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line5);
      sumrect|=brect5;
    }
    else if (sc0>=90)
    {
      line5=i18n("%1 won over 90 points. Super!").arg(getDocument()->GetName(0));
      rect=QRect(0,sumrect.height()+10,p->window().width(),p->window().height());
      brect5=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line5);
      sumrect|=brect5;
    }
    if (sc1>=120)
    {
      line5=i18n("%1 won to nil.  Congratulations!").arg(getDocument()->GetName(1));
      rect=QRect(0,sumrect.height()+10,p->window().width(),p->window().height());
      brect5=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line5);
      sumrect|=brect5;
    }
    else if (sc1>=90)
    {
      line5=i18n("%1 won over 90 points. Super!").arg(getDocument()->GetName(1));
      rect=QRect(0,sumrect.height()+10,p->window().width(),p->window().height());
      brect5=p->boundingRect(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line5);
      sumrect|=brect5;
    }
  }
  
  QPoint offset=QPoint(status_rect3.left()-sumrect.left(),status_rect3.top());
  sumrect.moveBy(offset.x(),offset.y());
  
  // draw actual strings and symbols
  QRect borderrect=QRect(sumrect.left()-20,sumrect.top()-20,sumrect.width()+40,sumrect.height()+40);
  p->drawRect(borderrect);
  drawBorder(p,borderrect,0,4,0);
  drawBorder(p,borderrect,10,1,1);


  p->setPen(black);
  p->setFont(font24);
  rect=sumrect;
  rect.setTop(brect1.top()+offset.y());
  //brect1.moveBy(offset.x(),offset.y());
  p->drawText(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line1);

  p->setFont(font14);
  p->setPen(COL_PLAYER);
  rect=sumrect;
  rect.setTop(brect2.top()+offset.y());
  //brect2.moveBy(offset.x(),offset.y());
  p->drawText(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line2);

  p->drawPixmap(sumrect.topLeft()+QPoint(-5,-5), getDocument()->mPixTrump[trump]);
  p->drawPixmap(sumrect.topLeft()+QPoint(5,-5)+QPoint(sumrect.width()-getDocument()->mPixTrump[trump].width(),0),
                getDocument()->mPixTrump[trump]);


  if (!col1_3.isNull())
  {
    p->setTabArray(ts);
    p->setFont(font14);
    p->setPen(Qt::black);
    rect=sumrect;
    rect.setTop(brect3.top()+offset.y());
    p->drawText(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop|Qt::ExpandTabs,line3);
    rect=sumrect;
    rect.setTop(brect4.top()+offset.y());
    p->drawText(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop|Qt::ExpandTabs,line4);
  }
  if (!line5.isNull()) 
  {
    p->setFont(font14);
    p->setPen(Qt::black);
    rect=sumrect;
    rect.setTop(brect5.top()+offset.y());
    p->drawText(rect,Qt::AlignHCenter|Qt::SingleLine|Qt::AlignTop,line5);
  }


}

// Draw the status field at the right side
void LSkatView::drawStatus(QPainter *p)
{
  QPoint p1,p2;
  int trump;
  QRect drawrect;
  // For loop
  QRect srect[2];
  srect[0]=status_rect1;
  srect[1]=status_rect2;

  QFont font10(QCString("Helvetica"),11,QFont::Normal,false);
  font10.setPixelSize(14);
  p->setFont(font10);

  trump=getDocument()->GetTrump();


  // draw text
  QString ld;
  QString line1,line2,line3,line4,line2a,line2b,line2c;
  QRect sumrect,rect,rect2,brect1,brect2,brect3,brect4;
  QPoint pa;

  for (int pl=0;pl<2;pl++)
  {
    drawrect=QRect(srect[pl].left()+14,srect[pl].top()+14,srect[pl].width()-28,srect[pl].height()-28);
    p1=drawrect.topLeft();
    p2=drawrect.bottomRight();

    // Draw border and field
    p->setPen(COL_STATUSBORDER);
    p->setBrush(COL_STATUSFIELD);
    p->drawRect(srect[pl]);
    drawBorder(p,srect[pl],0,4,0);
    drawBorder(p,srect[pl],10,1,1);


    // Player pl -------------------
    // line1=QString(i18n("Player 1"))+QString(QCString(" - "))+getDocument()->GetName(0);
    line1=getDocument()->GetName(pl);
    brect1=p->boundingRect(drawrect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line1);
    sumrect=brect1;

    if (getDocument()->IsRunning())
    {
      // Geometry and strings
      line2=i18n("Score:");
      rect=QRect(drawrect.left(),sumrect.bottom()+16,drawrect.width(),drawrect.height()-sumrect.height());
      brect2=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2);
      sumrect|=brect2;
      line3=i18n("Move:");
      rect=QRect(drawrect.left(),sumrect.bottom()+10,drawrect.width(),drawrect.height()-sumrect.height());
      brect3=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line3);
      sumrect|=brect3;

      rect=brect2|brect3;
      rect.setWidth(rect.width()+10);
      line2a.sprintf("%3d",getDocument()->GetScore(pl));
      line2b.sprintf("%3d",getDocument()->GetMoveNo());
      
      // paint
      if (getDocument()->GetStartPlayer()==pl) p->setPen(COL_PLAYER);
      else p->setPen(black);
      p->drawText(brect1,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line1);
      p->setPen(black);
      p->drawText(brect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2);
      p->drawText(brect3,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line3);
      rect2=QRect(brect2.left()+rect.width(),brect2.top(),drawrect.width()-brect2.width()-rect.width(),brect2.height());
      p->drawText(rect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2a);
      rect2=QRect(brect3.left()+rect.width(),brect3.top(),drawrect.width()-brect3.width()-rect.width(),brect3.height());
      p->drawText(rect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2b);
    
      pa=QPoint(drawrect.width()-getDocument()->mPixTrump[trump].width(),drawrect.height()-getDocument()->mPixTrump[trump].height());
      if (getDocument()->GetStartPlayer()==pl)
        p->drawPixmap(p1+pa+QPoint(3,3),getDocument()->mPixTrump[trump]);

      pa=QPoint(drawrect.width()-getDocument()->mPixType[getDocument()->GetPlayedBy(pl)-1].width(),0);
      p->drawPixmap(p1+pa+QPoint(3,-3), getDocument()->mPixType[getDocument()->GetPlayedBy(pl)-1]);
    }
    else // draw all time score
    {
      // Geometry and strings
      line2=i18n("Points:");
      rect=QRect(drawrect.left(),sumrect.bottom()+6,drawrect.width(),drawrect.height()-sumrect.height());
      brect2=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2);
      sumrect|=brect2;

      line3=i18n("Won:");
      rect=QRect(drawrect.left(),sumrect.bottom()+6,drawrect.width(),drawrect.height()-sumrect.height());
      brect3=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line3);
      sumrect|=brect3;

      line4=i18n("Games:");
      rect=QRect(drawrect.left(),sumrect.bottom()+6,drawrect.width(),drawrect.height()-sumrect.height());
      brect4=p->boundingRect(rect,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line4);
      sumrect|=brect4;

      rect=brect2|brect3|brect4;
      rect.setWidth(rect.width()+10);
      line2a.sprintf("%d",getDocument()->GetStatPoints(pl));
      line2b.sprintf("%d",getDocument()->GetStatWon(pl));
      line2c.sprintf("%d",getDocument()->GetStatGames(pl));

      // paint
      p->setPen(COL_PLAYER);
      p->drawText(brect1,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line1);
      p->setPen(black);
      p->drawText(brect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2);
      p->drawText(brect3,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line3);
      p->drawText(brect4,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line4);
      rect2=QRect(brect2.left()+rect.width(),brect2.top(),drawrect.width()-brect2.width()-rect.width(),brect2.height());
      p->drawText(rect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2a);
      rect2=QRect(brect3.left()+rect.width(),brect3.top(),drawrect.width()-brect3.width()-rect.width(),brect3.height());
      p->drawText(rect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2b);
      rect2=QRect(brect4.left()+rect.width(),brect4.top(),drawrect.width()-brect4.width()-rect.width(),brect4.height());
      p->drawText(rect2,Qt::AlignLeft|Qt::SingleLine|Qt::AlignTop,line2c);

      pa=QPoint(drawrect.width()-getDocument()->mPixType[getDocument()->GetPlayedBy(pl)-1].width(),0);
      p->drawPixmap(p1+pa+QPoint(3,-3), getDocument()->mPixType[getDocument()->GetPlayedBy(pl)-1]);

    }

  }// end pl
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
     KMessageBox::information(this,m);
  }
}


void LSkatView::InitMove(int player,int mx,int my)
{
    cardmovex=mx;
    cardmovey=my+2*player;
    cardmoveunder=getDocument()->GetCard(player,mx+4*my,getDocument()->GetHeight(player,mx+4*my));
    introTimer->stop();
    moveTimer->start(MOVE_TIMER_DELAY,FALSE);
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
    cardmovecnt++;
    pos=cardorigin+(cardend-cardorigin)*cardmovecnt/MOVECOUNTER;
    update(QRect(pos,getDocument()->cardsize));
    // Turning of the card
    if ( cardmoveunder>=0)
    {
      update(QRect(cardorigin,getDocument()->cardsize)); 
    }
  }
}

void LSkatView::updateStatus()
{
  update(status_rect1);
  update(status_rect2);
}

#include "lskatview.moc"
