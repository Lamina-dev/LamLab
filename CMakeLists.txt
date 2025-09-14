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

# 设置 vcpkg 工具链
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# 查找 wxWidgets
set(wxWidgets_USE_STATIC ON)
set(wxWidgets_USE_UNICODE ON)
find_package(wxWidgets REQUIRED COMPONENTS base core aui stc xml)

# 添加源文件
set(IDE_SOURCES
    src/main.cpp
    src/MainFrame.cpp
    src/LaminaApp.cpp
    src/LaminaEditor.cpp
    src/ProcessManager.cpp
    src/ThemeConfig.cpp
)

# 创建主执行文件
add_executable(LaminaIDE WIN32 ${IDE_SOURCES})

# 设置包含目录
target_include_directories(LaminaIDE PRIVATE
    include
)

# 链接库
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    target_link_libraries(LaminaIDE PRIVATE ${wxWidgets_LIBRARIES})
else()
    message(FATAL_ERROR "wxWidgets not found")
endif()

# 设置编译器标志
if(MSVC)
    target_compile_options(LaminaIDE PRIVATE /W3)
    # 添加 Unicode 支持
    target_compile_definitions(LaminaIDE PRIVATE UNICODE _UNICODE)
else()
    target_compile_options(LaminaIDE PRIVATE -Wall -Wextra)
endif()

# 设置输出目录
set_target_properties(LaminaIDE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# 复制配置文件
add_custom_command(TARGET LaminaIDE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/Debug/config"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/Release/config"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/config/themes.xml"
        "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/config/themes.xml"
    COMMENT "Copying configuration files to output directory"
)
