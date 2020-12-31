
set(lskat_SRCS
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
    fromlibkdegames/kcarddialog.cpp
)

ecm_setup_version(${LSKAT_VERSION}
    VARIABLE_PREFIX LSKAT
    VERSION_HEADER lskat_version.h
)

ki18n_wrap_ui(lskat_SRCS
    namedialog.ui
    fromlibkdegames/kgamecardselector.ui
)

configure_file(config-src.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

# Debug as default severity, at least until global_debug is removed
ecm_qt_declare_logging_category(lskat_SRCS
    HEADER lskat_debug.h
    IDENTIFIER LSKAT_LOG
    CATEGORY_NAME org.kde.lskat
    DEFAULT_SEVERITY Debug
    DESCRIPTION "LSkat game"
    EXPORT LSKAT
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../*-apps-lskat.png")
ecm_add_app_icon(lskat_SRCS ICONS ${ICONS_SRCS})
add_executable( lskat ${lskat_SRCS} )

target_link_libraries(lskat
    Qt5::Gui
    Qt5::Svg
    KF5::Crash
    KF5::GuiAddons
    KF5::XmlGui
    KF5KDEGames
)

install(TARGETS lskat  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES lskatui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/lskat)

ecm_qt_install_logging_categories(
    EXPORT LSKAT
    FILE lskat.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
