cmake_minimum_required(VERSION 3.16)
project(LaminaIDE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 添加第三方库
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/nfd)

# 禁用 GLFW 的一些不需要的功能
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# ImGui 源文件
set(IMGUI_SOURCES
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
)

# 创建 ImGui 库
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    third_party/imgui
    third_party/imgui/backends
)
target_link_libraries(imgui PUBLIC glfw)

# 如果使用 OpenGL，需要链接系统 OpenGL 库
find_package(OpenGL REQUIRED)

# IDE 源文件
set(IDE_SOURCES
    src/main.cpp
    src/LaminaIDE.cpp
    src/TextEditorWindow.cpp
    src/ProjectExplorerWindow.cpp
    src/ConsoleWindow.cpp
    src/ProcessExecutor.cpp
)

# 创建主执行文件
add_executable(LaminaIDE ${IDE_SOURCES})

# 设置包含目录
target_include_directories(LaminaIDE PRIVATE
    include
    third_party/imgui
    third_party/imgui/backends
    third_party/glfw/include
    third_party/nfd/src/include
)

# 链接库
target_link_libraries(LaminaIDE PRIVATE
    imgui
    glfw
    nfd
    OpenGL::GL
)

# Windows 特定设置
if(WIN32)
    target_link_libraries(LaminaIDE PRIVATE opengl32)
    # 设置为控制台应用程序以便调试
    set_target_properties(LaminaIDE PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# 设置输出目录
set_target_properties(LaminaIDE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

# 复制资源文件
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
