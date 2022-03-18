cmake_minimum_required(VERSION 3.16)
project(inspire)
set(PROJECT_VERSION "5.24.3")
set(PROJECT_VERSION_MAJOR 5)

set(KF5_MIN_VERSION "5.86")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
set(QT_MIN_VERSION "5.15.0")

include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(FeatureSummary)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake)

include(CMakePackageConfigHelpers)
include(ECMInstallIcons)
include(ECMPackageConfigHelpers)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)

include(GtkUpdateIconCache)

option(WITH_DECORATIONS "Build Inspire window decorations for KWin" ON)
if(WITH_DECORATIONS)
    find_package(KDecoration2 REQUIRED)
    add_subdirectory(kdecoration)
endif()

add_subdirectory(colors)
add_subdirectory(cursors)
add_subdirectory(libinspirecommon)
add_subdirectory(kstyle)
add_subdirectory(misc)

option(WITH_WALLPAPERS "Install Inspire default wallpapers" ON)
if (WITH_WALLPAPERS)
    add_subdirectory(wallpapers)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/po AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/po)
    find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
    ki18n_install(po)
endif()

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX INSPIRE
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/InspireConfigVersion.cmake"
                  )

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Inspire")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/InspireConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/InspireConfig.cmake"
                                  PATH_VARS KDE_INSTALL_FULL_DATADIR
                                  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                                  )

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/InspireConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/InspireConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
        )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
