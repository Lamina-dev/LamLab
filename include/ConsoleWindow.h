#ifndef CONSOLE_WINDOW_H
#define CONSOLE_WINDOW_H

#include "LaminaIDE.h"
#include <string>
#include <vector>
#include <deque>
#include <chrono>

// 日志级别
enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug,
    Success
};

// 日志条目
struct LogEntry {
    std::string message;
    LogLevel level;
    std::chrono::system_clock::time_point timestamp;
    
    LogEntry(const std::string& msg, LogLevel lvl) 
        : message(msg), level(lvl), timestamp(std::chrono::system_clock::now()) {}
};

class ConsoleWindow : public IDEWindow {
public:
    ConsoleWindow();
    ~ConsoleWindow() override = default;
    
    void Render() override;
    const char* GetTitle() const override { return "Console"; }
    WindowType GetType() const override { return WindowType::Console; }
    
    // 日志操作
    void AddLog(const std::string& message, LogLevel level = LogLevel::Info);
    void AddInfo(const std::string& message);
    void AddWarning(const std::string& message);
    void AddError(const std::string& message);
    void AddDebug(const std::string& message);
    void AddSuccess(const std::string& message);
    
    void Clear();
    void SaveToFile(const std::string& file_path);
    
    // 设置
    void SetMaxLogEntries(size_t max_entries) { max_log_entries_ = max_entries; }
    void SetAutoScroll(bool auto_scroll) { auto_scroll_ = auto_scroll; }
    void SetShowTimestamp(bool show_timestamp) { show_timestamp_ = show_timestamp; }
    void SetLogLevelFilter(LogLevel min_level) { min_log_level_ = min_level; }
    
private:
    std::deque<LogEntry> log_entries_;
    size_t max_log_entries_;
    bool auto_scroll_;
    bool show_timestamp_;
    bool show_level_colors_;
    LogLevel min_log_level_;
    
    // 过滤器
    char filter_text_[256];
    bool filter_info_;
    bool filter_warning_;
    bool filter_error_;
    bool filter_debug_;
    bool filter_success_;
    
    // 私有方法
    void RenderLogEntry(const LogEntry& entry);
    void RenderFilterBar();
    bool ShouldShowEntry(const LogEntry& entry);
    unsigned int GetLogLevelColor(LogLevel level);
    const char* GetLogLevelText(LogLevel level);
    std::string FormatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    
    // 工具方法
    void TrimOldEntries();
    void CopySelectedToClipboard();
};

#endif // CONSOLE_WINDOW_H
