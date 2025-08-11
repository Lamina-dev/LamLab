#include "ProcessExecutor.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#endif

ProcessExecutor::ProcessExecutor()
    : is_running_(false)
{
}

ProcessExecutor::~ProcessExecutor()
{
    Stop();
}

void ProcessExecutor::ExecuteAsync(const std::string& command, 
                                  const std::string& working_directory,
                                  OutputCallback output_callback,
                                  CompleteCallback complete_callback)
{
    if (is_running_) {
        Stop();
    }
    
    is_running_ = true;
    execution_thread_ = std::make_unique<std::thread>([this, command, working_directory, output_callback, complete_callback]() {
#ifdef _WIN32
        ExecuteWindows(command, working_directory, output_callback, complete_callback);
#else
        ExecuteUnix(command, working_directory, output_callback, complete_callback);
#endif
    });
}

ProcessResult ProcessExecutor::ExecuteSync(const std::string& command, 
                                          const std::string& working_directory)
{
    ProcessResult result;
    
#ifdef _WIN32
    SECURITY_ATTRIBUTES security_attrs;
    security_attrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attrs.bInheritHandle = TRUE;
    security_attrs.lpSecurityDescriptor = NULL;
    
    HANDLE stdout_read, stdout_write;
    HANDLE stderr_read, stderr_write;
    
    if (!CreatePipe(&stdout_read, &stdout_write, &security_attrs, 0) ||
        !CreatePipe(&stderr_read, &stderr_write, &security_attrs, 0)) {
        result.error = "Failed to create pipes";
        return result;
    }
    
    SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0);
    
    STARTUPINFOA startup_info;
    PROCESS_INFORMATION process_info;
    
    ZeroMemory(&startup_info, sizeof(STARTUPINFOA));
    startup_info.cb = sizeof(STARTUPINFOA);
    startup_info.hStdError = stderr_write;
    startup_info.hStdOutput = stdout_write;
    startup_info.dwFlags |= STARTF_USESTDHANDLES;
    
    std::string cmd = "cmd.exe /C " + command;
    
    if (CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, TRUE, 
                       CREATE_NO_WINDOW, NULL, 
                       working_directory.empty() ? NULL : working_directory.c_str(),
                       &startup_info, &process_info)) {
        
        CloseHandle(stdout_write);
        CloseHandle(stderr_write);
        
        // 读取输出
        DWORD bytes_read;
        char buffer[4096];
        
        while (ReadFile(stdout_read, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
            buffer[bytes_read] = '\0';
            result.output += buffer;
        }
        
        while (ReadFile(stderr_read, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
            buffer[bytes_read] = '\0';
            result.error += buffer;
        }
        
        WaitForSingleObject(process_info.hProcess, INFINITE);
        
        DWORD exit_code;
        GetExitCodeProcess(process_info.hProcess, &exit_code);
        result.exit_code = static_cast<int>(exit_code);
        result.success = (exit_code == 0);
        
        CloseHandle(process_info.hProcess);
        CloseHandle(process_info.hThread);
    } else {
        result.error = "Failed to create process";
    }
    
    CloseHandle(stdout_read);
    CloseHandle(stderr_read);
    
#else
    // Unix implementation
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        result.error = "Failed to execute command";
        return result;
    }
    
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result.output += buffer;
    }
    
    result.exit_code = pclose(pipe);
    result.success = (result.exit_code == 0);
#endif
    
    return result;
}

void ProcessExecutor::Stop()
{
    is_running_ = false;
    if (execution_thread_ && execution_thread_->joinable()) {
        execution_thread_->join();
    }
    execution_thread_.reset();
}

bool ProcessExecutor::IsRunning() const
{
    return is_running_;
}

#ifdef _WIN32
void ProcessExecutor::ExecuteWindows(const std::string& command, 
                                    const std::string& working_directory,
                                    OutputCallback output_callback,
                                    CompleteCallback complete_callback)
{
    ProcessResult result = ExecuteSync(command, working_directory);
    
    if (output_callback) {
        if (!result.output.empty()) {
            output_callback(result.output, false);
        }
        if (!result.error.empty()) {
            output_callback(result.error, true);
        }
    }
    
    if (complete_callback) {
        complete_callback(result);
    }
    
    is_running_ = false;
}
#else
void ProcessExecutor::ExecuteUnix(const std::string& command, 
                                 const std::string& working_directory,
                                 OutputCallback output_callback,
                                 CompleteCallback complete_callback)
{
    ProcessResult result = ExecuteSync(command, working_directory);
    
    if (output_callback) {
        if (!result.output.empty()) {
            output_callback(result.output, false);
        }
        if (!result.error.empty()) {
            output_callback(result.error, true);
        }
    }
    
    if (complete_callback) {
        complete_callback(result);
    }
    
    is_running_ = false;
}
#endif
