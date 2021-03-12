cmake_minimum_required(VERSION 3.0)
project(ferenkwin)
set(PROJECT_VERSION "5.21.2")
set(PROJECT_VERSION_MAJOR 5)

set(KF5_MIN_VERSION "5.78")

include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(FeatureSummary)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake)

include(ECMPackageConfigHelpers)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)

include(GtkUpdateIconCache)

find_package(KDecoration2 REQUIRED)
add_subdirectory(kdecoration)

add_subdirectory(libbreezecommon)

if(EXISTS ${CMAKE_SOURCE_DIR}/po AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/po)
    find_package(KF5I18n CONFIG REQUIRED)
    ki18n_install(po)
endif()

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX BREEZE
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/FerenKWinConfigVersion.cmake"
                  )

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/FerenKWin")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/FerenKWinConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/FerenKWinConfig.cmake"
                                  PATH_VARS KDE_INSTALL_FULL_DATADIR
                                  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                                  )

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/FerenKWinConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/FerenKWinConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
        )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
