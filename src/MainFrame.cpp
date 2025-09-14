#include "MainFrame.h"
#include "LaminaEditor.h"
#include "ProcessManager.h"
#include "ThemeConfig.h"
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/artprov.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(ID_SAVE_AS, MainFrame::OnSaveAs)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
    EVT_MENU(wxID_REDO, MainFrame::OnRedo)
    EVT_MENU(wxID_CUT, MainFrame::OnCut)
    EVT_MENU(wxID_COPY, MainFrame::OnCopy)
    EVT_MENU(wxID_PASTE, MainFrame::OnPaste)
    EVT_MENU(wxID_FIND, MainFrame::OnFind)
    EVT_MENU(ID_RUN, MainFrame::OnRun)
    EVT_MENU(ID_STOP, MainFrame::OnStop)
    EVT_MENU(ID_SETTINGS, MainFrame::OnSettings)
    EVT_MENU(ID_THEME_START, MainFrame::OnTheme)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_STC_CHANGE(ID_EDITOR, MainFrame::OnTextChange)
    EVT_STC_UPDATEUI(ID_EDITOR, MainFrame::OnUpdateUI)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "LaminaLab IDE v0.0.1-Alpha", wxDefaultPosition, wxSize(800, 600))
    , m_editor(nullptr)
    , m_console(nullptr)
    , m_processManager(nullptr)
    , m_isModified(false)
{
    // SetIcon(wxIcon(wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_OTHER, wxSize(32, 32))));
    
    // 确保主题配置已加载
    ThemeConfig::Get();
    
    CreateMenuBar();
    CreateStatusBar();
    CreateToolBar();
    
    InitializeAUI();
    CreateEditor();
    CreateConsole();
    
    LoadSettings();
    UpdateTitle();
}

MainFrame::~MainFrame()
{
    m_auiManager.UnInit();
}

// 添加主题菜单
void MainFrame::CreateThemeMenu(wxMenu* viewMenu)
{
    ThemeConfig& config = ThemeConfig::Get();
    const wxArrayString& themes = config.GetAvailableThemes();
    
    wxMenu* themeMenu = new wxMenu;
    for (size_t i = 0; i < themes.GetCount(); ++i)
    {
        themeMenu->AppendRadioItem(ID_THEME_START + i, themes[i]);
        if (themes[i] == config.GetCurrentTheme())
        {
            themeMenu->Check(ID_THEME_START + i, true);
        }
    }
    
    viewMenu->AppendSeparator();
    viewMenu->AppendSubMenu(themeMenu, _("主题(&T)"));
}

void MainFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();
    
    // 文件菜单
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_NEW, "&New\tCtrl+N", "Create a new file");
    fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O", "Open an existing file");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save the current file");
    fileMenu->Append(ID_SAVE_AS, "Save &As...\tCtrl+Shift+S", "Save the current file with a new name");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4", "Exit the application");
    
    // 编辑菜单
    wxMenu* editMenu = new wxMenu();
    editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z", "Undo the last action");
    editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y", "Redo the last action");
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, "Cu&t\tCtrl+X", "Cut selected text");
    editMenu->Append(wxID_COPY, "&Copy\tCtrl+C", "Copy selected text");
    editMenu->Append(wxID_PASTE, "&Paste\tCtrl+V", "Paste text from clipboard");
    editMenu->AppendSeparator();
    editMenu->Append(wxID_FIND, "&Find...\tCtrl+F", "Find text");
    
    // 运行菜单
    wxMenu* runMenu = new wxMenu();
    runMenu->Append(ID_RUN, "&Run Script\tF5", "Run the current script");
    runMenu->Append(ID_STOP, "&Stop Script\tShift+F5", "Stop the running script");
    runMenu->AppendSeparator();
    runMenu->Append(ID_SETTINGS, "&Interpreter Path...", "Configure interpreter settings");
    
    // 帮助菜单
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT, "&About", "About this application");
    
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(runMenu, "&Run");
    menuBar->Append(helpMenu, "&Help");
    
    SetMenuBar(menuBar);
}

void MainFrame::CreateStatusBar()
{
    wxFrame::CreateStatusBar(3);
    SetStatusText("Ready", 0);
    SetStatusText("Line 1, Column 1", 1);
    SetStatusText("", 2);
}

void MainFrame::CreateToolBar()
{
    wxToolBar* toolBar = wxFrame::CreateToolBar();
    
    toolBar->AddTool(wxID_NEW, "New", wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR), "New file");
    toolBar->AddTool(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR), "Open file");
    toolBar->AddTool(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR), "Save file");
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, "Cut", wxArtProvider::GetBitmap(wxART_CUT, wxART_TOOLBAR), "Cut");
    toolBar->AddTool(wxID_COPY, "Copy", wxArtProvider::GetBitmap(wxART_COPY, wxART_TOOLBAR), "Copy");
    toolBar->AddTool(wxID_PASTE, "Paste", wxArtProvider::GetBitmap(wxART_PASTE, wxART_TOOLBAR), "Paste");
    toolBar->AddSeparator();
    toolBar->AddTool(ID_RUN, "Run", wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR), "Run script");
    
    toolBar->Realize();
}

