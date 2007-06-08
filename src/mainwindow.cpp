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

#include "mainwindow.h"

// Include files for Qt

#include <QDir>
#include <QFile>

// Include files for KDE
#include <kstandardgameaction.h>
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
#include <kcarddialog.h>
#include <krandom.h>
#include <kglobal.h>
#include <ktoolbar.h>
#include <kselectaction.h>
#include <kapplication.h>

// Application specific includes
#include "lskatglobal.h"
#include "gameview.h"
#include "abstractengine.h"
#include "engine_two.h"
#include "display_two.h"
#include "config_two.h"
#include "display_intro.h"
#include "deck.h"
#include "player.h"
#include "mouseinput.h"
#include "aiinput.h"
#include "namedialogwidget.h"

// Configuration file
#include <config-src.h>

// Forward declarations
const int ADVANCE_PERDIOD =  20;

// Shortcut to access the actions
#define ACTION(x)   (actionCollection()->action(x))

// Name of the theme
const QString THEMENAME = "default.rc";

using namespace InputDevice;


// Construct the main application window
Mainwindow::Mainwindow(QWidget* parent)
          : KXmlGuiWindow(parent)
{
  // Reset stuff
  mDeck        = 0;
  mEngine      = 0;
  mDisplay     = 0;
  mView        = 0;
  mLSkatConfig = 0;
  mCanvas      = 0;
  mTheme       = 0;

  // Add resource type to grafix
  KGlobal::dirs()->addResourceType("lskattheme", "appdata", "grafix/");

  #ifdef SRC_DIR
  kDebug() << "Found SRC_DIR =" << SRC_DIR << endl;
  KGlobal::dirs()->addResourceDir("lskattheme",QString(SRC_DIR)+QString("/grafix/"));
  #endif

  // Theme file
  mThemeDirName = KGlobal::dirs()->findResourceDir("lskattheme",THEMENAME);
  kDebug() << "THEME DIR IS " << mThemeDirName << endl;

  // Check theme file
  QString theme = KStandardDirs::locate("lskattheme", THEMENAME);
  kDebug() << "theme file =" << theme << endl;
  if (theme.isEmpty())
  {
    KMessageBox::error(this, i18n("Installation error: No theme file found."));
    QTimer::singleShot(0, this,SLOT(close()));
    return;
  }


  // Create menus etc
  initGUI();

  // The LSkat config
  mLSkatConfig = new ConfigTwo(this);
  connect(mLSkatConfig, SIGNAL(signalInputType(int,InputDeviceType)),
          this, SLOT(setInputType(int,InputDeviceType)));
  mLSkatConfig->reset();

  // Read game properties and set default values (after config)
  readProperties();


  // Get the card deck
  long seed = KRandom::random();
  if (global_debug > 0) kDebug() << "Random seed " << seed << endl;
  mDeck = new Deck(seed, this);

  // Theme manager
  mTheme  = new ThemeManager(mCardDir, mDeckGrafix, KCardDialog::deckSVGFilePath(mDeckGrafix), 
                             THEMENAME, this, this->width());
  if (mTheme->checkTheme() != 0)
  {
    KMessageBox::error(this, i18n("Installation error: Theme file error."));
    QTimer::singleShot(0, this,SLOT(close()));
    return;
  }

  // Overall view
  mCanvas        = new QGraphicsScene(this);
  mView          = new GameView(QSize(880, 675), ADVANCE_PERDIOD, mCanvas, mTheme, this);

  // Create intro
  mGameMode      = Intro;
  mDisplay       = new DisplayIntro(mDeck, mCanvas, mTheme, ADVANCE_PERDIOD, mView);
  setCentralWidget(mView);

  // Create GUI
  setupGUI();
  toolBar()->hide();
  statusBar()->show();
  setAutoSaveSettings();

  statusBar()->showMessage(i18n("Welcome to Skat! Please start a new game."));

  // Skip intro?
  if (global_skip_intro)
  {
    menuNewLSkatGame();
  }
  // Start game automatically in demo mode
  else if (global_demo_mode)
  {
    // Start intro
    mDisplay->start();
    QTimer::singleShot(12500, this,SLOT(menuNewLSkatGame()));
  }
  else
  {
    // Start intro
    mDisplay->start();
  }
  if (global_debug > 0) kDebug() << "Mainwindow setup constructor done" << endl;

}

// Destructor
Mainwindow::~Mainwindow()
{
  saveProperties();
  if (mEngine) delete mEngine;
  if (mDisplay) delete mDisplay;
  if (mLSkatConfig) delete mLSkatConfig;
  if (mDeck) delete mDeck;
  if (mView) delete mView;
  if (mCanvas) delete mCanvas;
  if (mTheme) delete mTheme;
}


