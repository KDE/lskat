/*
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
*/

// Include files for Qt
#include <QString>
#include <QDir>
#include <QFile>

// Include files for KDE
#include <kapplication.h>
#include <kstdgameaction.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <khelpmenu.h>
#include <kdebug.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
#include <kicon.h>
#include <kmenubar.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kcarddialog.h>
#include <krandom.h>
#include <kglobal.h>
#include <ktoolbar.h>

// Application specific includes
#include "mainwindow.h"
#include "canvasview.h"
#include "abstractengine.h"
#include "engine_two.h"
#include "display_two.h"
#include "config_two.h"
#include "display_intro.h"
#include "deck.h"
#include "player.h"
#include "mouseinput.h"
#include "aiinput.h"
#include "playerstatuswidget.h"
#include "namedialogwidget.h"

// Forward declarations
#define ADVANCE_PERDIOD 20

#define ACTION(x)   (actionCollection()->action(x))

using namespace InputDevice;

/**
 * Construct the main application window 
 */
Mainwindow::Mainwindow(QWidget* parent) 
          : KMainWindow(parent, 0)
//, view(0), engine(0), mChat(0), mMyChatDlg(0)
{
  // Reset stuff
  mDeck    = 0;
  mEngine  = 0;
  mDisplay = 0;
  mView    = 0;


  // Find resource directory
  QString file=QString::fromLatin1("lskat/grafix/t0.png");
  mGrafixDir = kapp->dirs()->findResourceDir("data", file)+"lskat/grafix/";

  // Debug
  kDebug() << "GrafixDir="<<mGrafixDir << endl;

  // Our config
  mConfig=KGlobal::config();

  // Overall view
  mCanvas        = new QGraphicsScene(this);
  mView          = new CanvasView(QSize(880, 675), ADVANCE_PERDIOD, mCanvas, this);

  // Create menus etc
  initGUI();

  // Create GUI
  setupGUI(Keys|Create);
  toolBar()->hide();
  statusBar()->show();
  adjustSize();
  setAutoSaveSettings(); 

  // The LSkat config
  mLSkatConfig = new ConfigTwo(this);
  connect(mLSkatConfig, SIGNAL(signalInputType(int,InputDeviceType)),
          this, SLOT(setInputType(int,InputDeviceType)));
  mLSkatConfig->reset();

  // Read game properties and set default values
  readProperties(mConfig);

  // Get the card deck
  long seed = KRandom::random();
  kDebug() << "Random seed " << seed << endl;
  mDeck = new Deck(seed, this);
  mDeck->loadCards(mCardDir);
  mDeck->loadCardBackside(mDeckGrafix);
  mDeck->loadTrump(mGrafixDir);


  // Create intro
  mGameMode      = Intro;
  mDisplay       = new DisplayIntro(mGrafixDir, mDeck, mCanvas, this);
  mDisplay->setAdvancePeriod(ADVANCE_PERDIOD);
  setCentralWidget(mView);

  adjustSize();

  // Start intro
  mDisplay->start();

  statusBar()->showMessage(i18n("Welcome to Skat! Please start a new game."));

}

// Destructor
Mainwindow::~Mainwindow()
{
  if (mEngine) delete mEngine;
  if (mDisplay) delete mDisplay;
  if (mLSkatConfig) delete mLSkatConfig;
  if (mDeck) delete mDeck;
  if (mView) delete mView;
  kDebug() << "Destructor Mainwindow done" << endl;
}

// Called by KMainWindow when the last window of the application is 
bool Mainwindow::queryExit()
{
  if (mEngine)
  {
    mEngine->stopGame();
  }
  saveProperties(mConfig);
  return true;
}


// Save properties
void Mainwindow::saveProperties(KConfig *cfg)
{
    cfg->setGroup("ProgramData");
    cfg->writePathEntry("carddir", mCardDir);
    cfg->writePathEntry("deck",    mDeckGrafix);
    cfg->writeEntry("startplayer", mStartPlayer);
    mLSkatConfig->save(cfg);
}