void MainFrame::InitializeAUI()
{
    m_auiManager.SetManagedWindow(this);
}

void MainFrame::CreateEditor()
{
    m_editor = new LaminaEditor(this, ID_EDITOR);
    m_auiManager.AddPane(m_editor, wxAuiPaneInfo()
        .CenterPane()
        .Name("editor")
        .Caption("Editor"));
    
    m_auiManager.Update();
}

void MainFrame::CreateConsole()
{
    m_console = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    
    // 设置控制台字体为等宽字体
    wxFont consoleFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_console->SetFont(consoleFont);
    
    // 设置背景色为深色
    m_console->SetBackgroundColour(wxColour(30, 30, 30));
    m_console->SetForegroundColour(wxColour(200, 200, 200));
    
    m_auiManager.AddPane(m_console, wxAuiPaneInfo()
        .Bottom()
        .Name("console")
        .Caption("Console")
        .MinSize(wxSize(-1, 150))
        .BestSize(wxSize(-1, 200)));
    
    m_auiManager.Update();
}

void MainFrame::UpdateTitle()
{
    wxString title = "LaminaLab IDE v0.0.1-Alpha";
    if (!m_currentFile.IsEmpty())
    {
        title = wxFileName(m_currentFile).GetFullName() + " - " + title;
        if (m_isModified)
            title = "*" + title;
    }
    SetTitle(title);
}

void MainFrame::LoadSettings()
{
    wxConfig config("LaminaLabIDE");
    
    // 加载窗口位置和大小
    int x = config.Read("WindowX", -1);
    int y = config.Read("WindowY", -1);
    int width = config.Read("WindowWidth", 800);
    int height = config.Read("WindowHeight", 600);
    
    if (x != -1 && y != -1)
        SetPosition(wxPoint(x, y));
    SetSize(width, height);
    
    // 加载解释器路径
    m_interpreterPath = config.Read("InterpreterPath", "./laminalab %lmfilepath%");
}

void MainFrame::SaveSettings()
{
    wxConfig config("LaminaLabIDE");
    
    // 保存窗口位置和大小
    wxPoint pos = GetPosition();
    wxSize size = GetSize();
    config.Write("WindowX", pos.x);
    config.Write("WindowY", pos.y);
    config.Write("WindowWidth", size.x);
    config.Write("WindowHeight", size.y);
    
    // 保存解释器路径
    config.Write("InterpreterPath", m_interpreterPath);
    config.Flush();
}

