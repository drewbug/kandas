SET(kandas_base_SRCS
    base-worker.cpp
)
qt4_add_dbus_interface(kandas_base_SRCS ${CMAKE_SOURCE_DIR}/kandas-daemon/org.kandas.xml interface)

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
