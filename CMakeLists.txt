project(lskat)

cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
set (QT_MIN_VERSION "5.9.0")
set (KF5_MIN_VERSION "5.46.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Gui Svg Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    DocTools
    GuiAddons
    I18n
    WidgetsAddons
    XmlGui
)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions( -DQT_DISABLE_DEPRECATED_BEFORE=0x060000 )
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
if (${KF5Config_VERSION} STRGREATER "5.56.0")
        add_definitions(-DQT_NO_FOREACH)
        MESSAGE(STATUS "compile without foreach")
endif()

add_subdirectory(src)
add_subdirectory(grafix)
add_subdirectory(doc)

########### install files ###############
install(PROGRAMS org.kde.lskat.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.lskat.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
if (${ECM_VERSION} STRGREATER "5.58.0")
    install(FILES lskat.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
else()
    install(FILES lskat.categories  DESTINATION ${KDE_INSTALL_CONFDIR})
endif()

ecm_install_icons(ICONS
    128-apps-lskat.png
    16-apps-lskat.png
    22-apps-lskat.png
    32-apps-lskat.png
    48-apps-lskat.png
    64-apps-lskat.png
    DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