// Called by KMainWindow when the last window of the application is
bool Mainwindow::queryExit()
{
  if (mEngine)
  {
    mEngine->stopGame();
  }
  saveProperties();
  return true;
}


// Save properties
void Mainwindow::saveProperties()
{
  KConfig *config = KGlobal::config().data();

  // Program data
  KConfigGroup cfg = config->group("ProgramData");
  cfg.writePathEntry("carddir", mCardDir);
  cfg.writePathEntry("deck",    mDeckGrafix);
  cfg.writeEntry("startplayer", mStartPlayer);

  // LSkat data
  mLSkatConfig->save(config);
  config->sync();
}


// Load properties
void Mainwindow::readProperties()
{
  KConfig *config = KGlobal::config().data();

  // Program data
  KConfigGroup cfg = config->group("ProgramData");

  // Get default card data
  QString dcd = KCardDialog::getDefaultCardDir();
  dcd = KGlobal::dirs()->findResourceDir("cards", dcd)+dcd;
  QString dd = KCardDialog::getDefaultDeck();
  dd = KGlobal::dirs()->findResourceDir("cards", dd)+dd;


  // Read card path
  mCardDir    = cfg.readPathEntry("carddir", dcd);
  mDeckGrafix = cfg.readPathEntry("deck", dd);

  // Check for path existence
  QFile file(mDeckGrafix);
  if (!file.exists()) mDeckGrafix = dd;
  QDir dir(mCardDir);
  if (!dir.exists()) mCardDir = dcd;

  if (global_debug > 0)
  {
    kDebug() << "set mDeckGrafix=" << mDeckGrafix << endl;
    kDebug() << "set mCardDir=" << mCardDir << endl;
  }

  int no = cfg.readEntry("startplayer", 0);
  setStartPlayer(no);
  mLSkatConfig->load(config);
}


