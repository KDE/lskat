/*
    This file is part of the KDE games lskat program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mainwindow.h"

// Include files for Qt
#include <QAction>
#include <QDir>
#include <QKeySequence>
#include <QPointer>
#include <QRandomGenerator>
#include <QStatusBar>
#include <QStandardPaths>

// KF includes
#include <kwidgetsaddons_version.h>
#include <KActionCollection>
#include <KConfigGroup>
#include <KMessageBox>
#include <KSharedConfig>
#include <KStandardGameAction>
#include <KLocalizedString>
#include <KSelectAction>
// Application specific includes
#include "lskat_debug.h"
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
#include "fromlibkdegames/kcarddialog.h"
#include "fromlibkdegames/carddeckinfo.h"

// Configuration file
#include <config-src.h>

// Forward declarations
const int ADVANCE_PERIOD =  20;

// Shortcut to access the actions
#define ACTION(x)   (actionCollection()->action(x))

using namespace InputDevice;

// Construct the main application window
Mainwindow::Mainwindow(QWidget *parent)
          : KXmlGuiWindow(parent)
{
    // Reset stuff
    mDeck        = nullptr;
    mEngine      = nullptr;
    mDisplay     = nullptr;
    mView        = nullptr;
    mLSkatConfig = nullptr;
    mCanvas      = nullptr;
    mTheme       = nullptr;

    // Read theme files
    QStringList themeList;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("grafix"), QStandardPaths::LocateDirectory);
    for (const QString& dir : dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.desktop"));
        for (const QString& file : fileNames) {
            themeList.append(dir + '/' + file);
        }
    }
    if (themeList.isEmpty())
    {
        KMessageBox::error(this, i18n("Installation error: No theme list found."));
        QTimer::singleShot(0, this, &QWidget::close);
        return;
    }

    // Read theme files
    for (int i = 0; i < themeList.size(); i++)
    {
        KConfig themeInfo(themeList.at(i), KConfig::SimpleConfig);
        KConfigGroup themeGroup(&themeInfo, "Theme");
        QString name = themeGroup.readEntry("Name", QString());
        QString file = themeGroup.readEntry("File", QString());
        bool isDefault = themeGroup.readEntry("Default", false);
        mThemeFiles[name] = file;
        if (mThemeDefault.isNull()) mThemeDefault = name;
        if (isDefault) mThemeDefault = name;

        if (global_debug > 0) qCDebug(LSKAT_LOG) << "Found theme: " << themeList.at(i) << " Name(i18n)=" << name << " File=" << file << " default=" << isDefault;
    }
    mThemeIndexNo = themeIdxFromName(mThemeDefault);

    // Create menus etc
    initGUI();

    // The LSkat config
    mLSkatConfig = new ConfigTwo(this);
    connect(mLSkatConfig, &ConfigTwo::signalInputType, this, &Mainwindow::setInputType);
    mLSkatConfig->reset();

    // Read game properties and set default values (after config)
    readProperties();

    // TODO: Bugfix: Needs to be here if initGUI is before readProperties
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Setting current theme item to " << mThemeIndexNo;
    ((KSelectAction *)ACTION(QLatin1String("theme")))->setCurrentItem(mThemeIndexNo);

    // Get the card deck
    const quint32 seed = QRandomGenerator::global()->generate();
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Random seed" << seed;
    mDeck = new Deck(seed, this);

    // Theme manager
    QString themeFile = themefileFromIdx(mThemeIndexNo);
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Load theme" << themeFile << " no=" << mThemeIndexNo;
    mTheme  = new ThemeManager(mCardTheme, themeFile, this, this->width());
    if (mTheme->checkTheme() != 0)
    {
        KMessageBox::error(this, i18n("Installation error: Theme file error."));
        QTimer::singleShot(0, this, &QWidget::close);
        return;
    }

    // Overall view
    mCanvas        = new QGraphicsScene(this);
    mView          = new GameView(QSize(880, 675), ADVANCE_PERIOD, mCanvas, mTheme, this);

    // Create intro
    mGameMode      = Intro;
    mDisplay       = new DisplayIntro(mDeck, mCanvas, mTheme, ADVANCE_PERIOD, mView);
    setCentralWidget(mView);
    connect(mView, &GameView::signalLeftMousePress, this, &Mainwindow::menuNewLSkatGame);

    // Create GUI
    setupGUI();

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
        QTimer::singleShot(12500, this, &Mainwindow::menuNewLSkatGame);
    }
    else
    {
        // Start intro
        mDisplay->start();
    }
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Mainwindow setup constructor done";
}

// Destructor
Mainwindow::~Mainwindow()
{
    saveProperties();
    delete mEngine;
    delete mDisplay;
    delete mLSkatConfig;
    delete mDeck;
    delete mView;
    delete mCanvas;
    delete mTheme;
}

// Called by KMainWindow when the last window of the application is
void Mainwindow::closeEvent(QCloseEvent *event)
{
    if (mEngine)
    {
        mEngine->stopGame();
    }
    saveProperties();
    KXmlGuiWindow::closeEvent(event);
}

// Retrieve a theme file name from the menu index number
QString Mainwindow::themefileFromIdx(int idx)
{
    QStringList list(mThemeFiles.keys());
    list.sort();
    QString themeFile = mThemeFiles[list.at(idx)];
    return themeFile;
}

// Retrieve a theme idx from a theme name
int Mainwindow::themeIdxFromName(const QString &name)
{
    QStringList list(mThemeFiles.keys());
    list.sort();
    for (int i = 0; i < list.size(); ++i)
    {
        if (list[i] == name) return i;
    }
    qCCritical(LSKAT_LOG) << "Theme index lookup failed for " << name;
    return 0;
}

// Save properties
void Mainwindow::saveProperties()
{
    KConfig *config = KSharedConfig::openConfig().data();

    // Program data
    KConfigGroup cfg = config->group("ProgramData");
    cfg.writeEntry("startplayer", mStartPlayer);
    cfg.writeEntry("ThemeIndexNo", mThemeIndexNo);

    // LSkat data
    mLSkatConfig->save(config);
    config->sync();
}

// Load properties
void Mainwindow::readProperties()
{
    KConfig *config = KSharedConfig::openConfig().data();

    // Program data
    KConfigGroup cfg = config->group("ProgramData");

    // Theme number
    mThemeIndexNo = cfg.readEntry("ThemeIndexNo", themeIdxFromName(mThemeDefault));
    if (mThemeIndexNo >= mThemeFiles.size()) mThemeIndexNo = 0;

    // Read card path
    mCardTheme  = CardDeckInfo::deckName(cfg);

    int no = cfg.readEntry("startplayer", 0);
    setStartPlayer(no);
    mLSkatConfig->load(config);
}

// Create a input with the given type
AbstractInput *Mainwindow::createInput(
                                 InputDeviceType inputType,
                                 AbstractDisplay *display,
                                 AbstractEngine *engine)
{
    AbstractInput *input = nullptr;

    // Always use AI input in demo mode
    if (global_demo_mode)
    {
        inputType = TypeAiInput;
    }

    // Create the player input
    if (inputType == TypeMouseInput)
    {
        MouseInput *mouseInput = new MouseInput(this);
        connect(mView, &GameView::signalLeftMousePress,
                mouseInput, &MouseInput::mousePress);
        connect(mouseInput, &MouseInput::signalConvertMousePress,
                display, &AbstractDisplay::convertMousePress);
        connect(mouseInput, &MouseInput::signalPlayerInput,
                engine, &AbstractEngine::playerInput);
        input = mouseInput;
        if (global_debug > 0) qCDebug(LSKAT_LOG) << "Create MOUSE INPUT";
    }
    else if (inputType == TypeAiInput)
    {
        AiInput *aiInput = new AiInput((EngineTwo *)engine, this);
        connect(aiInput, &AiInput::signalPlayerInput,
                engine, &AbstractEngine::playerInput);
        input = aiInput;
        if (global_debug > 0) qCDebug(LSKAT_LOG) << "Create AI INPUT";
    }
    else
    {
        qCCritical(LSKAT_LOG) << "Unsupported input device type" << inputType;
    }

    return input;
}

// Start a new game
void Mainwindow::startGame()
{
    // Enable game action
    QLatin1String endName(KStandardGameAction::name(KStandardGameAction::End));
    ACTION(endName)->setEnabled(true);

    // Deal cards to player - Shuffle card deck and reset pile
    mDeck->shuffle();

    // Draw Trump
    Suite trump = mDeck->randomTrump();

    // Loop all players in the game
    QHashIterator<int,Player *> it = mLSkatConfig->playerIterator();
    while (it.hasNext())
    {
        it.next();
        Player *player = it.value();
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
    QLatin1String endName(KStandardGameAction::name(KStandardGameAction::End));
    ACTION(endName)->setEnabled(false);
    statusBar()->showMessage(i18n("Game Over. Please start a new game."));

    // Automatically restart game in demo mode
    if (global_demo_mode)
    {
        QTimer::singleShot(10000, this, &Mainwindow::menuNewLSkatGame);
    }
}

// Show next player
void Mainwindow::nextPlayer(Player *player)
{
    int no       = player->id() + 1;
    QString name = player->name();
    statusBar()->showMessage(i18nc("Player name and number", "Next move for %1 (player %2)", name, no));
}

// Setup the GUI
void Mainwindow::initGUI()
{
    QAction *action;

    // Start a new game
    action = KStandardGameAction::gameNew(this, &Mainwindow::menuNewLSkatGame, actionCollection());
    if (global_demo_mode) action->setEnabled(false);

    // Clear all time statistics
    action = KStandardGameAction::clearStatistics(this, &Mainwindow::menuClearStatistics, actionCollection());
    action->setWhatsThis(i18n("Clears the all time statistics which is kept in all sessions."));
    if (global_demo_mode) action->setEnabled(false);

    // End a game
    action = KStandardGameAction::end(this, &Mainwindow::menuEndGame, actionCollection());
    action->setWhatsThis(i18n("Ends a currently played game. No winner will be declared."));
    action->setEnabled(false);

    // Quit the program
    action = KStandardGameAction::quit(this, &QWidget::close, actionCollection());
    action->setWhatsThis(i18n("Quits the program."));

    // Determine start player
    KSelectAction *startPlayerAct = new KSelectAction(i18n("Starting Player"), this);
    actionCollection()->addAction(QStringLiteral("startplayer"), startPlayerAct);
    connect(startPlayerAct, &KSelectAction::indexTriggered, this, &Mainwindow::menuStartplayer);
    startPlayerAct->setToolTip(i18n("Changing starting player..."));
    startPlayerAct->setWhatsThis(i18n("Chooses which player begins the next game."));
    QStringList list;
    list.clear();
    list.append(i18n("Player &1"));
    list.append(i18n("Player &2"));
    startPlayerAct->setItems(list);
    if (global_demo_mode) startPlayerAct->setEnabled(false);

    // Determine who plays player 1
    KSelectAction *player1Act = new KSelectAction(i18n("Player &1 Played By"), this);
    actionCollection()->addAction(QStringLiteral("player1"), player1Act);
    connect(player1Act, &KSelectAction::indexTriggered, this, &Mainwindow::menuPlayer1By);
    player1Act->setToolTip(i18n("Changing who plays player 1..."));
    player1Act->setWhatsThis(i18n("Changing who plays player 1."));
    list.clear();
    list.append(i18n("&Mouse"));
    list.append(i18n("&Computer"));
    player1Act->setItems(list);
    if (global_demo_mode) player1Act->setEnabled(false);

    // Determine who plays player 2
    KSelectAction *player2Act = new KSelectAction(i18n("Player &2 Played By"), this);
    actionCollection()->addAction(QStringLiteral("player2"), player2Act);
    connect(player2Act, &KSelectAction::indexTriggered, this, &Mainwindow::menuPlayer2By);
    player2Act->setToolTip(i18n("Changing who plays player 2..."));
    player2Act->setWhatsThis(i18n("Changing who plays player 2."));
    player2Act->setItems(list);
    if (global_demo_mode) player2Act->setEnabled(false);

    // Add all theme files to the menu
    QStringList themes(mThemeFiles.keys());
    themes.sort();

    KSelectAction *themeAct = new KSelectAction(i18n("&Theme"), this);
    actionCollection()->addAction(QStringLiteral("theme"), themeAct);
    themeAct->setItems(themes);
    connect(themeAct, &KSelectAction::indexTriggered, this, &Mainwindow::changeTheme);
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Setting current theme item to " << mThemeIndexNo;
    themeAct->setCurrentItem(mThemeIndexNo);
    themeAct->setToolTip(i18n("Changing theme..."));
    themeAct->setWhatsThis(i18n("Changing theme."));

    // Choose card deck
    QAction *action1 = actionCollection()->addAction(QStringLiteral("select_carddeck"));
    action1->setText(i18n("Select &Card Deck..."));
    actionCollection()->setDefaultShortcut(action1, QKeySequence(Qt::Key_F10));
    connect(action1, &QAction::triggered, this, &Mainwindow::menuCardDeck);
    action1->setToolTip(i18n("Configure card decks..."));
    action1->setWhatsThis(i18n("Choose how the cards should look."));

    // Change player names
    action = actionCollection()->addAction(QStringLiteral("change_names"));
    action->setText(i18n("&Change Player Names..."));
    connect(action, &QAction::triggered, this, &Mainwindow::menuPlayerNames);
    if (global_demo_mode) action->setEnabled(false);
}

// Choose start player
void Mainwindow::menuStartplayer()
{
    int i = ((KSelectAction *)ACTION(QLatin1String("startplayer")))->currentItem();
    setStartPlayer(i);
}

// Change the theme of the game
void Mainwindow::changeTheme(int idx)
{
    mThemeIndexNo = idx;
    QString themeFile = themefileFromIdx(idx);
    if (global_debug > 0) qCDebug(LSKAT_LOG) << "Select theme " << themeFile;
    mTheme->updateTheme(themeFile);
}

// Select input for player 1
void Mainwindow::menuPlayer1By()
{
    int i = ((KSelectAction *)ACTION(QLatin1String("player1")))->currentItem();
    mLSkatConfig->setInputType(0, (InputDeviceType)i);
}

// Select input for player 2
void Mainwindow::menuPlayer2By()
{
    int i = ((KSelectAction *)ACTION(QLatin1String("player2")))->currentItem();
    mLSkatConfig->setInputType(1, (InputDeviceType)i);
}

// Choose a card deck
void Mainwindow::menuCardDeck()
{
    QString front = mCardTheme;

    KConfigGroup grp = KSharedConfig::openConfig()->group("ProgramData");
    KCardWidget *cardwidget = new KCardWidget();
    QPointer<KCardDialog> dlg;

    cardwidget->readSettings(grp);
    dlg = new KCardDialog(cardwidget);
    if (dlg->exec() == QDialog::Accepted)
    {
        // Always store the settings, other things than the deck may have changed
        cardwidget->saveSettings(grp);
        grp.sync();
        if (global_debug > 0) qCDebug(LSKAT_LOG) << "NEW CARDDECK: " << front;
        bool change = false; // Avoid unnecessary changes
        if (!cardwidget->deckName().isEmpty() && cardwidget->deckName() != mCardTheme)
        {
            mCardTheme    = cardwidget->deckName();
            change = true;
        }
        if (change)
        {
            mTheme->updateCardTheme(mCardTheme);
            mView->update(); // Be on the safe side and update
        }
    }
    delete dlg;
}

// Clear all time statistics
void Mainwindow::menuClearStatistics()
{
    QString message;
    message = i18n("Do you really want to clear the all time "
                    "statistical data?");

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    if (KMessageBox::PrimaryAction == KMessageBox::questionTwoActions(this,
#else
    if (KMessageBox::Yes == KMessageBox::questionYesNo(this,
#endif
                                                       message,
                                                       QString(),
                                                       KStandardGuiItem::clear(),
                                                       KStandardGuiItem::cancel()))
    {
        QHashIterator<int,Player *> it = mLSkatConfig->playerIterator();
        while (it.hasNext())
        {
            it.next();
            Player *player = it.value();
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
    disconnect(mView, &GameView::signalLeftMousePress, this, &Mainwindow::menuNewLSkatGame);

    Player *p1 = mLSkatConfig->player(0);
    Player *p2 = mLSkatConfig->player(1);

    // Stop running games
    if (mEngine)
    {
        mEngine->stopGame();
    }

    // Get rid of old stuff?
    if (true || (mGameMode != LSkat)) // Yes! Fixes bugs 330308 and 228067.
    // Jenkins objected to the indentation if the above was simply commented out.
    {
        // Set new game mode
        mGameMode = LSkat;

        // Start deleting
        delete mDisplay;
        delete mEngine;

        auto *display = new DisplayTwo(mDeck, mCanvas, mTheme, ADVANCE_PERIOD, mView);
        mDisplay = display;
        mEngine = new EngineTwo(this, mDeck, (DisplayTwo *)mDisplay);
        connect(mEngine, &AbstractEngine::signalGameOver, this, &Mainwindow::gameOver);
        connect(mEngine, &AbstractEngine::signalNextPlayer, this, &Mainwindow::nextPlayer);

        // Connect player score widget updates
        connect(p1, &Player::signalUpdate, display, &DisplayTwo::updatePlayer);
        connect(p2, &Player::signalUpdate, display, &DisplayTwo::updatePlayer);

        mEngine->addPlayer(0, p1);
        mEngine->addPlayer(1, p2);
    }// end if

    // Create inputs and store in player
    AbstractInput *input1 = createInput(mLSkatConfig->inputType(0), mDisplay, mEngine);
    p1->setInput(input1);
    AbstractInput *input2 = createInput(mLSkatConfig->inputType(1), mDisplay, mEngine);
    p2->setInput(input2);

    statusBar()->showMessage(i18n("Dealing cards..."));

    // Start game
    startGame();
}

// Change the player names in a dialog
void Mainwindow::menuPlayerNames()
{
    QPointer<NameDialogWidget> dlg = new NameDialogWidget(this);
    for (int i = 0; i < 2; i++)
    {
        Player *p = mLSkatConfig->player(i);
        dlg->setName(i, p->name());
    }

    if (dlg->exec() == QDialog::Accepted)
    {
        for (int i = 0; i < 2; i++)
        {
            Player *p = mLSkatConfig->player(i);
            p->setName(dlg->name(i));
        }
    }

    delete dlg;
}

// Set the start player.
void Mainwindow::setStartPlayer(int no)
{
    mStartPlayer = no;
    ((KSelectAction *)ACTION(QLatin1String("startplayer")))->setCurrentItem(mStartPlayer);
}

// Set the input type for a given player number.
void Mainwindow::setInputType(int no, InputDeviceType type)
{
    Player *p = nullptr;
    // Player 1
    if (no == 0)
    {
        ((KSelectAction *)ACTION(QLatin1String("player1")))->setCurrentItem((int)type);
        p = mLSkatConfig->player(0);
    }
    else if (no == 1)
    {
        ((KSelectAction *)ACTION(QLatin1String("player2")))->setCurrentItem((int)type);
        p = mLSkatConfig->player(1);
    }

    // Exchange player input at runtime
    if (mEngine && p && mDisplay && mEngine->isGameRunning())
    {
        AbstractInput *input = createInput(type, mDisplay, mEngine);
        p->setInput(input);
    }
}
