#include "TextEditorWindow.h"
#include "imgui.h"
#include "nfd.h"
#include <fstream>
#include <sstream>
#include <algorithm>

TextEditorWindow::TextEditorWindow()
    : active_tab_index_(-1)
    , show_find_dialog_(false)
    , show_replace_dialog_(false)
    , show_goto_dialog_(false)
    , goto_line_(1)
    , show_line_numbers_(true)
    , word_wrap_(true)
    , syntax_highlighting_(true)
    , font_size_(14.0f)
{
    memset(find_text_, 0, sizeof(find_text_));
    memset(replace_text_, 0, sizeof(replace_text_));
    
    // 创建一个默认的新文档
    NewDocument();
}

void TextEditorWindow::Render()
{
    if (!IsVisible()) return;
    
    if (ImGui::Begin("Text Editor", &visible_)) {
        // 渲染标签栏
        RenderTabBar();
        
        // 渲染编辑器
        RenderEditor();
    }
    ImGui::End();
    
    // 渲染对话框
    RenderFindDialog();
    RenderReplaceDialog();
    RenderGoToDialog();
}

void TextEditorWindow::RenderTabBar()
{
    if (ImGui::BeginTabBar("EditorTabs")) {
        for (int i = 0; i < tabs_.size(); ++i) {
            auto& tab = tabs_[i];
            
            ImGuiTabItemFlags flags = 0;
            if (tab.is_modified) {
                flags |= ImGuiTabItemFlags_UnsavedDocument;
            }
            
            bool tab_open = true;
            if (ImGui::BeginTabItem(GetDocumentTitle(tab).c_str(), &tab_open, flags)) {
                active_tab_index_ = i;
                tab.is_active = true;
                ImGui::EndTabItem();
            } else {
                tab.is_active = false;
            }
            
            if (!tab_open) {
                CloseDocument(i);
                break; // 避免迭代器失效
            }
        }
        
        // 新建标签按钮
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing)) {
            NewDocument();
        }
        
        ImGui::EndTabBar();
    }
}

void TextEditorWindow::RenderEditor()
{
    if (active_tab_index_ < 0 || active_tab_index_ >= tabs_.size()) {
        ImGui::Text("No document open");
        return;
    }
    
    auto& active_tab = tabs_[active_tab_index_];
    
    // 工具栏
    if (ImGui::Button("Save")) {
        SaveDocument(active_tab_index_);
    }
    ImGui::SameLine();
    if (ImGui::Button("Find")) {
        show_find_dialog_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Replace")) {
        show_replace_dialog_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Go to Line")) {
        show_goto_dialog_ = true;
    }
    
    ImGui::Separator();
    
    // 状态信息
    int line_count = CountLines(active_tab.content);
    ImGui::Text("Lines: %d | Cursor: %d:%d | %s", 
               line_count, 
               active_tab.cursor_line + 1, 
               active_tab.cursor_column + 1,
               active_tab.is_modified ? "Modified" : "Saved");
    
    // 编辑器区域
    ImVec2 editor_size = ImGui::GetContentRegionAvail();
    
    // 如果启用了行号，为行号留出空间
    float line_number_width = 0.0f;
    if (show_line_numbers_) {
        line_number_width = 50.0f; // 固定宽度
        editor_size.x -= line_number_width;
    }
    
    // 行号显示
    if (show_line_numbers_) {
        ImGui::BeginChild("LineNumbers", ImVec2(line_number_width, editor_size.y), true);
        RenderLineNumbers(active_tab.content);
        ImGui::EndChild();
        ImGui::SameLine();
    }
    
    // 主编辑区
    ImGui::BeginChild("Editor", editor_size, true);
    
    ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_AllowTabInput;
    if (!word_wrap_) {
        input_flags |= ImGuiInputTextFlags_NoHorizontalScroll;
    }
    
    std::string prev_content = active_tab.content;
    
    // 为 ImGui::InputTextMultiline 创建临时缓冲区
    static char text_buffer[1024 * 1024]; // 1MB 缓冲区
    strncpy_s(text_buffer, active_tab.content.c_str(), sizeof(text_buffer) - 1);
    text_buffer[sizeof(text_buffer) - 1] = '\0';
    
    if (ImGui::InputTextMultiline("##editor", 
                                 text_buffer, 
                                 sizeof(text_buffer),
                                 ImGui::GetContentRegionAvail(),
                                 input_flags)) {
        active_tab.content = text_buffer;
        if (prev_content != active_tab.content) {
            active_tab.is_modified = true;
            UpdateTabTitle(active_tab);
            
            // 更新语法高亮
            if (syntax_highlighting_) {
                UpdateSyntaxHighlighting(active_tab);
            }
        }
    }
    
    ImGui::EndChild();
}

