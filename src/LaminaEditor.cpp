#include "LaminaEditor.h"
#include "ThemeConfig.h"
#include <wx/file.h>

wxBEGIN_EVENT_TABLE(LaminaEditor, wxStyledTextCtrl)
    EVT_STC_CHANGE(wxID_ANY, LaminaEditor::OnTextChanged)
    EVT_STC_MARGINCLICK(wxID_ANY, LaminaEditor::OnMarginClick)
wxEND_EVENT_TABLE()

LaminaEditor::LaminaEditor(wxWindow* parent, wxWindowID id)
    : wxStyledTextCtrl(parent, id)
{
    // 基本编辑器设置
    SetTechnology(wxSTC_TECHNOLOGY_DEFAULT); // 使用默认渲染技术
    SetBufferedDraw(true); // 启用缓冲绘制
    SetCodePage(wxSTC_CP_UTF8); // 使用 UTF-8 编码
    
    // 设置主题和语法
    SetupEditorPreferences();
    SetupLaminaSyntax();
    
    // 刷新显示
    Refresh();
}

LaminaEditor::~LaminaEditor()
{
}

bool LaminaEditor::LoadFile(const wxString& filename)
{
    wxFile file(filename, wxFile::read);
    if (!file.IsOpened())
        return false;
    
    wxString content;
    if (!file.ReadAll(&content))
        return false;
    
    SetText(content);
    m_currentFile = filename;
    EmptyUndoBuffer();
    SetSavePoint();
    
    return true;
}

bool LaminaEditor::SaveFile(const wxString& filename)
{
    wxFile file(filename, wxFile::write);
    if (!file.IsOpened())
        return false;
    
    wxString content = GetText();
    if (!file.Write(content))
        return false;
    
    m_currentFile = filename;
    SetSavePoint();
    
    return true;
}

bool LaminaEditor::IsModified() const
{
    return GetModify();
}

void LaminaEditor::SetupEditorPreferences()
{
    // 基本设置
    SetWrapMode(wxSTC_WRAP_NONE);
    SetTabWidth(4);
    SetIndent(4);
    SetUseTabs(false);
    SetTabIndents(true);
    SetBackSpaceUnIndents(true);
    SetViewEOL(false);
    SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
    
    // 选择设置
    SetSelectionMode(wxSTC_SEL_STREAM);
    SetMultipleSelection(false);
    
    // 当前行高亮
    SetCaretLineVisible(true);
    SetCaretLineBackground(wxColour(240, 240, 240));
    
    // 括号匹配
    SetProperty("fold.compact", "1");
    SetProperty("fold.comment", "1");
    
    SetEditorStyles();
    SetMargins();
    SetFolding();
}

void LaminaEditor::SetupLaminaSyntax()
{
    // 使用 C++ lexer 作为基础
    SetLexer(wxSTC_LEX_CPP);
    
    // 设置分隔符
    SetWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_πe√");
    
    // 设置词法分析器属性
    SetProperty("lexer.cpp.allow.dollars", "0");
    SetProperty("lexer.cpp.hashquoted.strings", "0");
    SetProperty("lexer.cpp.track.preprocessor", "0");
    
    SetLexerKeywords();
    ApplyTheme();
}

void LaminaEditor::ApplyTheme(const wxString& themeName)
{
    if (!themeName.IsEmpty())
    {
        ThemeConfig::Get().LoadTheme(themeName);
    }
    SetLexerColors();
    SetEditorStyles();
}

void LaminaEditor::SetLexerKeywords()
{
    // Lamina 关键字
    wxString keywords0 = "if else while for return break continue "
                        "var func print input assert include "
                        "true false null";
    
    // 数据类型
    wxString keywords1 = "int float rational irrational bool string";
    
    // 内置常量和函数
    wxString keywords2 = "π e dot cross";
    
    SetKeyWords(0, keywords0);
    SetKeyWords(1, keywords1);
    SetKeyWords(2, keywords2);
}

