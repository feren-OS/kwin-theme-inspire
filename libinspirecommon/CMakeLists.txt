################# dependencies #################
### Qt/KDE
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets)

################# inspirestyle target #################
set(inspirecommon_LIB_SRCS
    inspireboxshadowrenderer.cpp
)

add_library(inspirecommon5 ${inspirecommon_LIB_SRCS})

generate_export_header(inspirecommon5
    BASE_NAME inspirecommon
    EXPORT_FILE_NAME inspirecommon_export.h)

target_link_libraries(inspirecommon5
    PUBLIC
        Qt::Core
        Qt::Gui)

set_target_properties(inspirecommon5 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS inspirecommon5 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
