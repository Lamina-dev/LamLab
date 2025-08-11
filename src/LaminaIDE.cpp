#include "LaminaIDE.h"
#include "TextEditorWindow.h"
#include "ProjectExplorerWindow.h"
#include "ConsoleWindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "nfd.h"
#include <iostream>
#include <fstream>

// 防止 Windows API 名称冲突
#ifdef _WIN32
#undef CreateWindow
#undef MessageBox
#include <windows.h>
#endif

LaminaIDE::LaminaIDE()
    : window_(nullptr)
    , imgui_context_(nullptr)
    , show_demo_window_(false)
    , show_about_window_(false)
    , show_style_editor_(false)
    , show_interpreter_config_(false)
    , docking_enabled_(true)
    , project_loaded_(false)
    , interpreter_path_("lamina")  // 默认解释器名称
    , interpreter_args_("")
{
}

LaminaIDE::~LaminaIDE()
{
    Shutdown();
}

bool LaminaIDE::Initialize()
{
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // 设置 OpenGL 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // 创建窗口
    window_ = glfwCreateWindow(1600, 900, "LaminaLab", nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // 启用 VSync
    
    // 设置窗口图标
    // TODO: 加载图标文件
    
    // 初始化 ImGui
    SetupImGui();
    
    // 初始化文件对话框库
    NFD_Init();
    
    // 创建默认窗口
    AddWindow(std::make_unique<TextEditorWindow>());
    AddWindow(std::make_unique<ProjectExplorerWindow>());
    AddWindow(std::make_unique<ConsoleWindow>());
    
    // 设置回调
    auto* project_explorer = dynamic_cast<ProjectExplorerWindow*>(GetWindow(WindowType::ProjectExplorer));
    auto* text_editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    
    if (project_explorer && text_editor) {
        project_explorer->SetFileOpenCallback([text_editor](const std::string& path) {
            text_editor->OpenDocument(path);
        });
    }
    
    if (console) {
        console->AddInfo("LaminaLab started successfully");
        console->AddInfo("Ready to create amazing Lamina projects!");
    }
    
    // 加载默认布局
    LoadDefaultLayout();
    
    return true;
}

void LaminaIDE::SetupImGui()
{
    // 创建 ImGui 上下文
    imgui_context_ = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // 启用导航和多 DPI 支持（如果版本支持的话）
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // 注意：某些较旧版本的 ImGui 可能不支持 Docking 和 Viewports
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // 设置样式
    ApplyDarkTheme();
    
    // 当 viewports 启用时，调整窗口圆角和边框（如果支持的话）
    ImGuiStyle& style = ImGui::GetStyle();
    // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //     style.WindowRounding = 0.0f;
    //     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    // }
    
    // 初始化平台和渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // 加载字体
    // TODO: 加载自定义字体
    io.Fonts->AddFontDefault();
}

void LaminaIDE::Run()
{
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        
        // 开始新的 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // 处理键盘快捷键
        HandleKeyboardShortcuts();
        
        // 渲染 Docking Space
        RenderDockSpace();
        
        // 渲染主菜单栏
        RenderMainMenuBar();
        
        // 渲染所有窗口
        RenderWindows();
        
        // 渲染对话框
        if (show_about_window_) {
            RenderAboutDialog();
        }
        
        if (show_style_editor_) {
            RenderStyleEditor();
        }
        
        if (show_interpreter_config_) {
            RenderInterpreterConfigDialog();
        }
        
        if (show_demo_window_) {
            ImGui::ShowDemoWindow(&show_demo_window_);
        }
        
        // 渲染
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // 更新和渲染额外的平台窗口（如果支持 Viewports）
        // ImGuiIO& io = ImGui::GetIO();
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        //     GLFWwindow* backup_current_context = glfwGetCurrentContext();
        //     ImGui::UpdatePlatformWindows();
        //     ImGui::RenderPlatformWindowsDefault();
        //     glfwMakeContextCurrent(backup_current_context);
        // }
        
        glfwSwapBuffers(window_);
    }
}

