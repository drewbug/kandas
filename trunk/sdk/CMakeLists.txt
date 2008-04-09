project(kandas-sdk)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(kandassimul_SRCS
    kandassimul.cpp
)

kde4_add_executable(kandassimul ${kandassimul_SRCS})
target_link_libraries(kandassimul ${KDE4_KDEUI_LIBS})

install(TARGETS kandassimul  DESTINATION ${BIN_INSTALL_DIR})
