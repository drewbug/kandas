project(candas)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(candas_SRCS
    main.cpp
    engine.cpp
)
qt4_add_dbus_interface(candas_SRCS ${KDE4_INCLUDE_DIR}/kandasd/org.kde.kandas.xml interface)

kde4_add_executable(candas ${candas_SRCS})
target_link_libraries(candas ${KDE4_KDEUI_LIBS})

install(TARGETS candas DESTINATION ${BIN_INSTALL_DIR})
