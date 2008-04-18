project(kandas-console)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINTIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

SET(kandas_base_SRCS
    base-worker.cpp
)
qt4_add_dbus_interface(kandas_base_SRCS ${KDE4_INCLUDE_DIR}/kandasd/org.kandas.xml interface)

set(kandas_info_SRCS
    ${kandas_base_SRCS}
    info-main.cpp
    info-worker.h
    info-worker.cpp
)
set(kandas_up_SRCS
    ${kandas_base_SRCS}
    up-main.cpp
    up-worker.h
    up-worker.cpp
)
set(kandas_down_SRCS
    ${kandas_base_SRCS}
    down-main.cpp
    down-worker.h
    down-worker.cpp
)

kde4_add_executable(kandas-info ${kandas_info_SRCS})
target_link_libraries(kandas-info ${KDE4_KDEUI_LIBS})
kde4_add_executable(kandas-up ${kandas_up_SRCS})
target_link_libraries(kandas-up ${KDE4_KDEUI_LIBS})
kde4_add_executable(kandas-down ${kandas_down_SRCS})
target_link_libraries(kandas-down ${KDE4_KDEUI_LIBS})

install (TARGETS kandas-info kandas-up kandas-down DESTINATION ${BIN_INSTALL_DIR})
