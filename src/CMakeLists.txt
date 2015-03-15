
include_directories( ${CMAKE_SOURCE_DIR}/libKF5KDEGames/kgame )

set( lskat_SRCS
    lskat_debug.cpp 
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


ki18n_wrap_ui( lskat_SRCS
    namedialog.ui
    fromlibkdegames/kgamecardselector.ui )

configure_file(config-src.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../*-apps-lskat.png")
ecm_add_app_icon(lskat_SRCS ICONS ${ICONS_SRCS})
add_executable( lskat ${lskat_SRCS} )

target_link_libraries( lskat KF5KDEGames KF5::KDELibs4Support  Qt5::Gui Qt5::Svg )

install(TARGETS lskat  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES lskatui.rc  DESTINATION  ${KDE_INSTALL_DATADIR}/lskat )


