add_definitions(-DTRANSLATION_DOMAIN="inspire_kwin_deco")

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons GuiAddons ConfigWidgets WindowSystem I18n IconThemes)
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS DBus)

### XCB
find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
  DESCRIPTION "X protocol C-language Binding"
  URL "https://xcb.freedesktop.org"
  TYPE OPTIONAL
  PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

if(UNIX AND NOT APPLE)

  set(INSPIRE_HAVE_X11 ${XCB_XCB_FOUND})
  if (XCB_XCB_FOUND)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS X11Extras)
  endif()

else()

  set(INSPIRE_HAVE_X11 FALSE)

endif()

################# configuration #################
configure_file(config-inspire.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-inspire.h )

################# includes #################
include_directories(${CMAKE_SOURCE_DIR}/libinspirecommon)
include_directories(${CMAKE_BINARY_DIR}/libinspirecommon)

################# newt target #################
### plugin classes
set(inspiredecoration_SRCS
    inspirebutton.cpp
    inspiredecoration.cpp
    inspireexceptionlist.cpp
    inspiresettingsprovider.cpp
    inspiresizegrip.cpp)

kconfig_add_kcfg_files(inspiredecoration_SRCS inspiresettings.kcfgc)

### config classes
### they are kept separately because they might move in a separate library in the future
set(inspiredecoration_config_SRCS
    config/inspireconfigwidget.cpp
    config/inspiredetectwidget.cpp
    config/inspireexceptiondialog.cpp
    config/inspireexceptionlistwidget.cpp
    config/inspireexceptionmodel.cpp
    config/inspireitemmodel.cpp
)

set(inspiredecoration_config_PART_FORMS
   config/ui/inspireconfigurationui.ui
   config/ui/inspiredetectwidget.ui
   config/ui/inspireexceptiondialog.ui
   config/ui/inspireexceptionlistwidget.ui
)

ki18n_wrap_ui(inspiredecoration_config_PART_FORMS_HEADERS ${inspiredecoration_config_PART_FORMS})

### build library
add_library(inspiredecoration MODULE
    ${inspiredecoration_SRCS}
    ${inspiredecoration_config_SRCS}
    ${inspiredecoration_config_PART_FORMS_HEADERS})

target_link_libraries(inspiredecoration
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::DBus
    PRIVATE
        inspirecommon5
        KDecoration2::KDecoration
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::ConfigWidgets
        KF5::GuiAddons
        KF5::I18n
        KF5::IconThemes
        KF5::WindowSystem)

if(INSPIRE_HAVE_X11)
  target_link_libraries(inspiredecoration
    PUBLIC
      Qt::X11Extras
      XCB::XCB)
endif()


install(TARGETS inspiredecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2)
install(FILES config/inspiredecorationconfig.desktop DESTINATION  ${KDE_INSTALL_KSERVICES5DIR})
