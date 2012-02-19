
include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/kgame )

set( lskat_SRCS 
    thememanager.cpp
    namedialogwidget.cpp
    gameview.cpp
    mainwindow.cpp
    main.cpp
    deck.cpp
    config_two.cpp
    abstractdisplay.cpp
    display_intro.cpp
    display_two.cpp
    cardsprite.cpp
    pixmapsprite.cpp
    scoresprite.cpp
    textsprite.cpp
    abstractengine.cpp
    engine_two.cpp
    player.cpp
    abstractinput.cpp
    mouseinput.cpp
    aiinput.cpp
    fromlibkdegames/cardcache.cpp
    fromlibkdegames/carddeckinfo.cpp
    fromlibkdegames/kcarddialog.cpp )


kde4_add_ui_files( lskat_SRCS
    namedialog.ui
    fromlibkdegames/kgamecardselector.ui )

configure_file(config-src.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

kde4_add_app_icon(lskat_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../hi*-app-lskat.png")
kde4_add_executable( lskat ${lskat_SRCS} )

target_link_libraries( lskat kdegames ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTSVG_LIBRARY} )

install(TARGETS lskat  DESTINATION  ${BIN_INSTALL_DIR} )

########### install files ###############

install( FILES lskatui.rc  DESTINATION  ${DATA_INSTALL_DIR}/lskat )


