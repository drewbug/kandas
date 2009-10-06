project(kandas)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

add_subdirectory(kandasd)
add_subdirectory(kandas-console)
add_subdirectory(kandas)
add_subdirectory(l10n)