void LaminaIDE::Shutdown()
{
    // 保存布局
    SaveLayout();
    
    // 清理窗口
    windows_.clear();
    
    // 清理 ImGui
    if (imgui_context_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(imgui_context_);
        imgui_context_ = nullptr;
    }
    
    // 清理 GLFW
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
    
    // 清理文件对话框库
    NFD_Quit();
}

void LaminaIDE::RenderDockSpace()
{
    // 简化版本：不使用 Docking，直接渲染窗口
    // 如果需要 Docking，需要更新 ImGui 版本
    return;
}

void LaminaIDE::RenderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        // 文件菜单
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New File", "Ctrl+N")) {
                NewFile();
            }
            if (ImGui::MenuItem("Open File", "Ctrl+O")) {
                OpenFile();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) {
                NewProject();
            }
            if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O")) {
                OpenProject();
            }
            if (ImGui::BeginMenu("Recent Projects")) {
                ImGui::MenuItem("No recent projects");
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                SaveFile();
            }
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
                SaveAsFile();
            }
            if (ImGui::MenuItem("Save All", "Ctrl+Shift+A")) {
                // TODO: 实现保存所有文件
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close File", "Ctrl+W")) {
                CloseFile();
            }
            if (ImGui::MenuItem("Close Project")) {
                CloseProject();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(window_, true);
            }
            ImGui::EndMenu();
        }
        
        // 编辑菜单
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Cut();
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Copy();
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Paste();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Find", "Ctrl+F")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Find();
            }
            if (ImGui::MenuItem("Replace", "Ctrl+H")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->Replace();
            }
            if (ImGui::MenuItem("Go to Line", "Ctrl+G")) {
                auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
                if (editor) editor->GoToLine();
            }
            ImGui::EndMenu();
        }
        
        // 视图菜单
        if (ImGui::BeginMenu("View")) {
            bool text_editor_visible = windows_[WindowType::TextEditor]->IsVisible();
            bool project_explorer_visible = windows_[WindowType::ProjectExplorer]->IsVisible();
            bool console_visible = windows_[WindowType::Console]->IsVisible();
            
            if (ImGui::MenuItem("Text Editor", nullptr, &text_editor_visible)) {
                windows_[WindowType::TextEditor]->SetVisible(text_editor_visible);
            }
            if (ImGui::MenuItem("Project Explorer", nullptr, &project_explorer_visible)) {
                windows_[WindowType::ProjectExplorer]->SetVisible(project_explorer_visible);
            }
            if (ImGui::MenuItem("Console", nullptr, &console_visible)) {
                windows_[WindowType::Console]->SetVisible(console_visible);
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Theme")) {
                if (ImGui::MenuItem("Dark")) {
                    ApplyDarkTheme();
                }
                if (ImGui::MenuItem("Light")) {
                    ApplyLightTheme();
                }
                if (ImGui::MenuItem("Custom")) {
                    ApplyCustomTheme();
                }
                ImGui::EndMenu();
            }
            ImGui::MenuItem("Style Editor", nullptr, &show_style_editor_);
            ImGui::EndMenu();
        }
        
        // 构建菜单
        if (ImGui::BeginMenu("Build")) {
            if (ImGui::MenuItem("Build Project", "F7")) {
                Build();
            }
            if (ImGui::MenuItem("Rebuild All", "Ctrl+F7")) {
                // TODO: 实现重新构建
            }
            if (ImGui::MenuItem("Clean", "Ctrl+Shift+F7")) {
                // TODO: 实现清理
            }
            ImGui::EndMenu();
        }
        
        // 运行菜单
        if (ImGui::BeginMenu("Run")) {
            if (ImGui::MenuItem("Run Current File", "Ctrl+F5")) {
                RunCurrentFile();
            }
            if (ImGui::MenuItem("Run Project", "F5")) {
                RunProject();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Configure Interpreter...")) {
                show_interpreter_config_ = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Debug", "Shift+F5")) {
                Debug();
            }
            if (ImGui::MenuItem("Stop", "Shift+F5")) {
                Stop();
            }
            ImGui::EndMenu();
        }
        
        // 帮助菜单
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About LaminaLab")) {
                show_about_window_ = true;
            }
            if (ImGui::MenuItem("Documentation")) {
                // TODO: 打开文档
            }
            if (ImGui::MenuItem("Lamina Language Reference")) {
                // TODO: 打开语言参考
            }
            ImGui::Separator();
            ImGui::MenuItem("Demo Window", nullptr, &show_demo_window_);
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void LaminaIDE::RenderWindows()
{
    for (auto& [type, window] : windows_) {
        if (window && window->IsVisible()) {
            window->Render();
        }
    }
}

