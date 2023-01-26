ecm_setup_version(${LSKAT_VERSION}
    VARIABLE_PREFIX LSKAT
    VERSION_HEADER lskat_version.h
)

configure_file(config-src.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

add_executable(lskat)

target_sources(lskat PRIVATE
    abstractdisplay.cpp
    abstractdisplay.h
    abstractengine.cpp
    abstractengine.h
    abstractinput.cpp
    abstractinput.h
    aiinput.cpp
    aiinput.h
    cardsprite.cpp
    cardsprite.h
    config_two.cpp
    config_two.h
    deck.cpp
    deck.h
    display_intro.cpp
    display_intro.h
    display_two.cpp
    display_two.h
    engine_two.cpp
    engine_two.h
    fromlibkdegames/cardcache.cpp
    fromlibkdegames/cardcache.h
    fromlibkdegames/cardcache_p.h
    fromlibkdegames/carddeckinfo.cpp
    fromlibkdegames/carddeckinfo.h
    fromlibkdegames/carddeckinfo_p.h
    fromlibkdegames/kcarddialog.cpp
    fromlibkdegames/kcarddialog.h
    gameview.cpp
    gameview.h
    lskatglobal.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mouseinput.cpp
    mouseinput.h
    namedialogwidget.cpp
    namedialogwidget.h
    pixmapsprite.cpp
    pixmapsprite.h
    player.cpp
    player.h
    scoresprite.cpp
    scoresprite.h
    textsprite.cpp
    textsprite.h
    thememanager.cpp
    thememanager.h

    lskat.qrc
)

ki18n_wrap_ui(lskat
    namedialog.ui
    fromlibkdegames/kgamecardselector.ui
)

# Debug as default severity, at least until global_debug is removed
ecm_qt_declare_logging_category(lskat
    HEADER lskat_debug.h
    IDENTIFIER LSKAT_LOG
    CATEGORY_NAME org.kde.lskat
    DEFAULT_SEVERITY Debug
    DESCRIPTION "LSkat game"
    EXPORT LSKAT
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../*-apps-lskat.png")
ecm_add_app_icon(lskat ICONS ${ICONS_SRCS})

target_link_libraries(lskat
    KF5KDEGames
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::GuiAddons
    KF${KF_MAJOR_VERSION}::XmlGui
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Svg
)

install(TARGETS lskat  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_qt_install_logging_categories(
    EXPORT LSKAT
    FILE lskat.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
