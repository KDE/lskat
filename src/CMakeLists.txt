
include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/kgame )

set( lskat_SRCS 
    playerstatuswidget.cpp
    namedialogwidget.cpp
    gameview.cpp
    canvasview.cpp
    mainwindow.cpp
    main.cpp
    deck.cpp
    config_two.cpp
    abstractdisplay.cpp
    display_intro.cpp
    display_two.cpp
    cardsprite.cpp
    pixmapsprite.cpp
    rectanglesprite.cpp
    textsprite.cpp
    abstractengine.cpp
    engine_two.cpp
    player.cpp
    abstractinput.cpp
    mouseinput.cpp
    aiinput.cpp )


kde4_add_ui_files( lskat_SRCS playerstatusform.ui namedialog.ui )

kde4_automoc( ${lskat_SRCS} )

kde4_add_executable( lskat ${lskat_SRCS} )

target_link_libraries( lskat kdegames )

install(TARGETS lskat  DESTINATION  ${BIN_INSTALL_DIR} )

########### install files ###############

install( FILES lskatui.rc  DESTINATION  ${DATA_INSTALL_DIR}/lskat )

