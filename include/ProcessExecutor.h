#ifndef PROCESS_EXECUTOR_H
#define PROCESS_EXECUTOR_H

#include <string>
#include <functional>
#include <thread>
#include <memory>

// 进程执行结果
struct ProcessResult {
    int exit_code;
    std::string output;
    std::string error;
    bool success;
    
    ProcessResult() : exit_code(-1), success(false) {}
};

// 异步进程执行器
class ProcessExecutor {
public:
    using OutputCallback = std::function<void(const std::string&, bool)>; // (line, is_error)
    using CompleteCallback = std::function<void(const ProcessResult&)>;
    
    ProcessExecutor();
    ~ProcessExecutor();
    
    // 异步执行命令
    void ExecuteAsync(const std::string& command, 
                     const std::string& working_directory = "",
                     OutputCallback output_callback = nullptr,
                     CompleteCallback complete_callback = nullptr);
    
    // 同步执行命令
    ProcessResult ExecuteSync(const std::string& command, 
                             const std::string& working_directory = "");
    
    // 停止当前执行
    void Stop();
    
    // 检查是否正在运行
    bool IsRunning() const;
    
private:
    bool is_running_;
    std::unique_ptr<std::thread> execution_thread_;
    
    // 平台特定的实现
#ifdef _WIN32
    void ExecuteWindows(const std::string& command, 
                       const std::string& working_directory,
                       OutputCallback output_callback,
                       CompleteCallback complete_callback);
#else
    void ExecuteUnix(const std::string& command, 
                    const std::string& working_directory,
                    OutputCallback output_callback,
                    CompleteCallback complete_callback);
#endif
};

#endif // PROCESS_EXECUTOR_H
