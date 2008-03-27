project(kandas)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(kandas_SRCS
    devicemodel.cpp
    main.cpp
    manager.cpp
    slotmodel.cpp
    view.cpp
    window.cpp
)
qt4_add_dbus_interface(kandas_SRCS ${KDE4_INCLUDE_DIR}/kandasd/org.kandas.xml interface)

kde4_add_executable(kandas ${kandas_SRCS})
target_link_libraries(kandas ${KDE4_KDEUI_LIBS})

install(TARGETS kandas  DESTINATION ${BIN_INSTALL_DIR})
install(FILES kandasui.rc  DESTINATION ${DATA_INSTALL_DIR}/kandas)
install(FILES kandas.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
