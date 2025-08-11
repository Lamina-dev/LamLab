#include "ProjectExplorerWindow.h"
#include "imgui.h"
#include <filesystem>
#include <algorithm>
#include <fstream>

ProjectExplorerWindow::ProjectExplorerWindow()
    : project_loaded_(false)
    , show_new_file_dialog_(false)
    , show_new_folder_dialog_(false)
    , show_rename_dialog_(false)
    , show_delete_confirm_(false)
    , show_hidden_files_(false)
{
    memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
}

void ProjectExplorerWindow::Render()
{
    if (!IsVisible()) return;
    
    if (ImGui::Begin("Project Explorer", &visible_)) {
        if (!project_loaded_) {
            ImGui::Text("No project loaded");
            if (ImGui::Button("Open Project")) {
                // 通过回调触发打开项目
            }
        } else {
            // 工具栏
            if (ImGui::Button("Refresh")) {
                RefreshProject();
            }
            ImGui::SameLine();
            if (ImGui::Button("New File")) {
                show_new_file_dialog_ = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("New Folder")) {
                show_new_folder_dialog_ = true;
            }
            
            ImGui::Separator();
            
            // 过滤器
            ImGui::Checkbox("Show hidden files", &show_hidden_files_);
            
            ImGui::Separator();
            
            // 文件树
            if (root_node_) {
                RenderFileTree(root_node_.get());
            }
            
            // 右键菜单
            RenderContextMenu();
        }
    }
    ImGui::End();
    
    // 对话框
    RenderNewFileDialog();
    RenderNewFolderDialog();
    RenderRenameDialog();
    RenderDeleteConfirmDialog();
}

bool ProjectExplorerWindow::LoadProject(const std::string& project_path)
{
    project_path_ = project_path;
    selected_path_.clear();
    
    try {
        BuildFileTree(project_path);
        project_loaded_ = true;
        return true;
    } catch (const std::exception& e) {
        project_loaded_ = false;
        return false;
    }
}

void ProjectExplorerWindow::CloseProject()
{
    project_path_.clear();
    selected_path_.clear();
    root_node_.reset();
    project_loaded_ = false;
}

void ProjectExplorerWindow::RefreshProject()
{
    if (!project_loaded_ || project_path_.empty()) {
        return;
    }
    
    BuildFileTree(project_path_);
}

void ProjectExplorerWindow::BuildFileTree(const std::string& path)
{
    namespace fs = std::filesystem;
    
    if (!fs::exists(path) || !fs::is_directory(path)) {
        return;
    }
    
    // 创建根节点
    root_node_ = std::make_unique<FileTreeNode>(
        fs::path(path).filename().string(), 
        path, 
        true
    );
    root_node_->is_expanded = true;
    
    // 递归构建文件树
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (!ShouldShowFile(entry.path().filename().string())) {
                continue;
            }
            
            AddFileTreeNode(root_node_.get(), entry);
        }
        
        // 排序节点
        SortNodes(root_node_->children);
    } catch (const std::exception& e) {
        // 处理权限错误等
    }
}

void ProjectExplorerWindow::AddFileTreeNode(FileTreeNode* parent, const std::filesystem::directory_entry& entry)
{
    namespace fs = std::filesystem;
    
    auto node = std::make_unique<FileTreeNode>(
        entry.path().filename().string(),
        entry.path().string(),
        entry.is_directory()
    );
    node->parent = parent;
    
    // 如果是目录，递归添加子节点
    if (entry.is_directory()) {
        try {
            for (const auto& child_entry : fs::directory_iterator(entry.path())) {
                if (!ShouldShowFile(child_entry.path().filename().string())) {
                    continue;
                }
                
                AddFileTreeNode(node.get(), child_entry);
            }
            
            // 排序子节点
            SortNodes(node->children);
        } catch (const std::exception& e) {
            // 处理权限错误等
        }
    }
    
    parent->children.push_back(std::move(node));
}

void ProjectExplorerWindow::RenderFileTree(FileTreeNode* node)
{
    if (!node) return;
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    
    if (node->children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    
    if (node->full_path == selected_path_) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    // 文件图标
    const char* icon = GetFileIcon(std::filesystem::path(node->full_path).extension().string());
    std::string display_name = std::string(icon) + " " + node->name;
    
    bool node_open = ImGui::TreeNodeEx(node->full_path.c_str(), flags, "%s", display_name.c_str());
    
    // 处理选择
    if (ImGui::IsItemClicked()) {
        selected_path_ = node->full_path;
    }
    
    // 处理双击打开文件
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        if (!node->is_directory && file_open_callback_) {
            file_open_callback_(node->full_path);
        }
    }
    
    // 拖拽支持
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("FILE_PATH", node->full_path.c_str(), node->full_path.size() + 1);
        ImGui::Text("%s", node->name.c_str());
        ImGui::EndDragDropSource();
    }
    
    // 递归渲染子节点
    if (node_open && !node->children.empty()) {
        for (auto& child : node->children) {
            RenderFileTree(child.get());
        }
        ImGui::TreePop();
    }
}

