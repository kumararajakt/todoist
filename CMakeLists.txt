cmake_minimum_required(VERSION 3.16)

project(todoist VERSION 0.1)

set(QT_QML_GENERATE_QMLLS_INI ON)

set(CMAKE_CXX_STANDARD 20)
set(QT6_MIN_VERSION 6.5.0)
set(KF6_MIN_VERSION 6.12.0)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
else()
    message(STATUS "ccache not found")
endif()

include(FeatureSummary)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMFindQmlModule)
include(KDECompilerSettings)
include(GenerateExportHeader)
include(ECMInstallIcons)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMSetupVersion)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX TODOIST
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/version-todoist.h"
)

set(QT_EXTRA_COMPONENTS)
list(APPEND QT_EXTRA_COMPONENTS Widgets)

find_package(Qt6 ${QT6_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Widgets Quick Svg Sql ${QT_EXTRA_COMPONENTS})
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Kirigami CoreAddons Config I18n)

qt_policy(SET QTP0001 NEW)


include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)


add_subdirectory(src)

install(FILES org.kde.todoist.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES todoist.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/256x256/apps RENAME org.kde.todoist.png)
ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
