cmake_minimum_required(VERSION 4.0)
project(LamLab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(EXISTS "${CMAKE_SOURCE_DIR}/qt_path.txt")
    file(READ "${CMAKE_SOURCE_DIR}/qt_path.txt" QT_PATH)
    string(STRIP "${QT_PATH}" QT_PATH)
    set(CMAKE_PREFIX_PATH "${QT_PATH}")
    message(STATUS "Set CMAKE_PREFIX_PATH from qt_path.txt: ${CMAKE_PREFIX_PATH}")
else()
    message(WARNING "qt_path.txt not found in project root directory")
endif()

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(LamLab src/main.cpp
        src/LaminaHighlighter.cpp
        src/LaminaHighlighter.h
        src/CodeCell.cpp
        src/CodeCell.h
        src/TextCell.cpp
        src/TextCell.h
        src/LaminaIDE.cpp
        src/LaminaIDE.h
)
target_link_libraries(LamLab
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

if (WIN32)
    if (MSVC)
        target_link_options(LamLab PRIVATE "/SUBSYSTEM:WINDOWS")
    elseif (MINGW)
        target_link_options(LamLab PRIVATE "-Wl,-subsystem,windows")
    endif()
endif()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
