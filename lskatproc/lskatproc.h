/***************************************************************************
                          lskatproc.h  -  description
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

#ifndef LSKATPROC_H
#define LSKATPROC_H
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "KInputChildProcess.h"

/**
  *@author Martin Heni
  */



#define LOWERT 	-999999999L
#define SIEG_WERT  9999999L


#define START_REK 1    // (0) 1:Nur Stellungsbewertung bei Level 1
					   //     0:Level 1 schon eine Rekursion

typedef enum {Club=0,Spade=1,Heart=2,Diamond=3,Grand=4} CCOLOUR;
typedef enum {Ace=0,King=1,Queen=2,Jack=3,Ten=4,Nine=5,Eight=6,Seven=7} CCARD;
#define NO_OF_CARDS 32
#define NO_OF_TILES 16
#define NO_OF_TRUMPS 5

class lgame 
{
  public:
  lgame();
  lgame(lgame &game);
  lgame &operator=(lgame &game);
  
  int WonMove(int c1,int c2);
  int CardValue(int card);
  bool LegalMove(int p1, int p2);
  void SetHeight(int player, int pos,int h);
  int GetHeight(int player, int pos);
  int GetCard(int player, int pos,int height);
  int Value(int player);
  void AddScore(int c1,int c2);
  void SwitchStartplayer();
  int MakeMove(int c,int pos);
  void Init();
  long random(long max);
  int Subvalue(int side);
  int ExtractGame(KEMessage *msg);
  
  short currentplayer;
  short startplayer;
  int card[NO_OF_CARDS];
  int cardheight[16];
  int cardvalues[14];
  short score[2];
  CCOLOUR trump;
  short movenumber;
  int curmove[2];
  bool endgame;
  int played[NO_OF_CARDS]; // cards already played
  short level;
};


class lskatproc : public KInputChildProcess
{

private:

public:
	lskatproc();
	~lskatproc();
	
  virtual bool ReceiveMsg(KEMessage *msg,int id);


  void initrandom();
  int GetComputerMove(lgame game,short &x,short &y,int rek);
  void SendDebug(const char *s);

  private:
  char buf[1024];
};

#endif