void LaminaIDE::RenderAboutDialog()
{
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("About LaminaLab", &show_about_window_)) {
        ImGui::Text("LaminaLab");
        ImGui::Text("Version 1.0.0");
        ImGui::Separator();
        ImGui::Text("A modern IDE for the Lamina programming language");
        ImGui::Text("Built with Dear ImGui and modern C++");
        ImGui::Spacing();
        ImGui::Text("Features:");
        ImGui::BulletText("Syntax highlighting for Lamina");
        ImGui::BulletText("Project management");
        ImGui::BulletText("Integrated debugging");
        ImGui::BulletText("Extensible plugin system");
        ImGui::Spacing();
        ImGui::Text("© 2025 Lamina Development Team");
        
        if (ImGui::Button("Close")) {
            show_about_window_ = false;
        }
    }
    ImGui::End();
}

void LaminaIDE::RenderStyleEditor()
{
    ImGui::Begin("Style Editor", &show_style_editor_);
    ImGui::ShowStyleEditor();
    ImGui::End();
}

void LaminaIDE::RenderInterpreterConfigDialog()
{
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Interpreter Configuration", &show_interpreter_config_)) {
        ImGui::Text("Lamina Interpreter Settings");
        ImGui::Separator();
        
        ImGui::Text("Interpreter Path:");
        static char interpreter_path_buffer[512];
        if (ImGui::IsWindowAppearing()) {
            strcpy_s(interpreter_path_buffer, sizeof(interpreter_path_buffer), interpreter_path_.c_str());
        }
        
        if (ImGui::InputText("##InterpreterPath", interpreter_path_buffer, sizeof(interpreter_path_buffer))) {
            interpreter_path_ = interpreter_path_buffer;
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            nfdchar_t* outPath = nullptr;
            nfdfilteritem_t filterItem[1] = { { "Executable Files", "exe" } };
            
            nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);
            if (result == NFD_OKAY) {
                interpreter_path_ = outPath;
                strcpy_s(interpreter_path_buffer, sizeof(interpreter_path_buffer), interpreter_path_.c_str());
                free(outPath);
            }
        }
        
        ImGui::Spacing();
        ImGui::Text("Additional Arguments:");
        static char args_buffer[256];
        if (ImGui::IsWindowAppearing()) {
            strcpy_s(args_buffer, sizeof(args_buffer), interpreter_args_.c_str());
        }
        
        if (ImGui::InputText("##InterpreterArgs", args_buffer, sizeof(args_buffer))) {
            interpreter_args_ = args_buffer;
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Test the interpreter:");
        
        if (ImGui::Button("Test Interpreter")) {
            auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
            if (console) {
                std::string test_command = interpreter_path_ + " --version";
                console->AddInfo("Testing interpreter: " + test_command);
                
                // TODO: 实际执行命令并捕获输出
                console->AddWarning("Interpreter test not fully implemented yet");
            }
        }
        
        ImGui::Spacing();
        if (ImGui::Button("OK")) {
            show_interpreter_config_ = false;
            
            auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
            if (console) {
                console->AddSuccess("Interpreter configuration updated");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            show_interpreter_config_ = false;
        }
    }
    ImGui::End();
}