// Load properties
void Mainwindow::readProperties(KConfig* cfg)
{
  cfg->setGroup("ProgramData");

  // Get default card data
  QString dcd = KCardDialog::getDefaultCardDir();
  dcd = KGlobal::dirs()->findResourceDir("cards", dcd)+dcd;
  QString dd = KCardDialog::getDefaultDeck();
  dd = KGlobal::dirs()->findResourceDir("cards", dd)+dd;

  // Read card path
  mCardDir    = cfg->readPathEntry("carddir", dcd);
  mDeckGrafix = cfg->readPathEntry("deck", dd);

  // Check for path existence
  QFile file(mDeckGrafix);
  if (!file.exists()) mDeckGrafix = dd;
  QDir dir(mCardDir);
  if (!dir.exists()) mCardDir = dcd;

  kDebug() << "set mDeckGrafix=" << mDeckGrafix << endl;
  kDebug() << "set mCardDir=" << mCardDir << endl;

  int no = cfg->readEntry("startplayer", 0);
  setStartPlayer(no);
  mLSkatConfig->load(cfg);
}



// Create a input with the given type
AbstractInput* Mainwindow::createInput(
                                 InputDeviceType inputType, 
                                 AbstractDisplay* display,
                                 AbstractEngine* engine)
{
   AbstractInput* input = 0;

  // Create the player input
  if (inputType == TypeMouseInput)
  {
    MouseInput* mouseInput = new MouseInput(mGrafixDir, this);
    connect((QObject*)mView, SIGNAL(signalLeftMousePress(QPoint)),
            mouseInput, SLOT(mousePress(QPoint)));
    connect(mouseInput, SIGNAL(signalConvertMousePress(QPoint,int&,int&)),
            display, SLOT(convertMousePress(QPoint,int&,int&)));
    connect(mouseInput, SIGNAL(signalPlayerInput(int,int,int)),
            engine, SLOT(playerInput(int,int,int) ));
    input = mouseInput;
    kDebug() << "Create MOUSE INPUT " << endl;
  }
  else if (inputType == TypeAiInput)
  {
    AiInput* aiInput = new AiInput((EngineTwo*)engine, mGrafixDir, this);
    connect(aiInput, SIGNAL(signalPlayerInput(int,int,int)),
            engine, SLOT(playerInput(int,int,int) ));
    input = aiInput;
    kDebug() << "Create AI INPUT " << endl;
  }
  else
  {
    kFatal() << "Unpupported input device type " << inputType << endl;
  }

  return input;         
}


// Start a new game 
void Mainwindow::startGame()
{   
  // Deal cards to player - Shuffle card deck and reset pile
  mDeck->shuffle();

  // Draw Trump
  Suite trump = mDeck->randomTrump();

  // Loop all players in the game
  QHashIterator<int,Player*> it = mLSkatConfig->playerIterator();
  while(it.hasNext())
  {
    it.next();
    Player* player = it.value();
    player->setDeck(mDeck);
    // Deal cards
    player->deal(16);
    // Store trump
    player->setTrump(trump);
  }

  // Start display 
  mDisplay->start();

  // Start the game engine
  mEngine->startGame(trump, mStartPlayer); 

  // Start player for next game
  setStartPlayer(1-mStartPlayer);

  statusBar()->clearMessage();
}

// Here a game over is signalled
void Mainwindow::gameOver(int winner)
{
  kDebug() << "GameOver:: Winner= " << winner << endl;
  statusBar()->showMessage(i18n("Please start a new game."));
}


