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

# 设置 vcpkg 工具链文件
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# 查找 wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base stc aui)
include(${wxWidgets_USE_FILE})

# IDE 源文件
set(IDE_SOURCES
    src/main.cpp
    src/LaminaIDE.cpp
    src/LaminaIDE.h
    src/CodeCell.cpp
    src/CodeCell.h
    src/TextCell.cpp
    src/TextCell.h
    src/LaminaHighlighter.cpp
    src/LaminaHighlighter.h
)

# 创建主执行文件
add_executable(LaminaIDE ${IDE_SOURCES})

# 设置包含目录
target_include_directories(LaminaIDE PRIVATE
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
endif()

# 设置输出目录
set_target_properties(LaminaIDE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# 安装规则
install(TARGETS LaminaIDE
    RUNTIME DESTINATION bin
)