void LaminaEditor::SetLexerColors()
{
    ThemeConfig& theme = ThemeConfig::Get();
    // 应用默认样式
    StyleClearAll();
    // 首先设置默认风格
    StyleSetBackground(wxSTC_STYLE_DEFAULT, theme.GetColor("default", "background"));
    StyleSetForeground(wxSTC_STYLE_DEFAULT, theme.GetColor("default"));
    SetWhitespaceBackground(true, theme.GetColor("default", "background"));
    SetWhitespaceForeground(true, theme.GetColor("default"));
    
    // 设置选择区域的颜色
    SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    
    // 注释
    StyleSetForeground(wxSTC_C_COMMENT, theme.GetColor("comments"));
    StyleSetForeground(wxSTC_C_COMMENTLINE, theme.GetColor("comments"));
    StyleSetItalic(wxSTC_C_COMMENT, theme.IsItalic("comments"));
    StyleSetItalic(wxSTC_C_COMMENTLINE, theme.IsItalic("comments"));
    
    // 控制关键字
    StyleSetForeground(wxSTC_C_WORD, theme.GetColor("keywords"));
    StyleSetBold(wxSTC_C_WORD, theme.IsBold("keywords", "control"));
    
    // 数据类型
    StyleSetForeground(wxSTC_C_WORD2, theme.GetColor("keywords", "types"));
    StyleSetBold(wxSTC_C_WORD2, theme.IsBold("keywords", "types"));
    
    // 字符串
    StyleSetForeground(wxSTC_C_STRING, theme.GetColor("strings"));
    
    // 数字
    StyleSetForeground(wxSTC_C_NUMBER, theme.GetColor("numbers"));
    
    // 运算符
    StyleSetForeground(wxSTC_C_OPERATOR, theme.GetColor("operators"));
    StyleSetBold(wxSTC_C_OPERATOR, theme.IsBold("operators"));
    
    // 特殊常量
    StyleSetForeground(wxSTC_C_GLOBALCLASS, theme.GetColor("constants"));
    StyleSetBold(wxSTC_C_GLOBALCLASS, theme.IsBold("constants"));
    
    // 函数名
    StyleSetForeground(wxSTC_C_IDENTIFIER, theme.GetColor("functions"));
    
    // 标识符
    StyleSetForeground(wxSTC_C_IDENTIFIER, theme.GetColor("identifiers"));
}

void LaminaEditor::SetEditorStyles()
{
    ThemeConfig& theme = ThemeConfig::Get();
    
    // 行号样式
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, theme.GetColor("linenumber"));
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, theme.GetColor("linenumber", "background"));
    
    // 折叠标记样式
    StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, theme.GetColor("linenumber"));
    
    // 当前行高亮
    SetCaretLineBackground(theme.GetColor("currentLine", "background"));
    
    // 大括号匹配
    StyleSetForeground(wxSTC_STYLE_BRACELIGHT, theme.GetColor("braceMatch"));
    StyleSetBackground(wxSTC_STYLE_BRACELIGHT, theme.GetColor("braceMatch", "background"));
    StyleSetBold(wxSTC_STYLE_BRACELIGHT, theme.IsBold("braceMatch"));
    
    StyleSetForeground(wxSTC_STYLE_BRACEBAD, theme.GetColor("braceMismatch"));
    StyleSetBackground(wxSTC_STYLE_BRACEBAD, theme.GetColor("braceMismatch", "background"));
    StyleSetBold(wxSTC_STYLE_BRACEBAD, theme.IsBold("braceMismatch"));
}

void LaminaEditor::SetMargins()
{
    // 行号边距
    SetMarginType(0, wxSTC_MARGIN_NUMBER);
    SetMarginWidth(0, 50);
    SetMarginSensitive(0, false);
    
    // 折叠边距
    SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    SetMarginMask(1, wxSTC_MASK_FOLDERS);
    SetMarginWidth(1, 20);
    SetMarginSensitive(1, true);
    
    // 设置边距之间的分割线
    SetMarginLeft(5);
    SetMarginRight(5);
}

void LaminaEditor::SetFolding()
{
    SetProperty("fold", "1");
    SetProperty("fold.compact", "1");
    SetProperty("fold.comment", "1");
    SetProperty("fold.preprocessor", "1");
    
    // 设置折叠标记
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, wxColour(255, 255, 255), wxColour(0, 0, 0));
    MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, wxColour(255, 255, 255), wxColour(0, 0, 0));
    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, wxColour(255, 255, 255), wxColour(0, 0, 0));
    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, wxColour(255, 255, 255), wxColour(0, 0, 0));
    MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, wxColour(255, 255, 255), wxColour(0, 0, 0));
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, wxColour(255, 255, 255), wxColour(0, 0, 0));
    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, wxColour(255, 255, 255), wxColour(0, 0, 0));
}

void LaminaEditor::OnTextChanged(wxStyledTextEvent& event)
{
    if (m_changeCallback)
        m_changeCallback();
    
    event.Skip();
}

void LaminaEditor::OnMarginClick(wxStyledTextEvent& event)
{
    if (event.GetMargin() == 1)
    {
        int lineClick = LineFromPosition(event.GetPosition());
        int levelClick = GetFoldLevel(lineClick);
        
        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
        {
            ToggleFold(lineClick);
        }
    }
    
    event.Skip();
}
