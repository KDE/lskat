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

                          Lskat
                          -----
    begin                : March 2000
    copyright            : (C) 1995-2006 by Martin Heni
    email                : kde@heni-online.de
*/

#ifndef LSKAT_MAINWINDOW_H
#define LSKAT_MAINWINDOW_H

// Qt includes
#include <QHash>

// KDE includes
#include <KXmlGuiWindow>

// Local includes
#include "abstractinput.h"
#include "thememanager.h"

// Forward declaration
class GameView;
class Deck;
class AbstractEngine;
class AbstractDisplay;
class Player;
class ConfigTwo;
class QGraphicsScene;

using namespace InputDevice;

/**
 * The main application window.
 */
class Mainwindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    /**
     * Game mode type
     */
    enum GameMode {Intro = 0, LSkat = 1, Skat = 2};

    /**
     * Construct the main window.
     * @param parent The parent widget
     */
    explicit Mainwindow(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~Mainwindow();

public slots:
    /**
     * A new game is started from the menu.
     */
    void menuNewLSkatGame();

    /**
     * A game is stopped in the menu.
     */
    void menuEndGame();

    /**
     * The start player is changed in the menu.
     */
    void menuStartplayer();

    /**
     * The input device for player 1 is changed in the menu.
     */
    void menuPlayer1By();

    /**
     * The input device for player 2 is changed in the menu.
     */
    void menuPlayer2By();

    /**
     * A card deck is chosen from the menu.
     */
    void menuCardDeck();

    /**
     * The player names shall be changed.
     */
    void menuPlayerNames();

    /**
     * The all time statistics is cleared from the menu.
     */
    void menuClearStatistics();

    /**
     * A game over signal arrived.
     */
    void gameOver(int winner);

    /**
     * Next players turn.
     */
    void nextPlayer(Player *player);

    /**
     * A new theme is selected from the menu.
     * @param idx The theme index
     */
    void changeTheme(int idx);

protected:
    /**
     * A new input device is created.
     * @param inputType The type of input
     * @param display   The display.
     * @param engine    The game engine.
     * @return The new input device.
     */
    AbstractInput *createInput(
                         InputDevice::InputDeviceType inputType,
                         AbstractDisplay *display,
                         AbstractEngine *engine);

    /**
     * Initialize all GUI elements.
     */
    void initGUI();

    /**
     * Start a new game
     */
    void startGame();

    /**
     * Set-up the start player.
     * @param no The start player.
     */
    void setStartPlayer(int no);

    /**
     * Saves the window properties
     * @see KMainWindow#saveProperties
     */
    virtual void saveProperties();

    /**
     * Reads the session config file and restores the application's state.
     * @see KMainWindow#readProperties
     */
    virtual void readProperties();

    /**
     * Called by KMainWindow when closing the window.
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * Retrieve the theme file from the theme index number give.
     * @param idx The theme index number [0..]
     * @return The theme file name.
     */
    QString themefileFromIdx(int idx);

    /**
     * Retrieve the theme idx number from the theme name give.
     * @param name The theme file name.
     * @return The theme index number [0..]
     */
    int themeIdxFromName(const QString &name);

protected slots:
    /**
     * Set the input type for a given player number.
     * @param no The player number
     * @param type The input type (Ai, Mouse, ...)
     */
    void setInputType(int no, InputDeviceType type);

private:
    // Attributes not to be saved
    // The game engine
    AbstractEngine *mEngine;
    // The game display
    AbstractDisplay *mDisplay;
    // The main view
    GameView *mView;
    // The card deck
    Deck *mDeck;
    // LSkat config
    ConfigTwo *mLSkatConfig;
    // The graphics scene
    QGraphicsScene *mCanvas;
    // The theme manager used
    ThemeManager *mTheme;
    // The available themes
    QHash<QString, QString> mThemeFiles;
    // Current theme index
    int mThemeIndexNo;
    // Default theme
    QString mThemeDefault;

    // Properties to be saved
    // The cards directory
    QString mCardTheme;
    // Current game mode (Intro, LSkat, ...)
    GameMode mGameMode;
    // Who starts the next game round/
    int mStartPlayer;
};

#endif // LSKAT_MAINWINDOW_H