void ProjectExplorerWindow::RenderContextMenu()
{
    if (ImGui::BeginPopupContextWindow()) {
        if (!selected_path_.empty()) {
            namespace fs = std::filesystem;
            bool is_directory = fs::is_directory(selected_path_);
            
            if (ImGui::MenuItem("Open") && !is_directory) {
                if (file_open_callback_) {
                    file_open_callback_(selected_path_);
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("New File")) {
                show_new_file_dialog_ = true;
            }
            
            if (ImGui::MenuItem("New Folder")) {
                show_new_folder_dialog_ = true;
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Rename")) {
                show_rename_dialog_ = true;
                strcpy_s(new_name_buffer_, sizeof(new_name_buffer_), 
                        fs::path(selected_path_).filename().string().c_str());
            }
            
            if (ImGui::MenuItem("Delete")) {
                show_delete_confirm_ = true;
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Copy Path")) {
                ImGui::SetClipboardText(selected_path_.c_str());
            }
            
            if (ImGui::MenuItem("Open in Explorer")) {
                // TODO: 在文件资源管理器中打开
            }
        } else {
            if (ImGui::MenuItem("New File")) {
                show_new_file_dialog_ = true;
            }
            
            if (ImGui::MenuItem("New Folder")) {
                show_new_folder_dialog_ = true;
            }
            
            if (ImGui::MenuItem("Refresh")) {
                RefreshProject();
            }
        }
        
        ImGui::EndPopup();
    }
}

void ProjectExplorerWindow::RenderNewFileDialog()
{
    if (show_new_file_dialog_) {
        ImGui::OpenPopup("New File");
        show_new_file_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("New File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("File name:");
        ImGui::InputText("##FileName", new_name_buffer_, sizeof(new_name_buffer_));
        
        ImGui::Spacing();
        
        if (ImGui::Button("Create")) {
            std::string parent_path = selected_path_.empty() ? project_path_ : 
                (std::filesystem::is_directory(selected_path_) ? selected_path_ : 
                 std::filesystem::path(selected_path_).parent_path().string());
            
            std::string new_file_path = parent_path + "/" + new_name_buffer_;
            
            if (CreateFile(new_file_path)) {
                RefreshProject();
                if (file_create_callback_) {
                    file_create_callback_(new_file_path);
                }
            }
            
            memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void ProjectExplorerWindow::RenderNewFolderDialog()
{
    if (show_new_folder_dialog_) {
        ImGui::OpenPopup("New Folder");
        show_new_folder_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Folder name:");
        ImGui::InputText("##FolderName", new_name_buffer_, sizeof(new_name_buffer_));
        
        ImGui::Spacing();
        
        if (ImGui::Button("Create")) {
            std::string parent_path = selected_path_.empty() ? project_path_ : 
                (std::filesystem::is_directory(selected_path_) ? selected_path_ : 
                 std::filesystem::path(selected_path_).parent_path().string());
            
            std::string new_folder_path = parent_path + "/" + new_name_buffer_;
            
            if (CreateFolder(new_folder_path)) {
                RefreshProject();
            }
            
            memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void ProjectExplorerWindow::RenderRenameDialog()
{
    if (show_rename_dialog_) {
        ImGui::OpenPopup("Rename");
        show_rename_dialog_ = false;
    }
    
    if (ImGui::BeginPopupModal("Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New name:");
        ImGui::InputText("##NewName", new_name_buffer_, sizeof(new_name_buffer_));
        
        ImGui::Spacing();
        
        if (ImGui::Button("Rename")) {
            std::string parent_path = std::filesystem::path(selected_path_).parent_path().string();
            std::string new_path = parent_path + "/" + new_name_buffer_;
            
            if (RenamePath(selected_path_, new_path)) {
                selected_path_ = new_path;
                RefreshProject();
            }
            
            memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            memset(new_name_buffer_, 0, sizeof(new_name_buffer_));
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void ProjectExplorerWindow::RenderDeleteConfirmDialog()
{
    if (show_delete_confirm_) {
        ImGui::OpenPopup("Delete Confirmation");
        show_delete_confirm_ = false;
    }
    
    if (ImGui::BeginPopupModal("Delete Confirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete:");
        ImGui::Text("%s", selected_path_.c_str());
        ImGui::Text("This action cannot be undone!");
        
        ImGui::Spacing();
        
        if (ImGui::Button("Delete")) {
            if (DeletePath(selected_path_)) {
                selected_path_.clear();
                RefreshProject();
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

bool ProjectExplorerWindow::CreateFile(const std::string& path)
{
    std::ofstream file(path);
    return file.good();
}

bool ProjectExplorerWindow::CreateFolder(const std::string& path)
{
    return std::filesystem::create_directory(path);
}

bool ProjectExplorerWindow::DeletePath(const std::string& path)
{
    std::error_code ec;
    return std::filesystem::remove_all(path, ec) > 0;
}

bool ProjectExplorerWindow::RenamePath(const std::string& old_path, const std::string& new_path)
{
    std::error_code ec;
    std::filesystem::rename(old_path, new_path, ec);
    return !ec;
}

bool ProjectExplorerWindow::ShouldShowFile(const std::string& filename)
{
    if (!show_hidden_files_ && filename.starts_with(".")) {
        return false;
    }
    
    // 过滤一些不需要显示的文件
    if (filename == "." || filename == "..") {
        return false;
    }
    
    return true;
}

const char* ProjectExplorerWindow::GetFileIcon(const std::string& extension)
{
    if (extension == ".lm") {
        return "🔥"; // Lamina 文件
    } else if (extension == ".cpp" || extension == ".c" || extension == ".h" || extension == ".hpp") {
        return "🔧"; // C/C++ 文件
    } else if (extension == ".txt" || extension == ".md") {
        return "📄"; // 文本文件
    } else if (extension == ".json" || extension == ".xml" || extension == ".yaml") {
        return "⚙️"; // 配置文件
    } else if (extension.empty()) {
        return "📁"; // 目录
    } else {
        return "📎"; // 其他文件
    }
}

void ProjectExplorerWindow::SortNodes(std::vector<std::unique_ptr<FileTreeNode>>& nodes)
{
    std::sort(nodes.begin(), nodes.end(), [](const auto& a, const auto& b) {
        // 目录优先，然后按名称排序
        if (a->is_directory != b->is_directory) {
            return a->is_directory > b->is_directory;
        }
        return a->name < b->name;
    });
}