// Setup the GUI 
void Mainwindow::initGUI()
{
  // Start a new game
  KStdGameAction::gameNew(this, SLOT(menuNewLSkatGame()),
                          actionCollection(), "new_game");
  ACTION("new_game")->setToolTip(i18n("Starting a new game..."));
  ACTION("new_game")->setWhatsThis(i18n("Start a new game."));

  // Clear all time statistics
  KAction* clearStatAct = new KAction( KIcon("flag"), i18n("&Clear Statistics"), actionCollection(), "clear_statistics");
  connect(clearStatAct, SIGNAL(triggered(bool)), this, SLOT(menuClearStatistics()));
  clearStatAct->setToolTip(i18n("Delete all time statistics..."));
  clearStatAct->setWhatsThis(i18n("Clears the all time statistics which is kept in all sessions."));

  // End a game
  KAction* endGameAct = new KAction( KIcon("stop"), i18n("End game"), actionCollection(), "end_game");
  connect(endGameAct, SIGNAL(triggered(bool)), this, SLOT(menuEndGame()));
  endGameAct->setToolTip(i18n("Ending the current game..."));
  endGameAct->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));

  // Quite the program
  KStdGameAction::quit(this, SLOT(close()),
                       actionCollection(), "game_exit");
  ACTION("game_exit")->setToolTip(i18n("Exiting..."));
  ACTION("game_exit")->setWhatsThis(i18n("Quits the program."));


  // Determine start player
  KSelectAction* startPlayerAct = new KSelectAction(i18n("Starting Player"), actionCollection(), "startplayer");
  connect(startPlayerAct, SIGNAL(triggered(int)), this, SLOT(menuStartplayer()));
  startPlayerAct->setToolTip(i18n("Changing starting player..."));
  startPlayerAct->setWhatsThis(i18n("Chooses which player begins the next game."));
  QStringList list;
  list.clear();
  list.append(i18n("Player &1"));
  list.append(i18n("Player &2"));
  startPlayerAct->setItems(list);

  
  // Determine who player player 1
  KSelectAction* player1Act = new KSelectAction(i18n("Player &1 Played By"), actionCollection(), "player1");
  connect(player1Act, SIGNAL(triggered(int)), this, SLOT(menuPlayer1By()));
  player1Act->setToolTip(i18n("Changing who plays player 1..."));
  player1Act->setWhatsThis(i18n("Changing who plays player 1."));
  list.clear();
  list.append(i18n("&Mouse"));
  list.append(i18n("&Computer"));
  player1Act->setItems(list);

  // Determine who player player 2
  KSelectAction* player2Act = new KSelectAction(i18n("Player &2 Played By"), actionCollection(), "player2");
  connect(player2Act, SIGNAL(triggered(int)), this, SLOT(menuPlayer2By()));
  player2Act->setToolTip(i18n("Changing who plays player 2..."));
  player2Act->setWhatsThis(i18n("Changing who plays player 2."));
  player2Act->setItems(list);

 // Choose card deck
 KAction* selectDeckAct = new KAction(i18n("Select &Card Deck..."), actionCollection(), "select_carddeck");
  connect(selectDeckAct, SIGNAL(triggered(bool)), this, SLOT(menuCardDeck()));
  selectDeckAct->setToolTip(i18n("Configure card decks..."));
  selectDeckAct->setWhatsThis(i18n("Choose how the cards should look."));

  // Change player names
  KAction* changeNamesAct = new KAction(i18n("&Change Player Names"), actionCollection(), "change_names");
  connect(changeNamesAct, SIGNAL(triggered(bool)), this, SLOT(menuPlayerNames()));
}


// Slots

// Choose start player
void Mainwindow::menuStartplayer()
{
  int i=((KSelectAction *)ACTION("startplayer"))->currentItem();
  setStartPlayer(i);
}

// Select input for player 1
void Mainwindow::menuPlayer1By()
{
  int i = ((KSelectAction *)ACTION("player1"))->currentItem();
  mLSkatConfig->setInputType(0, (InputDeviceType)i);
}

// Select input for player 2
void Mainwindow::menuPlayer2By()
{
  int i = ((KSelectAction *)ACTION("player2"))->currentItem();
  mLSkatConfig->setInputType(1, (InputDeviceType)i);
}

// Choose a card deck
void Mainwindow::menuCardDeck()
{
  QString s1,s2;
  int result;

  result=KCardDialog::getCardDeck(s1,s2);
  if (result==QDialog::Accepted)
  {
    kDebug() << "NEW CARDDECK: " << s1 << " and " << s2 << endl;
    bool change = false; // Avoid unnecessary changes
    if (!s1.isEmpty() && s1 != mDeckGrafix)
    {
      mDeckGrafix = s1;
      mDeck->loadCardBackside(mDeckGrafix);
      change = true;
    }
    if (!s2.isEmpty() && s2 != mCardDir)
    {
      mCardDir    = s2;
      mDeck->loadCards(mCardDir);
      change = true;
    }
    if (change)
    {
      mDisplay->updateSpriteGraphics();
      mView->update(); // Be on the safe side and update
    }
  }
}