void TextEditorWindow::RenderLineNumbers(const std::string& content)
{
    int line_count = CountLines(content);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    // 背景颜色
    ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
    draw_list->AddRectFilled(canvas_pos, 
                           ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), 
                           bg_color);
    
    // 行号文本
    ImU32 text_color = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    float line_height = ImGui::GetTextLineHeight();
    
    for (int i = 1; i <= line_count; ++i) {
        std::string line_num = std::to_string(i);
        ImVec2 text_pos = ImVec2(canvas_pos.x + 5, canvas_pos.y + (i - 1) * line_height);
        draw_list->AddText(text_pos, text_color, line_num.c_str());
    }
}

void TextEditorWindow::RenderFindDialog()
{
    if (show_find_dialog_) {
        ImGui::OpenPopup("Find");
        show_find_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("Find", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Find what:");
        ImGui::InputText("##FindText", find_text_, sizeof(find_text_));
        
        ImGui::Spacing();
        
        if (ImGui::Button("Find Next")) {
            if (active_tab_index_ >= 0 && strlen(find_text_) > 0) {
                FindInText(tabs_[active_tab_index_].content, find_text_);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void TextEditorWindow::RenderReplaceDialog()
{
    if (show_replace_dialog_) {
        ImGui::OpenPopup("Replace");
        show_replace_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("Replace", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Find what:");
        ImGui::InputText("##FindText2", find_text_, sizeof(find_text_));
        ImGui::Text("Replace with:");
        ImGui::InputText("##ReplaceText", replace_text_, sizeof(replace_text_));
        
        ImGui::Spacing();
        
        if (ImGui::Button("Replace")) {
            if (active_tab_index_ >= 0 && strlen(find_text_) > 0) {
                ReplaceInText(tabs_[active_tab_index_].content, find_text_, replace_text_);
                tabs_[active_tab_index_].is_modified = true;
                UpdateTabTitle(tabs_[active_tab_index_]);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Replace All")) {
            if (active_tab_index_ >= 0 && strlen(find_text_) > 0) {
                std::string& content = tabs_[active_tab_index_].content;
                size_t pos = 0;
                int count = 0;
                while ((pos = content.find(find_text_, pos)) != std::string::npos) {
                    content.replace(pos, strlen(find_text_), replace_text_);
                    pos += strlen(replace_text_);
                    count++;
                }
                if (count > 0) {
                    tabs_[active_tab_index_].is_modified = true;
                    UpdateTabTitle(tabs_[active_tab_index_]);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void TextEditorWindow::RenderGoToDialog()
{
    if (show_goto_dialog_) {
        ImGui::OpenPopup("Go to Line");
        show_goto_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("Go to Line", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Line number:");
        ImGui::InputInt("##LineNumber", &goto_line_);
        
        ImGui::Spacing();
        
        if (ImGui::Button("Go")) {
            // TODO: 实现跳转到指定行
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void TextEditorWindow::NewDocument()
{
    DocumentTab new_tab;
    new_tab.title = "Untitled";
    new_tab.content = "";
    new_tab.is_modified = false;
    new_tab.is_active = false;
    
    tabs_.push_back(new_tab);
    active_tab_index_ = tabs_.size() - 1;
}

bool TextEditorWindow::OpenDocument(const std::string& file_path)
{
    // 检查文件是否已经打开
    for (int i = 0; i < tabs_.size(); ++i) {
        if (tabs_[i].file_path == file_path) {
            active_tab_index_ = i;
            return true;
        }
    }
    
    std::string content = LoadFileContent(file_path);
    if (content.empty() && !file_path.empty()) {
        return false; // 文件加载失败
    }
    
    DocumentTab new_tab;
    new_tab.file_path = file_path;
    new_tab.content = content;
    new_tab.is_modified = false;
    new_tab.is_active = false;
    UpdateTabTitle(new_tab);
    
    // 应用语法高亮
    if (syntax_highlighting_) {
        UpdateSyntaxHighlighting(new_tab);
    }
    
    tabs_.push_back(new_tab);
    active_tab_index_ = tabs_.size() - 1;
    
    return true;
}

bool TextEditorWindow::SaveDocument(int tab_index)
{
    if (tab_index < 0) {
        tab_index = active_tab_index_;
    }
    
    if (tab_index < 0 || tab_index >= tabs_.size()) {
        return false;
    }
    
    auto& tab = tabs_[tab_index];
    
    if (tab.file_path.empty()) {
        return SaveDocumentAs(tab_index);
    }
    
    if (SaveFileContent(tab.file_path, tab.content)) {
        tab.is_modified = false;
        UpdateTabTitle(tab);
        return true;
    }
    
    return false;
}

bool TextEditorWindow::SaveDocumentAs(int tab_index)
{
    if (tab_index < 0) {
        tab_index = active_tab_index_;
    }
    
    if (tab_index < 0 || tab_index >= tabs_.size()) {
        return false;
    }
    
    nfdchar_t* outPath = nullptr;
    nfdfilteritem_t filterItem[1] = { { "Lamina Files", "lm,txt" } };
    
    nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, nullptr, "untitled.lm");
    
    if (result == NFD_OKAY) {
        auto& tab = tabs_[tab_index];
        tab.file_path = outPath;
        
        if (SaveFileContent(tab.file_path, tab.content)) {
            tab.is_modified = false;
            UpdateTabTitle(tab);
            free(outPath);
            return true;
        }
        
        free(outPath);
    }
    
    return false;
}

void TextEditorWindow::CloseDocument(int tab_index)
{
    if (tab_index < 0 || tab_index >= tabs_.size()) {
        return;
    }
    
    // TODO: 检查是否需要保存修改
    
    tabs_.erase(tabs_.begin() + tab_index);
    
    if (active_tab_index_ >= tabs_.size()) {
        active_tab_index_ = tabs_.size() - 1;
    }
    
    if (tabs_.empty()) {
        NewDocument();
    }
}

void TextEditorWindow::CloseAllDocuments()
{
    // TODO: 检查是否需要保存修改
    tabs_.clear();
    active_tab_index_ = -1;
    NewDocument();
}

DocumentTab* TextEditorWindow::GetActiveDocument()
{
    if (active_tab_index_ >= 0 && active_tab_index_ < tabs_.size()) {
        return &tabs_[active_tab_index_];
    }
    return nullptr;
}

void TextEditorWindow::Undo()
{
    // TODO: 实现撤销功能
}

void TextEditorWindow::Redo()
{
    // TODO: 实现重做功能
}

void TextEditorWindow::Cut()
{
    // TODO: 实现剪切功能
}

void TextEditorWindow::Copy()
{
    // TODO: 实现复制功能
}

void TextEditorWindow::Paste()
{
    // TODO: 实现粘贴功能
}

void TextEditorWindow::SelectAll()
{
    // TODO: 实现全选功能
}

void TextEditorWindow::Find()
{
    show_find_dialog_ = true;
}

void TextEditorWindow::Replace()
{
    show_replace_dialog_ = true;
}

void TextEditorWindow::GoToLine()
{
    show_goto_dialog_ = true;
}

void TextEditorWindow::UpdateSyntaxHighlighting(DocumentTab& tab)
{
    // 检查文件扩展名来决定语法高亮类型
    if (tab.file_path.ends_with(".lm")) {
        ApplyLaminaSyntax(tab);
    }
    // TODO: 添加其他语言的语法高亮
}

void TextEditorWindow::ApplyLaminaSyntax(DocumentTab& tab)
{
    // TODO: 实现 Lamina 语言的语法高亮
    // 这里需要解析代码并生成颜色范围
    tab.syntax.ranges.clear();
    
    // 示例：高亮关键字
    std::vector<std::string> keywords = {
        "func", "var", "let", "const", "if", "else", "while", "for", 
        "return", "import", "export", "class", "struct", "enum"
    };
    
    for (const auto& keyword : keywords) {
        size_t pos = 0;
        while ((pos = tab.content.find(keyword, pos)) != std::string::npos) {
            SyntaxHighlight::ColorRange range;
            range.start = pos;
            range.length = keyword.length();
            range.color = IM_COL32(86, 156, 214, 255); // 蓝色
            tab.syntax.ranges.push_back(range);
            pos += keyword.length();
        }
    }
}

std::string TextEditorWindow::LoadFileContent(const std::string& file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

bool TextEditorWindow::SaveFileContent(const std::string& file_path, const std::string& content)
{
    std::ofstream file(file_path, std::ios::binary);
    if (!file) {
        return false;
    }
    
    file << content;
    return file.good();
}

std::string TextEditorWindow::GetDocumentTitle(const DocumentTab& tab)
{
    std::string title = tab.title;
    if (title.empty()) {
        if (tab.file_path.empty()) {
            title = "Untitled";
        } else {
            size_t pos = tab.file_path.find_last_of("/\\");
            title = (pos != std::string::npos) ? tab.file_path.substr(pos + 1) : tab.file_path;
        }
    }
    
    if (tab.is_modified) {
        title += "*";
    }
    
    return title;
}

void TextEditorWindow::UpdateTabTitle(DocumentTab& tab)
{
    if (!tab.file_path.empty()) {
        size_t pos = tab.file_path.find_last_of("/\\");
        tab.title = (pos != std::string::npos) ? tab.file_path.substr(pos + 1) : tab.file_path;
    } else {
        tab.title = "Untitled";
    }
}

void TextEditorWindow::FindInText(const std::string& text, const std::string& search)
{
    size_t pos = text.find(search);
    if (pos != std::string::npos) {
        // TODO: 高亮找到的文本并滚动到该位置
    }
}

void TextEditorWindow::ReplaceInText(std::string& text, const std::string& search, const std::string& replace)
{
    size_t pos = text.find(search);
    if (pos != std::string::npos) {
        text.replace(pos, search.length(), replace);
    }
}

int TextEditorWindow::CountLines(const std::string& text)
{
    return std::count(text.begin(), text.end(), '\n') + 1;
}
