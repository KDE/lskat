cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "25")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump LSKAT_BASE_VERSION once new features are added
set(LSKAT_BASE_VERSION "1.50")

set(LSKAT_VERSION "${LSKAT_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(lskat VERSION ${LSKAT_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(FeatureSummary)
include(ECMDeprecationSettings)

include(InternalMacros)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Gui
    Svg
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    DBusAddons
    DocTools
    GuiAddons
    I18n
    WidgetsAddons
    XmlGui
    IconThemes
)

find_package(KDEGames6 6.0.0 REQUIRED)

ecm_set_disabled_deprecation_versions(
    QT 6.9.0
    KF 6.16.0
    KDEGAMES 6.0
)

add_subdirectory(src)
add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)

########### install files ###############
install(PROGRAMS org.kde.lskat.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.lskat.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