// Create a input with the given type
AbstractInput* Mainwindow::createInput(
                                 InputDeviceType inputType,
                                 AbstractDisplay* display,
                                 AbstractEngine* engine)
{
   AbstractInput* input = 0;

  // Always use AI input in demo mode
  if (global_demo_mode)
  {
    inputType = TypeAiInput;
  }

  // Create the player input
  if (inputType == TypeMouseInput)
  {
    MouseInput* mouseInput = new MouseInput(this);
    connect((QObject*)mView, SIGNAL(signalLeftMousePress(QPoint)),
            mouseInput, SLOT(mousePress(QPoint)));
    connect(mouseInput, SIGNAL(signalConvertMousePress(QPoint,int&,int&)),
            display, SLOT(convertMousePress(QPoint,int&,int&)));
    connect(mouseInput, SIGNAL(signalPlayerInput(int,int,int)),
            engine, SLOT(playerInput(int,int,int) ));
    input = mouseInput;
    if (global_debug > 0) kDebug() << "Create MOUSE INPUT " << endl;
  }
  else if (inputType == TypeAiInput)
  {
    AiInput* aiInput = new AiInput((EngineTwo*)engine, this);
    connect(aiInput, SIGNAL(signalPlayerInput(int,int,int)),
            engine, SLOT(playerInput(int,int,int) ));
    input = aiInput;
    if (global_debug > 0) kDebug() << "Create AI INPUT " << endl;
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
  // Enable game action
  QString endName = KStandardGameAction::name(KStandardGameAction::End);
  ACTION(endName)->setEnabled(true);

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

  //  statusBar()->clearMessage();
}


// Here a game over is signalled
void Mainwindow::gameOver(int /*winner*/)
{
  QString endName = KStandardGameAction::name(KStandardGameAction::End);
  ACTION(endName)->setEnabled(false);
  statusBar()->showMessage(i18n("Game Over. Please start a new game."));

  // Automatically restart game in demo mode
  if (global_demo_mode)
  {
    QTimer::singleShot(10000, this, SLOT(menuNewLSkatGame()));
  }
}


// Show next player
void Mainwindow::nextPlayer(Player* player)
{
  int no       = player->id()+1;
  QString name = player->name();
  statusBar()->showMessage(i18nc("Player name and number", "Next move for %1 (player %2)", name, no));
}


// Setup the GUI
void Mainwindow::initGUI()
{
  QAction *action;

  // Start a new game
  action = KStandardGameAction::gameNew(this, SLOT(menuNewLSkatGame()), actionCollection());
  if (global_demo_mode) action->setEnabled(false);

  // Clear all time statistics
  action = KStandardGameAction::clearStatistics(this, SLOT(menuClearStatistics()), actionCollection());
  action->setWhatsThis(i18n("Clears the all time statistics which is kept in all sessions."));
  if (global_demo_mode) action->setEnabled(false);

  // End a game
  action = KStandardGameAction::end(this, SLOT(menuEndGame()), actionCollection());
  action->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));
  if (global_demo_mode) action->setEnabled(false);
  else action->setEnabled(false);

  // Quit the program
  action = KStandardGameAction::quit(this, SLOT(close()), actionCollection());
  action->setWhatsThis(i18n("Quits the program."));

  // Determine start player
  KSelectAction* startPlayerAct = new KSelectAction(i18n("Starting Player"), this);
  actionCollection()->addAction("startplayer", startPlayerAct);
  connect(startPlayerAct, SIGNAL(triggered(int)), this, SLOT(menuStartplayer()));
  startPlayerAct->setToolTip(i18n("Changing starting player..."));
  startPlayerAct->setWhatsThis(i18n("Chooses which player begins the next game."));
  QStringList list;
  list.clear();
  list.append(i18n("Player &1"));
  list.append(i18n("Player &2"));
  startPlayerAct->setItems(list);
  if (global_demo_mode) startPlayerAct->setEnabled(false);


  // Determine who player player 1
  KSelectAction* player1Act = new KSelectAction(i18n("Player &1 Played By"), this);
  actionCollection()->addAction("player1", player1Act);
  connect(player1Act, SIGNAL(triggered(int)), this, SLOT(menuPlayer1By()));
  player1Act->setToolTip(i18n("Changing who plays player 1..."));
  player1Act->setWhatsThis(i18n("Changing who plays player 1."));
  list.clear();
  list.append(i18n("&Mouse"));
  list.append(i18n("&Computer"));
  player1Act->setItems(list);
  if (global_demo_mode) player1Act->setEnabled(false);

  // Determine who player player 2
  KSelectAction* player2Act = new KSelectAction(i18n("Player &2 Played By"), this);
  actionCollection()->addAction("player2", player2Act);
  connect(player2Act, SIGNAL(triggered(int)), this, SLOT(menuPlayer2By()));
  player2Act->setToolTip(i18n("Changing who plays player 2..."));
  player2Act->setWhatsThis(i18n("Changing who plays player 2."));
  player2Act->setItems(list);
  if (global_demo_mode) player2Act->setEnabled(false);

  // Choose card deck
  action = actionCollection()->addAction("select_carddeck");
  action->setText(i18n("Select &Card Deck..."));
  connect(action, SIGNAL(triggered(bool)), this, SLOT(menuCardDeck()));
  action->setToolTip(i18n("Configure card decks..."));
  action->setWhatsThis(i18n("Choose how the cards should look."));

  // Change player names
  action = actionCollection()->addAction("change_names");
  action->setText(i18n("&Change Player Names"));
  connect(action, SIGNAL(triggered(bool)), this, SLOT(menuPlayerNames()));
  if (global_demo_mode) action->setEnabled(false);
}


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

  KCardDialog::CardFlags flags = KCardDialog::CardFlags(KCardDialog::Both|KCardDialog::SVGCards);
  result=KCardDialog::getCardDeck(s1,s2, this, flags);
  if (result==QDialog::Accepted)
  {
    if (global_debug > 0) kDebug() << "NEW CARDDECK: " << s1 << " and " << s2 << endl;
    bool change = false; // Avoid unnecessary changes
    if (!s1.isEmpty() && s1 != mDeckGrafix)
    {
      mDeckGrafix = s1;
      change = true;
    }
    if (!s2.isEmpty() && s2 != mCardDir)
    {
      mCardDir    = s2;
      change = true;
    }
    if (change)
    {
      mTheme->updateCardTheme(mCardDir, mDeckGrafix, KCardDialog::deckSVGFilePath(mDeckGrafix));
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
                                                   QString(),
                                                   KStandardGuiItem::clear()))
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
  kDebug() << "Mainwindow::menuNewLSkatGame()"<<endl;

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

    mDisplay = new DisplayTwo(mDeck, mCanvas, mTheme, ADVANCE_PERDIOD, mView);
    mEngine = new EngineTwo(this, mDeck, (DisplayTwo*)mDisplay);
    connect(mEngine, SIGNAL(signalGameOver(int)), this, SLOT(gameOver(int)));
    connect(mEngine, SIGNAL(signalNextPlayer(Player*)), this, SLOT(nextPlayer(Player*)));

    // Connect player score widget updates
    connect(p1, SIGNAL(signalUpdate(Player*)), mDisplay, SLOT(updatePlayer(Player*)));
    connect(p2, SIGNAL(signalUpdate(Player*)), mDisplay, SLOT(updatePlayer(Player*)));

    mEngine->addPlayer(0, p1);
    mEngine->addPlayer(1, p2);
  }// end if

  // Create inputs and store in player
  AbstractInput* input1 = createInput(mLSkatConfig->inputType(0), mDisplay, mEngine);
  p1->setInput(input1);
  AbstractInput* input2 = createInput(mLSkatConfig->inputType(1), mDisplay, mEngine);
  p2->setInput(input2);

  statusBar()->showMessage(i18n("Dealing cards..."));

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
