/***************************************************************************
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
 ***************************************************************************/

#ifndef LSKAT_H
#define LSKAT_H

#include <QHash>
 
#include <kmainwindow.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kdemacros.h>
#include "abstractinput.h"

// Forward declararion
class GameView;
class Deck;
class AbstractEngine;
class AbstractDisplay;
class Player;
class ConfigTwo;

using namespace InputDevice;

/**
 * The main application window.
 */
class Mainwindow : public KMainWindow
{

  Q_OBJECT

  public:
    /** Game mode type */
    enum GameMode    {Intro=0,LSkat=1,Skat=2};

    /** Construct the main window.
     *  @param The parent widget 
     */
    Mainwindow(QWidget *parent=0);

    /** Destructor 
     */
    ~Mainwindow();

  public slots:
    /** A new game is started from the menu.
     */
    void menuNewLSkatGame();

    /** A game is stopped in the menu.
     */
    void menuEndGame();

    /** The start player is changed in the menu.
     */
    void menuStartplayer();

    /* The input device for player 1 is changed in
     *  the menu.
     */
    void menuPlayer1By();

    /* The input device for player 2 is changed in
     *  the menu.
     */
    void menuPlayer2By();

    /** A card deck is chosen from the menu.
     */
    void menuCardDeck();

    /** The player names shall be changed.
     */
    void menuPlayerNames();

    /** The all time statistics is cleared from the menu.
     */
    void menuClearStatistics();

    /** A game over signal arrived.
     */
    void gameOver(int winner);

  protected:
    /** A new input device is created.
      * @param inputType The type of input
      * @param display   The display.
      * @param engine    The game engine.
      * @return The new input device.
      */
    AbstractInput* createInput(
                         InputDevice::InputDeviceType inputType, 
                         AbstractDisplay* display,
                         AbstractEngine* engine);

    /** Initialize all GUI elements.
     */
    void initGUI();

    /** Start a new game
     */
    void startGame();

    /** Set the start player.
     *  @param no The start player.
     */
    void setStartPlayer(int no);

    /** Saves the window properties 
     * @see KMainWindow#saveProperties
     */
    virtual void saveProperties(KConfig *cfg);

    /** Reads the session config file and restores the application's
     *  state.
     * @see KMainWindow#readProperties
     */
    virtual void readProperties(KConfig *cfg);

    /** Called by KMainWindow when the last window of the application is 
     * going to be closed.
     * @see KMainWindow#queryExit
     * @see KMainWindow#closeEvent
     */
    virtual bool queryExit();

  protected slots:
    /** Set the input type for a given player number.
     *  @param no The player number
     *  @param type The input type (Ai, Mouse, ...)
     */
    void setInputType(int no, InputDeviceType type);


  private:
    // Attributes not to be saved 
    /** The game engine */
    AbstractEngine* mEngine;
    /** The game display */
    AbstractDisplay* mDisplay;
    /** The main view */
    GameView* mView;
    /** The card deck */
    Deck* mDeck;
    /** The grafix directory */
    QString mGrafixDir;
    /** LSkat config */
    ConfigTwo* mLSkatConfig;
    /** Application config */
    KConfig* mConfig;

    
    // Properties to be saved 
    /** The cards directory */
    QString mCardDir;
    /** The card deck backside */
    QString mDeckGrafix;
    /** Current game mode (Intro, LSkat, ...) */
    GameMode mGameMode;
    /** Who starts the next game round */
    int mStartPlayer;
};
 
#endif 

