#include "ConsoleWindow.h"
#include "imgui.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>

ConsoleWindow::ConsoleWindow()
    : max_log_entries_(1000)
    , auto_scroll_(true)
    , show_timestamp_(true)
    , show_level_colors_(true)
    , min_log_level_(LogLevel::Info)
    , filter_info_(true)
    , filter_warning_(true)
    , filter_error_(true)
    , filter_debug_(true)
    , filter_success_(true)
{
    memset(filter_text_, 0, sizeof(filter_text_));
}

void ConsoleWindow::Render()
{
    if (!IsVisible()) return;
    
    if (ImGui::Begin("Console", &visible_)) {
        // 工具栏
        if (ImGui::Button("Clear")) {
            Clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Log")) {
            SaveToFile("console_log.txt");
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &auto_scroll_);
        ImGui::SameLine();
        ImGui::Checkbox("Timestamp", &show_timestamp_);
        
        // 过滤器栏
        RenderFilterBar();
        
        ImGui::Separator();
        
        // 日志区域
        ImGui::BeginChild("LogArea", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        
        for (const auto& entry : log_entries_) {
            if (ShouldShowEntry(entry)) {
                RenderLogEntry(entry);
            }
        }
        
        // 自动滚动到底部
        if (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        
        ImGui::EndChild();
    }
    ImGui::End();
}

void ConsoleWindow::RenderFilterBar()
{
    ImGui::Text("Filter:");
    ImGui::SameLine();
    ImGui::InputText("##Filter", filter_text_, sizeof(filter_text_));
    ImGui::SameLine();
    
    // 日志级别过滤器
    ImGui::Checkbox("Info", &filter_info_);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &filter_warning_);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &filter_error_);
    ImGui::SameLine();
    ImGui::Checkbox("Debug", &filter_debug_);
    ImGui::SameLine();
    ImGui::Checkbox("Success", &filter_success_);
}

void ConsoleWindow::RenderLogEntry(const LogEntry& entry)
{
    // 设置颜色
    if (show_level_colors_) {
        ImGui::PushStyleColor(ImGuiCol_Text, GetLogLevelColor(entry.level));
    }
    
    std::string log_line;
    
    // 添加时间戳
    if (show_timestamp_) {
        log_line += "[" + FormatTimestamp(entry.timestamp) + "] ";
    }
    
    // 添加日志级别
    log_line += "[" + std::string(GetLogLevelText(entry.level)) + "] ";
    
    // 添加消息
    log_line += entry.message;
    
    ImGui::TextUnformatted(log_line.c_str());
    
    if (show_level_colors_) {
        ImGui::PopStyleColor();
    }
}

void ConsoleWindow::AddLog(const std::string& message, LogLevel level)
{
    log_entries_.emplace_back(message, level);
    TrimOldEntries();
}

void ConsoleWindow::AddInfo(const std::string& message)
{
    AddLog(message, LogLevel::Info);
}

void ConsoleWindow::AddWarning(const std::string& message)
{
    AddLog(message, LogLevel::Warning);
}

void ConsoleWindow::AddError(const std::string& message)
{
    AddLog(message, LogLevel::Error);
}

void ConsoleWindow::AddDebug(const std::string& message)
{
    AddLog(message, LogLevel::Debug);
}

void ConsoleWindow::AddSuccess(const std::string& message)
{
    AddLog(message, LogLevel::Success);
}

void ConsoleWindow::Clear()
{
    log_entries_.clear();
}

void ConsoleWindow::SaveToFile(const std::string& file_path)
{
    std::ofstream file(file_path);
    if (!file) {
        AddError("Failed to save log to file: " + file_path);
        return;
    }
    
    for (const auto& entry : log_entries_) {
        std::string log_line;
        
        // 添加时间戳
        log_line += "[" + FormatTimestamp(entry.timestamp) + "] ";
        
        // 添加日志级别
        log_line += "[" + std::string(GetLogLevelText(entry.level)) + "] ";
        
        // 添加消息
        log_line += entry.message + "\n";
        
        file << log_line;
    }
    
    AddSuccess("Log saved to: " + file_path);
}

bool ConsoleWindow::ShouldShowEntry(const LogEntry& entry)
{
    // 检查日志级别过滤器
    switch (entry.level) {
        case LogLevel::Info:
            if (!filter_info_) return false;
            break;
        case LogLevel::Warning:
            if (!filter_warning_) return false;
            break;
        case LogLevel::Error:
            if (!filter_error_) return false;
            break;
        case LogLevel::Debug:
            if (!filter_debug_) return false;
            break;
        case LogLevel::Success:
            if (!filter_success_) return false;
            break;
    }
    
    // 检查最小日志级别
    if (entry.level < min_log_level_) {
        return false;
    }
    
    // 检查文本过滤器
    if (strlen(filter_text_) > 0) {
        std::string message_lower = entry.message;
        std::string filter_lower = filter_text_;
        
        // 转换为小写进行比较
        std::transform(message_lower.begin(), message_lower.end(), message_lower.begin(), ::tolower);
        std::transform(filter_lower.begin(), filter_lower.end(), filter_lower.begin(), ::tolower);
        
        if (message_lower.find(filter_lower) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

unsigned int ConsoleWindow::GetLogLevelColor(LogLevel level)
{
    switch (level) {
        case LogLevel::Info:
            return IM_COL32(255, 255, 255, 255); // 白色
        case LogLevel::Warning:
            return IM_COL32(255, 255, 0, 255);   // 黄色
        case LogLevel::Error:
            return IM_COL32(255, 100, 100, 255); // 红色
        case LogLevel::Debug:
            return IM_COL32(150, 150, 150, 255); // 灰色
        case LogLevel::Success:
            return IM_COL32(100, 255, 100, 255); // 绿色
        default:
            return IM_COL32(255, 255, 255, 255);
    }
}

const char* ConsoleWindow::GetLogLevelText(LogLevel level)
{
    switch (level) {
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Success: return "SUCCESS";
        default:                return "UNKNOWN";
    }
}

std::string ConsoleWindow::FormatTimestamp(const std::chrono::system_clock::time_point& timestamp)
{
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

void ConsoleWindow::TrimOldEntries()
{
    while (log_entries_.size() > max_log_entries_) {
        log_entries_.pop_front();
    }
}

void ConsoleWindow::CopySelectedToClipboard()
{
    // TODO: 实现复制选中的日志条目到剪贴板
}
