project(kandasd)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(kandasd_SRCS
    engine.cpp
    main.cpp
)
qt4_add_dbus_adaptor(kandasd_SRCS org.kde.kandas.xml engine.h Kandas::Daemon::Engine)

kde4_add_executable(kandasd ${kandasd_SRCS})
target_link_libraries(kandasd ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kandasd  DESTINATION ${SBIN_INSTALL_DIR})
install(FILES kandas.conf DESTINATION /etc/dbus-1/system.d)
install(FILES definitions.h org.kde.kandas.xml DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/kandasd)
