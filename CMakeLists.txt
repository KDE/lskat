cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "90")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump LSKAT_BASE_VERSION once new features are added
set(LSKAT_BASE_VERSION "1.50")

set(LSKAT_VERSION "${LSKAT_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(lskat VERSION ${LSKAT_VERSION})

set (QT_MIN_VERSION "5.10.0")
set (KF5_MIN_VERSION "5.46.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

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
include(ECMSetupVersion)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

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
