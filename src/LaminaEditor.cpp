#include "LaminaEditor.h"
#include <wx/file.h>

wxBEGIN_EVENT_TABLE(LaminaEditor, wxStyledTextCtrl)
    EVT_STC_CHANGE(wxID_ANY, LaminaEditor::OnTextChanged)
    EVT_STC_MARGINCLICK(wxID_ANY, LaminaEditor::OnMarginClick)
wxEND_EVENT_TABLE()

LaminaEditor::LaminaEditor(wxWindow* parent, wxWindowID id)
    : wxStyledTextCtrl(parent, id)
{
    SetupEditorPreferences();
    SetupLaminaSyntax();
}

LaminaEditor::~LaminaEditor() = default;

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

    if (wxString content = GetText(); !file.Write(content))
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
    // 使用 C++ lexer 作为基础，因为 Lamina 语法类似
    SetLexer(wxSTC_LEX_CPP);
    
    SetLexerKeywords();
    SetLexerColors();
}

void LaminaEditor::SetLexerKeywords()
{
    // Lamina 关键字 - 根据实际 Lamina 语言定义调整
    wxString keywords0 = "if else elif while for in break continue function return "
                        "true false null undefined let const var class struct "
                        "import export from as try catch finally throw "
                        "async await yield new delete this super extends "
                        "public private protected static abstract interface "
                        "enum type union namespace module package";
    
    // 内置类型
    wxString keywords1 = "int float double bool string char void "
                        "number object array map set list tuple "
                        "any unknown never";
    
    // 内置函数
    wxString keywords2 = "print println log error warn info debug "
                        "length size push pop shift unshift "
                        "indexOf contains startsWith endsWith "
                        "substr substring split join trim "
                        "toString toNumber toBoolean";
    
    SetKeyWords(0, keywords0);
    SetKeyWords(1, keywords1);
    SetKeyWords(2, keywords2);
}

void LaminaEditor::SetLexerColors()
{
    // 默认文本
    StyleSetForeground(wxSTC_C_DEFAULT, wxColour(0, 0, 0));
    StyleSetBackground(wxSTC_C_DEFAULT, wxColour(255, 255, 255));
    
    // 注释
    StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
    StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
    StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(128, 128, 128));
    
    // 关键字
    StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
    StyleSetBold(wxSTC_C_WORD, true);
    
    // 类型关键字
    StyleSetForeground(wxSTC_C_WORD2, wxColour(43, 145, 175));
    StyleSetBold(wxSTC_C_WORD2, true);
    
    // 字符串
    StyleSetForeground(wxSTC_C_STRING, wxColour(163, 21, 21));
    StyleSetForeground(wxSTC_C_CHARACTER, wxColour(163, 21, 21));
    StyleSetForeground(wxSTC_C_STRINGEOL, wxColour(163, 21, 21));
    
    // 数字
    StyleSetForeground(wxSTC_C_NUMBER, wxColour(255, 127, 0));
    
    // 操作符
    StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 0));
    StyleSetBold(wxSTC_C_OPERATOR, true);
    
    // 标识符
    StyleSetForeground(wxSTC_C_IDENTIFIER, wxColour(0, 0, 0));
    
    // 预处理器
    StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(128, 128, 128));
}

void LaminaEditor::SetEditorStyles()
{
    // 行号样式
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(75, 75, 75));
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(245, 245, 245));
    
    // 折叠标记样式
    StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(192, 192, 192));
    
    // 大括号匹配
    StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColour(0, 0, 255));
    StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColour(255, 255, 255));
    StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);
    
    StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(255, 0, 0));
    StyleSetBackground(wxSTC_STYLE_BRACEBAD, wxColour(255, 255, 255));
    StyleSetBold(wxSTC_STYLE_BRACEBAD, true);
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
