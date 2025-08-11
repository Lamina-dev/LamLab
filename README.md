# LaminaLab

一个现代化的 Lamina 编程语言集成开发环境，使用 Dear ImGui 构建。

## 功能特性

### 已实现功能
- ✅ **现代化 UI**: 基于 Dear ImGui 的响应式界面
- ✅ **Docking 支持**: 灵活的多窗口布局系统
- ✅ **文本编辑器**: 多标签页编辑器，支持语法高亮
- ✅ **项目资源管理器**: 文件树浏览，文件操作
- ✅ **控制台输出**: 多级别日志系统
- ✅ **菜单系统**: 完整的菜单栏和快捷键支持
- ✅ **主题支持**: 暗色/亮色主题切换

### 计划中功能
- 🔄 **Lamina 语法高亮**: 专为 Lamina 语言优化的语法高亮
- 🔄 **智能代码补全**: 基于语言服务器的代码补全
- 🔄 **集成调试器**: 断点、变量查看、调用栈
- 🔄 **构建系统**: 项目构建和管理
- 🔄 **插件系统**: 可扩展的插件架构
- 🔄 **Git 集成**: 版本控制支持

## 项目结构

```
LaminaLab/
├── include/           # 头文件
│   ├── LaminaIDE.h
│   ├── TextEditorWindow.h
│   ├── ProjectExplorerWindow.h
│   └── ConsoleWindow.h
├── src/              # 源文件
│   ├── main.cpp
│   ├── LaminaIDE.cpp
│   ├── TextEditorWindow.cpp
│   ├── ProjectExplorerWindow.cpp
│   └── ConsoleWindow.cpp
├── third_party/      # 第三方库 (Git 子模块)
│   ├── imgui/        # Dear ImGui
│   ├── glfw/         # GLFW 窗口库
│   ├── nfd/          # Native File Dialog
│   └── glad/         # OpenGL 加载器 (需手动配置)
├── assets/           # 资源文件
└── build/           # 构建输出目录
```

## 依赖库

项目使用 Git 子模块管理以下依赖：

- **Dear ImGui**: 立即模式 GUI 库
- **GLFW**: 跨平台窗口和输入管理
- **NFD (NativeFileDialog-extended)**: 跨平台文件对话框
- **OpenGL**: 图形渲染 API

## 构建说明

### 前置要求

- CMake 3.16 或更高版本
- 支持 C++20 的编译器 (MSVC 2019+, GCC 10+, Clang 10+)
- Git (用于子模块管理)

### 构建步骤

1. **克隆项目并初始化子模块**
   ```bash
   git clone https://github.com/Lamina-dev/LamLab.git
   cd LamLab
   git submodule update --init --recursive
   ```

2. **配置 GLAD (重要)**
   - 访问 https://glad.dav1d.de/
   - 选择设置：
     - Language: C/C++
     - API: OpenGL
     - Version: 3.3 Core
     - Options: 勾选 "Generate a loader"
   - 下载并解压到 `third_party/glad/`
   - 更新 `third_party/glad/CMakeLists.txt`

3. **构建项目**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

4. **运行 IDE**
   ```bash
   # Windows
   .\bin\Release\LaminaIDE.exe
   
   # Linux/macOS
   ./bin/Release/LaminaIDE
   ```

## 使用说明

### 基本操作

1. **新建文件**: `Ctrl+N` 或 文件 → 新建文件
2. **打开文件**: `Ctrl+O` 或 文件 → 打开文件
3. **保存文件**: `Ctrl+S` 或 文件 → 保存
4. **新建项目**: `Ctrl+Shift+N` 或 文件 → 新建项目
5. **打开项目**: `Ctrl+Shift+O` 或 文件 → 打开项目

### 窗口管理

- **Docking**: 拖拽窗口标题栏到其他位置实现窗口停靠
- **多标签页**: 编辑器支持多个文件同时打开
- **布局保存**: 窗口布局会自动保存和恢复

### 快捷键

| 功能 | 快捷键 |
|------|--------|
| 新建文件 | Ctrl+N |
| 打开文件 | Ctrl+O |
| 保存文件 | Ctrl+S |
| 另存为 | Ctrl+Shift+S |
| 关闭文件 | Ctrl+W |
| 新建项目 | Ctrl+Shift+N |
| 打开项目 | Ctrl+Shift+O |
| 查找 | Ctrl+F |
| 替换 | Ctrl+H |
| 跳转到行 | Ctrl+G |
| 运行项目 | F5 |
| 调试项目 | Shift+F5 |
| 构建项目 | F7 |

## 技术架构

### 核心组件

- **LaminaIDE**: 主应用程序类，管理整个 IDE 生命周期
- **IDEWindow**: 窗口基类，所有 IDE 窗口都继承自此类
- **TextEditorWindow**: 文本编辑器窗口，支持多标签页和语法高亮
- **ProjectExplorerWindow**: 项目资源管理器，文件树浏览和操作
- **ConsoleWindow**: 控制台输出窗口，多级别日志系统

### 设计模式

- **工厂模式**: 窗口创建和管理
- **观察者模式**: 事件回调系统
- **单例模式**: 主应用程序实例
- **命令模式**: 菜单和快捷键处理

## 开发路线图

### Phase 1: 基础框架 ✅
- [x] ImGui 集成和窗口系统
- [x] 基础 UI 组件
- [x] 文件操作支持
- [x] 项目管理基础

### Phase 2: 编辑器增强 🔄
- [ ] 语法高亮引擎
- [ ] 代码折叠
- [ ] 智能缩进
- [ ] 查找替换增强

### Phase 3: Lamina 语言支持 🔄
- [ ] Lamina 语法解析器
- [ ] 语义分析
- [ ] 错误检测和提示
- [ ] 代码补全

### Phase 4: 高级功能 📋
- [ ] 集成调试器
- [ ] 性能分析工具
- [ ] 版本控制集成
- [ ] 插件系统

## 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 致谢

- [Dear ImGui](https://github.com/ocornut/imgui) - 出色的立即模式 GUI 库
- [GLFW](https://github.com/glfw/glfw) - 跨平台窗口库
- [NativeFileDialog-extended](https://github.com/btzy/nativefiledialog-extended) - 文件对话框库

---

© 2025 Lamina Development Team
