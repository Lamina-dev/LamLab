#ifndef LAMINA_IDE_H
#define LAMINA_IDE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "ProcessExecutor.h"

// ImGui 前向声明
struct ImGuiContext;
struct GLFWwindow;

// 编辑器窗口类型
enum class WindowType {
    TextEditor,
    ProjectExplorer,
    Console,
    Output,
    Properties,
    Debugger
};

// 基础窗口接口
class IDEWindow {
public:
    virtual ~IDEWindow() = default;
    virtual void Render() = 0;
    virtual const char* GetTitle() const = 0;
    virtual WindowType GetType() const = 0;
    virtual bool IsVisible() const { return visible_; }
    virtual void SetVisible(bool visible) { visible_ = visible; }

protected:
    bool visible_ = true;
};

// 主 IDE 应用程序类
class LaminaIDE {
public:
    LaminaIDE();
    ~LaminaIDE();
    
    // 应用程序生命周期
    bool Initialize();
    void Run();
    void Shutdown();
    
    // 窗口管理
    void AddWindow(std::unique_ptr<IDEWindow> window);
    void RemoveWindow(WindowType type);
    IDEWindow* GetWindow(WindowType type);
    
    // 文件操作
    void NewFile();
    void OpenFile();
    void SaveFile();
    void SaveAsFile();
    void CloseFile();
    
    // 项目操作
    void NewProject();
    void OpenProject();
    void CloseProject();
    
    // 构建操作
    void Build();
    void RunProject();
    void Debug();
    void Stop();
    
    // 解释器设置
    void SetInterpreterPath(const std::string& path);
    const std::string& GetInterpreterPath() const;
    void ConfigureInterpreter();
    void RunCurrentFile();
    
private:
    // 窗口和上下文
    GLFWwindow* window_;
    ImGuiContext* imgui_context_;
    
    // IDE 窗口
    std::unordered_map<WindowType, std::unique_ptr<IDEWindow>> windows_;
    
    // 应用程序状态
    bool show_demo_window_;
    bool show_about_window_;
    bool show_style_editor_;
    bool show_interpreter_config_;
    bool docking_enabled_;
    
    // 当前项目信息
    std::string current_project_path_;
    std::string current_file_path_;
    bool project_loaded_;
    
    // Lamina 解释器设置
    std::string interpreter_path_;
    std::string interpreter_args_;
    
    // 进程执行器
    ProcessExecutor process_executor_;
    
    // 私有方法
    void SetupImGui();
    void SetupDocking();
    void RenderMainMenuBar();
    void RenderToolBar();
    void RenderStatusBar();
    void RenderDockSpace();
    void RenderWindows();
    void RenderAboutDialog();
    void RenderStyleEditor();
    void RenderInterpreterConfigDialog();
    
    // 工具方法
    void LoadDefaultLayout();
    void SaveLayout();
    void LoadLayout();
    void SetTheme(int theme_id);
    
    // 事件处理
    void HandleKeyboardShortcuts();
    void HandleDropFiles(const std::vector<std::string>& files);
    
    // 样式和主题
    void ApplyDarkTheme();
    void ApplyLightTheme();
    void ApplyCustomTheme();
};

#endif // LAMINA_IDE_H