void LaminaIDE::AddWindow(std::unique_ptr<IDEWindow> window)
{
    if (window) {
        WindowType type = window->GetType();
        windows_[type] = std::move(window);
    }
}

void LaminaIDE::RemoveWindow(WindowType type)
{
    windows_.erase(type);
}

IDEWindow* LaminaIDE::GetWindow(WindowType type)
{
    auto it = windows_.find(type);
    return (it != windows_.end()) ? it->second.get() : nullptr;
}

void LaminaIDE::NewFile()
{
    auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
    if (editor) {
        editor->NewDocument();
    }
}

void LaminaIDE::OpenFile()
{
    nfdchar_t* outPath = nullptr;
    nfdfilteritem_t filterItem[1] = { { "Lamina Files", "lm,txt" } };
    
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);
    
    if (result == NFD_OKAY) {
        auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
        if (editor) {
            editor->OpenDocument(outPath);
        }
        free(outPath);
    }
}

void LaminaIDE::SaveFile()
{
    auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
    if (editor) {
        editor->SaveDocument();
    }
}

void LaminaIDE::SaveAsFile()
{
    auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
    if (editor) {
        editor->SaveDocumentAs();
    }
}

void LaminaIDE::CloseFile()
{
    auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
    if (editor) {
        int active_tab = editor->GetActiveTabIndex();
        if (active_tab >= 0) {
            editor->CloseDocument(active_tab);
        }
    }
}

void LaminaIDE::NewProject()
{
    // TODO: 实现新建项目
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    if (console) {
        console->AddInfo("New project creation not implemented yet");
    }
}

void LaminaIDE::OpenProject()
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(&outPath, nullptr);
    
    if (result == NFD_OKAY) {
        auto* project_explorer = dynamic_cast<ProjectExplorerWindow*>(GetWindow(WindowType::ProjectExplorer));
        if (project_explorer) {
            project_explorer->LoadProject(outPath);
            current_project_path_ = outPath;
            project_loaded_ = true;
        }
        
        auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
        if (console) {
            console->AddSuccess("Project opened: " + std::string(outPath));
        }
        
        free(outPath);
    }
}

void LaminaIDE::CloseProject()
{
    auto* project_explorer = dynamic_cast<ProjectExplorerWindow*>(GetWindow(WindowType::ProjectExplorer));
    if (project_explorer) {
        project_explorer->CloseProject();
    }
    
    current_project_path_.clear();
    project_loaded_ = false;
    
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    if (console) {
        console->AddInfo("Project closed");
    }
}

void LaminaIDE::Build()
{
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    if (console) {
        console->AddInfo("Building project...");
        // TODO: 实现构建系统
        console->AddError("Build system not implemented yet");
    }
}

void LaminaIDE::RunProject()
{
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    if (console) {
        console->AddInfo("Running project...");
        // TODO: 实现运行系统
        console->AddError("Run system not implemented yet");
    }
}

void LaminaIDE::Debug()
{
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    if (console) {
        console->AddInfo("Starting debug session...");
        // TODO: 实现调试系统
        console->AddError("Debug system not implemented yet");
    }
}

void LaminaIDE::Stop()
{
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    if (console) {
        console->AddInfo("Stopping execution...");
    }
}

