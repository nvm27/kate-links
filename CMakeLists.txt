cmake_minimum_required(VERSION 2.6)
project (kate-links)

set (LIBRARY_TYPE SHARED)

find_package(KDE4 4.8.0 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS -DKTEXTEDITOR_NO_DEPRECATED)

include_directories (${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES} ${KDE4_KIO_INCLUDES} ${KACTIVITIES_INCLUDE_DIRS})

macro_optional_add_subdirectory (src)

macro_display_feature_log()
