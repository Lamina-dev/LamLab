#ifndef TEXT_EDITOR_WINDOW_H
#define TEXT_EDITOR_WINDOW_H

#include "LaminaIDE.h"
#include <string>
#include <vector>
#include <unordered_map>

// 语法高亮信息
struct SyntaxHighlight {
    struct ColorRange {
        int start;
        int length;
        unsigned int color;
    };
    
    std::vector<ColorRange> ranges;
};

// 文档标签页
struct DocumentTab {
    std::string file_path;
    std::string content;
    std::string title;
    bool is_modified;
    bool is_active;
    SyntaxHighlight syntax;
    int cursor_line;
    int cursor_column;
    
    DocumentTab() : is_modified(false), is_active(false), cursor_line(0), cursor_column(0) {}
};

class TextEditorWindow : public IDEWindow {
public:
    TextEditorWindow();
    ~TextEditorWindow() override = default;
    
    void Render() override;
    const char* GetTitle() const override { return "Text Editor"; }
    WindowType GetType() const override { return WindowType::TextEditor; }
    
    // 文件操作
    void NewDocument();
    bool OpenDocument(const std::string& file_path);
    bool SaveDocument(int tab_index = -1);
    bool SaveDocumentAs(int tab_index = -1);
    void CloseDocument(int tab_index);
    void CloseAllDocuments();
    
    // 编辑操作
    void Undo();
    void Redo();
    void Cut();
    void Copy();
    void Paste();
    void SelectAll();
    void Find();
    void Replace();
    void GoToLine();
    
    // 获取当前文档
    DocumentTab* GetActiveDocument();
    int GetActiveTabIndex() const { return active_tab_index_; }
    
private:
    std::vector<DocumentTab> tabs_;
    int active_tab_index_;
    
    // 查找替换状态
    bool show_find_dialog_;
    bool show_replace_dialog_;
    bool show_goto_dialog_;
    char find_text_[256];
    char replace_text_[256];
    int goto_line_;
    
    // 编辑器设置
    bool show_line_numbers_;
    bool word_wrap_;
    bool syntax_highlighting_;
    float font_size_;
    
    // 私有方法
    void RenderTabBar();
    void RenderEditor();
    void RenderFindDialog();
    void RenderReplaceDialog();
    void RenderGoToDialog();
    void RenderLineNumbers(const std::string& content);
    
    // 语法高亮
    void UpdateSyntaxHighlighting(DocumentTab& tab);
    void ApplyLaminaSyntax(DocumentTab& tab);
    
    // 工具方法
    std::string LoadFileContent(const std::string& file_path);
    bool SaveFileContent(const std::string& file_path, const std::string& content);
    std::string GetDocumentTitle(const DocumentTab& tab);
    void UpdateTabTitle(DocumentTab& tab);
    
    // 编辑操作实现
    void FindInText(const std::string& text, const std::string& search);
    void ReplaceInText(std::string& text, const std::string& search, const std::string& replace);
    int CountLines(const std::string& text);
};

#endif // TEXT_EDITOR_WINDOW_H