void LaminaIDE::HandleKeyboardShortcuts()
{
    ImGuiIO& io = ImGui::GetIO();
    
    if (io.KeyCtrl) {
        if (ImGui::IsKeyPressed(ImGuiKey_N) && !io.KeyShift) {
            NewFile();
        } else if (ImGui::IsKeyPressed(ImGuiKey_N) && io.KeyShift) {
            NewProject();
        } else if (ImGui::IsKeyPressed(ImGuiKey_O) && !io.KeyShift) {
            OpenFile();
        } else if (ImGui::IsKeyPressed(ImGuiKey_O) && io.KeyShift) {
            OpenProject();
        } else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
            SaveFile();
        } else if (ImGui::IsKeyPressed(ImGuiKey_W)) {
            CloseFile();
        } else if (ImGui::IsKeyPressed(ImGuiKey_F5)) {
            RunCurrentFile();  // Ctrl+F5 运行当前文件
        }
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_F5) && !io.KeyCtrl) {
        if (io.KeyShift) {
            Debug();
        } else {
            RunProject();
        }
    } else if (ImGui::IsKeyPressed(ImGuiKey_F7)) {
        Build();
    }
}

void LaminaIDE::LoadDefaultLayout()
{
    // TODO: 实现默认布局加载
}

void LaminaIDE::SaveLayout()
{
    // TODO: 实现布局保存
}

void LaminaIDE::LoadLayout()
{
    // TODO: 实现布局加载
}

void LaminaIDE::ApplyDarkTheme()
{
    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
}

void LaminaIDE::ApplyLightTheme()
{
    ImGui::StyleColorsLight();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
}

void LaminaIDE::ApplyCustomTheme()
{
    // TODO: 实现自定义主题
    ApplyDarkTheme();
}

void LaminaIDE::SetInterpreterPath(const std::string& path)
{
    interpreter_path_ = path;
}

const std::string& LaminaIDE::GetInterpreterPath() const
{
    return interpreter_path_;
}

void LaminaIDE::ConfigureInterpreter()
{
    show_interpreter_config_ = true;
}

void LaminaIDE::RunCurrentFile()
{
    auto* editor = dynamic_cast<TextEditorWindow*>(GetWindow(WindowType::TextEditor));
    auto* console = dynamic_cast<ConsoleWindow*>(GetWindow(WindowType::Console));
    
    if (!editor || !console) {
        return;
    }
    
    auto* active_doc = editor->GetActiveDocument();
    if (!active_doc) {
        console->AddError("No active document to run");
        return;
    }
    
    // 检查是否是 Lamina 文件
    if (!active_doc->file_path.ends_with(".lm")) {
        console->AddError("Can only run Lamina (.lm) files");
        return;
    }
    
    // 检查文件是否已保存
    if (active_doc->is_modified) {
        console->AddWarning("File has unsaved changes, saving...");
        if (!editor->SaveDocument()) {
            console->AddError("Failed to save file");
            return;
        }
    }
    
    if (active_doc->file_path.empty()) {
        console->AddError("File must be saved before running");
        return;
    }
    
    // 检查解释器路径
    if (interpreter_path_.empty()) {
        console->AddError("Lamina interpreter not configured. Use Run → Configure Interpreter...");
        return;
    }
    
    // 构建运行命令
    std::string command = interpreter_path_;
    if (!interpreter_args_.empty()) {
        command += " " + interpreter_args_;
    }
    command += " \"" + active_doc->file_path + "\"";
    
    console->AddInfo("Running: " + command);
    console->AddInfo("File: " + active_doc->file_path);
    
    // 获取工作目录
    std::string working_dir = std::filesystem::path(active_doc->file_path).parent_path().string();
    
    // 异步执行命令
    process_executor_.ExecuteAsync(
        command,
        working_dir,
        [console](const std::string& output, bool is_error) {
            // 输出回调 - 实时显示输出
            if (!output.empty()) {
                if (is_error) {
                    console->AddError(output);
                } else {
                    console->AddInfo(output);
                }
            }
        },
        [console](const ProcessResult& result) {
            // 完成回调
            if (result.success) {
                console->AddSuccess("Program finished with exit code: " + std::to_string(result.exit_code));
            } else {
                console->AddError("Program failed with exit code: " + std::to_string(result.exit_code));
                if (!result.error.empty()) {
                    console->AddError("Error output: " + result.error);
                }
            }
        }
    );
}
