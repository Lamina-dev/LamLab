#ifndef PROJECT_EXPLORER_WINDOW_H
#define PROJECT_EXPLORER_WINDOW_H

#include "LaminaIDE.h"
#include <string>
#include <vector>
#include <functional>
#include <filesystem>

// 文件树节点
struct FileTreeNode {
    std::string name;
    std::string full_path;
    bool is_directory;
    bool is_expanded;
    std::vector<std::unique_ptr<FileTreeNode>> children;
    FileTreeNode* parent;
    
    FileTreeNode(const std::string& name, const std::string& path, bool is_dir) 
        : name(name), full_path(path), is_directory(is_dir), is_expanded(false), parent(nullptr) {}
};

class ProjectExplorerWindow : public IDEWindow {
public:
    ProjectExplorerWindow();
    ~ProjectExplorerWindow() override = default;
    
    void Render() override;
    const char* GetTitle() const override { return "Project Explorer"; }
    WindowType GetType() const override { return WindowType::ProjectExplorer; }
    
    // 项目操作
    bool LoadProject(const std::string& project_path);
    void CloseProject();
    void RefreshProject();
    
    // 文件操作
    void CreateNewFile();
    void CreateNewFolder();
    void DeleteSelected();
    void RenameSelected();
    
    // 回调设置
    void SetFileOpenCallback(std::function<void(const std::string&)> callback) {
        file_open_callback_ = callback;
    }
    
    void SetFileCreateCallback(std::function<void(const std::string&)> callback) {
        file_create_callback_ = callback;
    }
    
private:
    std::unique_ptr<FileTreeNode> root_node_;
    std::string project_path_;
    std::string selected_path_;
    bool project_loaded_;
    
    // 对话框状态
    bool show_new_file_dialog_;
    bool show_new_folder_dialog_;
    bool show_rename_dialog_;
    bool show_delete_confirm_;
    char new_name_buffer_[256];
    
    // 过滤器
    bool show_hidden_files_;
    std::string file_filter_;
    
    // 回调函数
    std::function<void(const std::string&)> file_open_callback_;
    std::function<void(const std::string&)> file_create_callback_;
    
    // 私有方法
    void BuildFileTree(const std::string& path);
    void RenderFileTree(FileTreeNode* node);
    void RenderContextMenu();
    void RenderNewFileDialog();
    void RenderNewFolderDialog();
    void RenderRenameDialog();
    void RenderDeleteConfirmDialog();
    
    // 文件操作实现
    bool CreateFile(const std::string& path);
    bool CreateFolder(const std::string& path);
    bool DeletePath(const std::string& path);
    bool RenamePath(const std::string& old_path, const std::string& new_path);
    
    // 工具方法
    void AddFileTreeNode(FileTreeNode* parent, const std::filesystem::directory_entry& entry);
    bool ShouldShowFile(const std::string& filename);
    const char* GetFileIcon(const std::string& extension);
    void SortNodes(std::vector<std::unique_ptr<FileTreeNode>>& nodes);
};

#endif // PROJECT_EXPLORER_WINDOW_H
