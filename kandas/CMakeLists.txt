project(kandas)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(kandas_SRCS
    iconwidget.cpp
    main.cpp
    manager.cpp
    ndasdata.cpp
    ndasdevice.cpp
    ndasmodel.cpp
    ndasslot.cpp
    ndassystemmodel.cpp
    view.cpp
    viewdelegate.cpp
    window.cpp
)
find_path(KANDASD_INTERFACE_PATH NAMES org.kandas.xml PATH_SUFFIXES kandasd)
qt4_add_dbus_interface(kandas_SRCS ${KANDASD_INTERFACE_PATH}/org.kandas.xml interface)

kde4_add_executable(kandas ${kandas_SRCS})
target_link_libraries(kandas ${KDE4_KDEUI_LIBS})

install(TARGETS kandas  DESTINATION ${BIN_INSTALL_DIR})
install(FILES kandasui.rc  DESTINATION ${DATA_INSTALL_DIR}/kandas)
install(FILES kandas.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
