#pragma once

#include <wx/wx.h>
#include <wx/process.h>
#include <functional>
#include <memory>

class ProcessManager : public wxEvtHandler
{
public:
    ProcessManager();
    virtual ~ProcessManager();
    
    // 运行命令
    bool RunCommand(const wxString& command, const wxString& workingDir = wxEmptyString);
    
    // 停止当前进程
    void StopProcess();
    
    // 查询状态
    bool IsRunning() const { return m_process != nullptr; }
    
    // 设置输出回调
    void SetOutputCallback(std::function<void(const wxString&)> callback) { m_outputCallback = callback; }
    void SetErrorCallback(std::function<void(const wxString&)> callback) { m_errorCallback = callback; }
    void SetFinishedCallback(std::function<void(int)> callback) { m_finishedCallback = callback; }
    
private:
    // 事件处理
    void OnProcessTerminate(wxProcessEvent& event);
    void OnTimer(wxTimerEvent& event);
    
    // 读取输出
    void ReadOutput();
    void ReadError();
    
private:
    std::unique_ptr<wxProcess> m_process;
    wxTimer m_timer;
    int m_pid;
    
    // 回调函数
    std::function<void(const wxString&)> m_outputCallback;
    std::function<void(const wxString&)> m_errorCallback;
    std::function<void(int)> m_finishedCallback;
    
    wxDECLARE_EVENT_TABLE();
};
