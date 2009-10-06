set(kandas_SRCS
    adddialog.cpp
    connectmenu.cpp
    iconwidget.cpp
    main.cpp
    manager.cpp
    ndasdata.cpp
    ndasdevice.cpp
    ndasmodel.cpp
    ndasslot.cpp
    ndassystemmodel.cpp
    removedialog.cpp
    view.cpp
    viewdelegate.cpp
    window.cpp
)
find_path(KANDASD_INTERFACE_PATH NAMES org.kandas.xml PATH_SUFFIXES kandasd)
qt4_add_dbus_interface(kandas_SRCS ${CMAKE_SOURCE_DIR}/kandasd/org.kandas.xml interface)
kde4_add_ui_files(kandas_SRCS adddialog.ui)

kde4_add_executable(kandas ${kandas_SRCS})
target_link_libraries(kandas ${KDE4_KDEUI_LIBS})

install(TARGETS kandas  DESTINATION ${BIN_INSTALL_DIR})
install(FILES kandasui.rc  DESTINATION ${DATA_INSTALL_DIR}/kandas)
install(FILES kandas.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
