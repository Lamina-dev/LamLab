#pragma once

#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <functional>

class LaminaEditor : public wxStyledTextCtrl
{
public:
    LaminaEditor(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~LaminaEditor();
    
    // 文件操作
    bool LoadFile(const wxString& filename);
    bool SaveFile(const wxString& filename);
    
    // 编辑器设置
    void SetupLaminaSyntax();
    void SetupEditorPreferences();
    
    // 状态查询
    bool IsModified() const;
    wxString GetCurrentFile() const { return m_currentFile; }
    
    // 设置变化回调
    void SetChangeCallback(std::function<void()> callback) { m_changeCallback = callback; }
    
    // 主题配置
    void ApplyTheme(const wxString& themeName = wxEmptyString);
    
private:
    // 事件处理
    void OnTextChanged(wxStyledTextEvent& event);
    void OnMarginClick(wxStyledTextEvent& event);
    
    // 语法高亮设置
    void SetLexerColors();
    void SetLexerKeywords();
    
    // 编辑器配置
    void SetEditorStyles();
    void SetMargins();
    void SetFolding();
    
private:
    wxString m_currentFile;
    std::function<void()> m_changeCallback;
    
    wxDECLARE_EVENT_TABLE();
};
