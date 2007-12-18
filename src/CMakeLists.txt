
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
    aiinput.cpp )


kde4_add_ui_files( lskat_SRCS namedialog.ui )

configure_file(config-src.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

set(lskat_icons
	${CMAKE_CURRENT_SOURCE_DIR}/../hi16-app-lskat.png
	${CMAKE_CURRENT_SOURCE_DIR}/../hi32-app-lskat.png
	${CMAKE_CURRENT_SOURCE_DIR}/../hi48-app-lskat.png
	${CMAKE_CURRENT_SOURCE_DIR}/../hi64-app-lskat.png
	${CMAKE_CURRENT_SOURCE_DIR}/../hi128-app-lskat.png
)
kde4_add_win32_app_icon(lskat_SRCS ${lskat_icons})
kde4_add_executable( lskat ${lskat_SRCS} )

target_link_libraries( lskat kdegames )

install(TARGETS lskat  DESTINATION  ${BIN_INSTALL_DIR} )

########### install files ###############

install( FILES lskatui.rc  DESTINATION  ${DATA_INSTALL_DIR}/lskat )

