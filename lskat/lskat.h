/***************************************************************************
                          lskat.h  -  description
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

#ifndef LSKAT_H
#define LSKAT_H
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
#include <q3strlist.h>

// include files for KDE 
#include <kapplication.h>
#include <kmainwindow.h>
#include <kaccel.h>

#include "KEInput.h"
#include "KEMessage.h"
#include "KConnectTypes.h"

typedef enum {Club=0,Spade=1,Heart=2,Diamond=3,Grand=4} CCOLOUR;
typedef enum {Ace=0,King=1,Queen=2,Jack=3,Ten=4,Nine=5,Eight=6,Seven=7} CCARD;
#define NO_OF_CARDS 32
#define NO_OF_TILES 16
#define NO_OF_TRUMPS 5
//#define NO_OF_DECKS 6
#define NO_OF_ANIM  24

// Window update codes
#define UPDATE_STATUS  1

extern int global_debug;
// forward declaration of the LSkat classes
class LSkatDoc;
class LSkatView;

/**
  * The base class for LSkat application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. An instance of LSkatView creates your center view, which is connected
  * to the window's Doc object.
  * LSkatApp reimplements the methods that KMainWindow provides for main window handling and supports
  * full session management as well as keyboard accelerator configuration by using KAccel.
  * @see KMainWindow
  * @see KApplication
  * @see KConfig
  * @see KAccel
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
  * @version KDevelop version 0.4 code generation
  */
class LSkatApp : public KMainWindow
{
  Q_OBJECT

  friend class LSkatView;

  public:
    /** construtor of LSkatApp, calls all init functions to create the application.
     */
    LSkatApp();
    ~LSkatApp();
    /** enables menuentries/toolbar items
     */
    void enableAction(const char *);
    /** disables menuentries/toolbar items
     */
    void disableAction(const char *);
    /** add a opened file to the recent file list and update recent_file_menu
     */
    /** returns a pointer to the current document connected to the KMainWindow instance and is used by
     * the View class to access the document object's methods
     */	
    LSkatDoc *getDocument() const; 	
    void MoveFinished();
    void Move(int x,int y,int player,bool remote);
    void SetGrafix(QString s);

  protected:
    void NewGame();
    bool MakeInputDevice(int no);
    void OptionsNetwork();
    // Puts all game data into a message
    void PrepareGame(KEMessage *msg);
    void ExtractGame(KEMessage *msg);

    /** initGUI creates the menubar and inserts the menupopups as well as creating the helpMenu.
     * @see KApplication#getHelpMenu
     */
    void initGUI();
    /** Checks all menus..usually done on init programm */
    void checkMenus(int menu=0);

    /** this creates the toolbars.
     */
    /** sets up the statusbar for the main window by initialzing a statuslabel.
     */
    void initStatusBar();
    /** initializes the document object of the main window that is connected to the view in initView().
     * @see initView();
     */
    void initDocument();
    /** creates the centerwidget of the KMainWindow instance and sets it as the view
     */
    void initView();
    /** queryClose is called by KMainWindow on each closeEvent of a window. Against the
     * default implementation (only returns true), this calles saveModified() on the document object to ask if the document shall
     * be saved if Modified; on cancel the closeEvent is rejected.
     * @see KMainWindow#queryClose
     * @see KMainWindow#closeEvent
     */
    virtual bool queryClose();
    /** queryExit is called by KMainWindow when the last window of the application is going to be closed during the closeEvent().
     * Against the default implementation that just returns true, this calls saveOptions() to save the settings of the last window's	
     * properties.
     * @see KMainWindow#queryExit
     * @see KMainWindow#closeEvent
     */
    virtual bool queryExit();
    /** saves the window properties for each open window during session end to the session config file, including saving the currently
     * opened file by a temporary filename provided by KApplication.
     * @see KMainWindow#saveProperties
     */
    virtual void saveProperties(KConfig *_cfg);
    /** reads the session config file and restores the application's state including the last opened files and documents by reading the
     * temporary files saved by saveProperties()
     * @see KMainWindow#readProperties
     */
    virtual void readProperties(KConfig *_cfg);

  public slots:
    void slotPrepareProcessMove(KEMessage *msg);
    void slotPrepareRemoteMove(KEMessage *msg);
    void slotPrepareInteractiveMove(KEMessage *msg);
    void slotReceiveInput(KEMessage *msg,int id);

    /** clears the document in the actual view to reuse it as the new document */
    void slotFileNew();
    /** asks for saving if the file is modified, then closes the actual file and window*/
    void slotFileEnd();
    /** closes all open windows by calling close() on each memberList item until the list is empty, then quits the application.
     * If queryClose() returns false because the user canceled the saveModified() dialog, the closing breaks.
     */
    void slotFileQuit();
    // clear all time stats
    void slotFileStatistics();
    /** Msg to remote player */
    void slotFileMessage();
    /** changes the statusbar contents for the standard label permanently, used to indicate current actions.
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusMsg(const QString &text);
    void slotClearStatusMsg();
    /** changes the status message of the whole statusbar for two seconds, then restores the last status. This is used to display
     * statusbar messages that give information about actions for toolbar icons and menuentries.
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusHelpMsg(const QString &text);
    /** Set the names in the mover field */
    void slotStatusNames();
    void slotStatusMover(const QString &text);

    void slotLevel();
    void slotStartplayer();
    void slotPlayer1(KG_INPUTTYPE i);
    void slotPlayer2(KG_INPUTTYPE i);
    void slotPlayer1By();
    void slotPlayer2By();
    void slotOptionsNames();
    void slotOptionsCardDeck();
  /** Triggers the process timer */
  void slotProcTimer(void);

protected: // Protected attributes
  
  enum CheckFlags {All=0,CheckFileMenu=1,CheckOptionsMenu=2,CheckViewMenu=4};
  
  /**  */
  /** Counts the time in the status bar */
  QTimer * procTimer;
  KEInput *mInput;
  QString mGrafix;

  private:
    /** contains the recently used filenames */
    Q3StrList recentFiles;

    /** the configuration object of the application */
    KConfig *config;

    LSkatView *view;
    /** doc represents your actual document and is created only once. It keeps
     * information such as filename and does the serialization of your files.
     */
    LSkatDoc *doc;
};
 
#endif // LSKAT_H