// 事件处理函数
void MainFrame::OnNew(wxCommandEvent& event)
{
    if (CheckSaveChanges())
    {
        m_editor->ClearAll();
        m_currentFile.Clear();
        m_isModified = false;
        UpdateTitle();
    }
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
    if (!CheckSaveChanges())
        return;
    
    wxFileDialog dialog(this, "Open Lamina file", "", "",
                       "Lamina files (*.lm)|*.lm|All files (*.*)|*.*",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename = dialog.GetPath();
        if (m_editor->LoadFile(filename))
        {
            m_currentFile = filename;
            m_isModified = false;
            UpdateTitle();
        }
        else
        {
            wxMessageBox("Failed to open file", "Error", wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    if (m_currentFile.IsEmpty())
    {
        OnSaveAs(event);
    }
    else
    {
        if (m_editor->SaveFile(m_currentFile))
        {
            m_isModified = false;
            UpdateTitle();
            SetStatusText("File saved", 0);
        }
        else
        {
            wxMessageBox("Failed to save file", "Error", wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dialog(this, "Save Lamina file", "", "",
                       "Lamina files (*.lm)|*.lm|All files (*.*)|*.*",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename = dialog.GetPath();
        if (m_editor->SaveFile(filename))
        {
            m_currentFile = filename;
            m_isModified = false;
            UpdateTitle();
            SetStatusText("File saved", 0);
        }
        else
        {
            wxMessageBox("Failed to save file", "Error", wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnUndo(wxCommandEvent& event)
{
    if (m_editor)
        m_editor->Undo();
}

void MainFrame::OnRedo(wxCommandEvent& event)
{
    if (m_editor)
        m_editor->Redo();
}

void MainFrame::OnCut(wxCommandEvent& event)
{
    if (m_editor)
        m_editor->Cut();
}

void MainFrame::OnCopy(wxCommandEvent& event)
{
    if (m_editor)
        m_editor->Copy();
}

void MainFrame::OnPaste(wxCommandEvent& event)
{
    if (m_editor)
        m_editor->Paste();
}

void MainFrame::OnFind(wxCommandEvent& event)
{
    if (m_editor)
    {
        wxString findText = wxGetTextFromUser("Find:", "Find Text", "", this);
        if (!findText.IsEmpty())
        {
            int pos = m_editor->FindText(m_editor->GetCurrentPos(), m_editor->GetTextLength(), findText, 0);
            if (pos != -1)
            {
                m_editor->SetSelection(pos, pos + findText.Length());
                m_editor->EnsureCaretVisible();
            }
            else
            {
                wxMessageBox("Text not found", "Find", wxOK | wxICON_INFORMATION);
            }
        }
    }
}

void MainFrame::OnRun(wxCommandEvent& event)
{
    if (m_currentFile.IsEmpty())
    {
        wxMessageBox("No file is currently open", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    if (m_isModified)
    {
        OnSave(event);
    }
    
    if (!m_processManager)
    {
        m_processManager = new ProcessManager();
        
        // 设置输出回调
        m_processManager->SetOutputCallback([this](const wxString& output) {
            if (m_console) {
                m_console->SetDefaultStyle(wxTextAttr(wxColour(200, 200, 200)));
                m_console->AppendText(output);
            }
        });
        
        m_processManager->SetErrorCallback([this](const wxString& error) {
            if (m_console) {
                m_console->SetDefaultStyle(wxTextAttr(wxColour(255, 100, 100)));
                m_console->AppendText("ERROR: " + error);
            }
        });
        
        m_processManager->SetFinishedCallback([this](int exitCode) {
            if (m_console) {
                m_console->SetDefaultStyle(wxTextAttr(wxColour(100, 255, 100)));
                m_console->AppendText(wxString::Format("\n--- Process finished with exit code %d ---\n", exitCode));
            }
            SetStatusText("Script finished", 0);
        });
    }
    
    // 清空控制台
    if (m_console) {
        m_console->Clear();
        m_console->SetDefaultStyle(wxTextAttr(wxColour(150, 150, 255)));
        m_console->AppendText(wxString::Format("Running: %s\n", m_currentFile));
        m_console->SetDefaultStyle(wxTextAttr(wxColour(200, 200, 200)));
    }
    
    // 替换占位符
    wxString command = m_interpreterPath;
    command.Replace("%lmfilepath%", m_currentFile);
    
    m_processManager->RunCommand(command);
    SetStatusText("Script is running...", 0);
}

void MainFrame::OnStop(wxCommandEvent& event)
{
    if (m_processManager)
    {
        m_processManager->StopProcess();
        SetStatusText("Script stopped", 0);
    }
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    wxDialog dialog(this, wxID_ANY, "Interpreter Configuration", wxDefaultPosition, wxSize(400, 200));
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // 说明文本
    wxStaticText* label = new wxStaticText(&dialog, wxID_ANY, 
        "Configure the path to the LaminaLab interpreter.\nUse %lmfilepath% as placeholder for the current file path.");
    mainSizer->Add(label, 0, wxALL | wxEXPAND, 10);
    
    // 路径输入
    wxTextCtrl* pathCtrl = new wxTextCtrl(&dialog, wxID_ANY, m_interpreterPath, 
                                         wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    mainSizer->Add(pathCtrl, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    
    // 按钮
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okBtn = new wxButton(&dialog, wxID_OK, "OK");
    wxButton* cancelBtn = new wxButton(&dialog, wxID_CANCEL, "Cancel");
    buttonSizer->Add(okBtn, 0, wxRIGHT, 5);
    buttonSizer->Add(cancelBtn, 0, 0, 0);
    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_RIGHT, 10);
    
    dialog.SetSizer(mainSizer);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        m_interpreterPath = pathCtrl->GetValue();
        SaveSettings();
    }
}

void MainFrame::OnTheme(wxCommandEvent& event)
{
    if (m_editor)
    {
        int menuId = event.GetId();
        int themeIndex = menuId - ID_THEME_START;
        
        if (themeIndex >= 0 && themeIndex < ThemeConfig::Get().GetAvailableThemes().GetCount())
        {
            wxString themeName = ThemeConfig::Get().GetAvailableThemes()[themeIndex];
            m_editor->ApplyTheme(themeName);
        }
    }
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("LaminaLab IDE v0.0.1-Alpha\n\nA simple IDE for the Lamina programming language.",
                 "About LaminaLab IDE", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if (CheckSaveChanges())
    {
        SaveSettings();
        event.Skip();
    }
    else
    {
        event.Veto();
    }
}

void MainFrame::OnTextChange(wxStyledTextEvent& event)
{
    if (!m_isModified)
    {
        m_isModified = true;
        UpdateTitle();
    }
}

void MainFrame::OnUpdateUI(wxStyledTextEvent& event)
{
    if (m_editor)
    {
        int line = m_editor->GetCurrentLine() + 1;
        int col = m_editor->GetColumn(m_editor->GetCurrentPos()) + 1;
        SetStatusText(wxString::Format("Line %d, Column %d", line, col), 1);
    }
}

bool MainFrame::CheckSaveChanges()
{
    if (m_isModified)
    {
        int result = wxMessageBox("Save changes?", "Confirm", 
                                 wxYES_NO | wxCANCEL | wxICON_QUESTION);
        
        if (result == wxYES)
        {
            wxCommandEvent saveEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_SAVE);
            OnSave(saveEvent);
            return !m_isModified; // 只有在成功保存后才返回true
        }
        else if (result == wxCANCEL)
        {
            return false;
        }
    }
    return true;
}
