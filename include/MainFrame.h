#pragma once

#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/aui/aui.h>
#include <memory>

class LaminaEditor;
class ProcessManager;

// Menu IDs
enum {
    ID_SAVE_AS = wxID_HIGHEST + 1,
    ID_RUN,
    ID_STOP,
    ID_SETTINGS,
    ID_EDITOR,
    ID_CONSOLE,
    ID_THEME_START,
    ID_THEME_END = ID_THEME_START + 100 // 支持最多100个主题
};

class MainFrame : public wxFrame
{
public:
    MainFrame();
    virtual ~MainFrame();

private:
    // 事件处理
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    
    void OnRun(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnTheme(wxCommandEvent& event);
    
    void OnAbout(wxCommandEvent& event);
    
    void OnClose(wxCloseEvent& event);
    
    // UI 创建
    void CreateMenuBar();
    void CreateStatusBar();
    void CreateToolBar();
    void InitializeAUI();
    void CreateEditor();
    void CreateConsole();
    
    void CreateThemeMenu(wxMenu* viewMenu);
    
    // 文本编辑器事件
    void OnTextChange(wxStyledTextEvent& event);
    void OnUpdateUI(wxStyledTextEvent& event);
    
    // 实用函数
    bool CheckSaveChanges();
    
    // 配置管理
    void LoadSettings();
    void SaveSettings();
    
    // 更新界面
    void UpdateTitle();
    
private:
    // UI 组件
    wxAuiManager m_auiManager;
    LaminaEditor* m_editor;
    wxTextCtrl* m_console;
    
    // 文件信息
    wxString m_currentFile;
    bool m_isModified;
    
    // 解释器配置
    wxString m_interpreterPath;
    
    // 进程管理
    ProcessManager* m_processManager;
    
    // 事件ID
    enum
    {
        ID_SAVE_AS = wxID_HIGHEST + 1,
        ID_RUN,
        ID_STOP,
        ID_SETTINGS,
        ID_EDITOR
    };
    
    wxDECLARE_EVENT_TABLE();
};
