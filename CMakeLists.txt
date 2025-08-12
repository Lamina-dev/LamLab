cmake_minimum_required(VERSION 3.20)
project(LaminaIDE 
    VERSION 0.1.0
    DESCRIPTION "Lamina Interactive Development Environment"
    LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

<<<<<<< HEAD
# 设置 vcpkg 工具链文件
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# 查找 wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base stc aui)
include(${wxWidgets_USE_FILE})
=======
# 设置 vcpkg 工具链
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# 查找 wxWidgets
find_package(wxWidgets CONFIG REQUIRED)
>>>>>>> a84a2c4 (更新CMakeLists.txt，README.md和14多个文件... 重制，使用WxWidgets)

# IDE 源文件
set(IDE_SOURCES
    src/main.cpp
<<<<<<< HEAD
    src/LaminaIDE.cpp
    src/LaminaIDE.h
    src/CodeCell.cpp
    src/CodeCell.h
    src/TextCell.cpp
    src/TextCell.h
    src/LaminaHighlighter.cpp
    src/LaminaHighlighter.h
=======
    src/MainFrame.cpp
    src/LaminaApp.cpp
    src/LaminaEditor.cpp
    src/ProcessManager.cpp
>>>>>>> a84a2c4 (更新CMakeLists.txt，README.md和14多个文件... 重制，使用WxWidgets)
)

# 创建主执行文件
add_executable(LaminaIDE WIN32 ${IDE_SOURCES})

# 设置包含目录
target_include_directories(LaminaIDE PRIVATE
<<<<<<< HEAD
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
)

# 链接 wxWidgets
target_link_libraries(LaminaIDE PRIVATE ${wxWidgets_LIBRARIES})

# 添加编译定义
target_compile_definitions(LaminaIDE PRIVATE
    WXUSINGDLL
    _UNICODE
    UNICODE
)

# Windows 特定设置
if(WIN32)
    # 设置为控制台应用程序以便调试
    set_target_properties(LaminaIDE PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
=======
    include
)

# 链接库
target_link_libraries(LaminaIDE PRIVATE
    wx::core
    wx::base
    wx::stc
    wx::aui
)

# 设置编译器标志
if(MSVC)
    target_compile_options(LaminaIDE PRIVATE /W3)
    # 添加 Unicode 支持
    target_compile_definitions(LaminaIDE PRIVATE UNICODE _UNICODE)
else()
    target_compile_options(LaminaIDE PRIVATE -Wall -Wextra)
>>>>>>> a84a2c4 (更新CMakeLists.txt，README.md和14多个文件... 重制，使用WxWidgets)
endif()

# 设置输出目录
set_target_properties(LaminaIDE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)
<<<<<<< HEAD

# 安装规则
install(TARGETS LaminaIDE
    RUNTIME DESTINATION bin
)
=======
>>>>>>> a84a2c4 (更新CMakeLists.txt，README.md和14多个文件... 重制，使用WxWidgets)