// Clear all time statistics
void Mainwindow::menuClearStatistics()
{
   QString message;
   message=i18n("Do you really want to clear the all time "
                "statistical data?");

  if (KMessageBox::Yes==KMessageBox::questionYesNo(this,
                                                   message,
                                                   QString::null,
                                                   KStdGuiItem::clear()))
  {
    QHashIterator<int,Player*> it = mLSkatConfig->playerIterator();
    while(it.hasNext())
    {
      it.next();
      Player* player = it.value();
      player->clear();
    }
  }
}

// Abort a game
void Mainwindow::menuEndGame()
{
   if (mEngine)
   {
     mEngine->stopGame();
   }
}


// Start a new game
void Mainwindow::menuNewLSkatGame()
{

  Player* p1 = mLSkatConfig->player(0);
  Player* p2 = mLSkatConfig->player(1);

  // Stop running games
  if (mEngine)
  {
    mEngine->stopGame();
  }

  // Get rid of old stuff?
  if (mGameMode != LSkat)
  {
    // Set new game mode
    mGameMode = LSkat;

    // Start deleting
    if (mDisplay) delete mDisplay;
    if (mEngine) delete mEngine;


    // Create new stuff
    PlayerStatusWidget* playerWidget1 = new PlayerStatusWidget(p1, this);
    PlayerStatusWidget* playerWidget2 = new PlayerStatusWidget(p2, this);
    mView->setStatusWidget(0, playerWidget1);
    mView->setStatusWidget(2, playerWidget2);


    mDisplay     = new DisplayTwo(mGrafixDir, mDeck, mCanvas, this);
    mDisplay->setAdvancePeriod(ADVANCE_PERDIOD);
    mEngine  = new EngineTwo(this, mDeck, (DisplayTwo*)mDisplay);
    connect(mEngine, SIGNAL(signalGameOver(int)), this, SLOT(gameOver(int)));
             
    mEngine->addPlayer(0, p1);
    mEngine->addPlayer(1, p2);
  }// end if

  // Create inputs and store in player
  AbstractInput* input1 = createInput(mLSkatConfig->inputType(0), mDisplay, mEngine);
  p1->setInput(input1);
  AbstractInput* input2 = createInput(mLSkatConfig->inputType(1), mDisplay, mEngine);
  p2->setInput(input2);

  // Start game
  startGame();
}

// Change the player names in a dialog
void Mainwindow::menuPlayerNames()
{
  NameDialogWidget dlg(this);
  for (int i=0;i<2;i++)
  {
    Player* p = mLSkatConfig->player(i);
    dlg.setName(i, p->name());
  }

  int result = dlg.exec();
  
  if (result == QDialog::Accepted)
  {
    for (int i=0;i<2;i++)
    {
      Player* p = mLSkatConfig->player(i);
      p->setName(dlg.name(i));
    }
  }
}


// Set the start player.
void Mainwindow::setStartPlayer(int no)
{
  mStartPlayer = no;
  ((KSelectAction *)ACTION("startplayer"))->setCurrentItem(mStartPlayer);
}

// Set the input type for a given player number.
void Mainwindow::setInputType(int no, InputDeviceType type)
{
  Player* p = 0;
  // Player 1
  if (no == 0)
  {
    ((KSelectAction *)ACTION("player1"))->setCurrentItem((int)type);
    p = mLSkatConfig->player(0);
  }
  else if (no == 1)
  {
    ((KSelectAction *)ACTION("player2"))->setCurrentItem((int)type);
    p = mLSkatConfig->player(1);
  }

  // Exchange player input at runtime
  if (mEngine && p && mDisplay && mEngine->isGameRunning())
  {
    AbstractInput* input = createInput(type, mDisplay, mEngine);
    p->setInput(input);
  }

}


#include "mainwindow.moc"
