#include "ProcessManager.h"
#include <wx/stream.h>
#include <wx/txtstrm.h>

wxBEGIN_EVENT_TABLE(ProcessManager, wxEvtHandler)
    EVT_END_PROCESS(wxID_ANY, ProcessManager::OnProcessTerminate)
    EVT_TIMER(wxID_ANY, ProcessManager::OnTimer)
wxEND_EVENT_TABLE()

ProcessManager::ProcessManager()
    : m_timer(this)
    , m_pid(0)
{
}

ProcessManager::~ProcessManager()
{
    StopProcess();
}

bool ProcessManager::RunCommand(const wxString& command, const wxString& workingDir)
{
    if (IsRunning())
    {
        StopProcess();
    }
    
    m_process = std::make_unique<wxProcess>(this);
    m_process->Redirect();
    
    // 设置工作目录
    wxString currentDir;
    if (!workingDir.IsEmpty())
    {
        currentDir = wxGetCwd();
        wxSetWorkingDirectory(workingDir);
    }
    
    m_pid = wxExecute(command, wxEXEC_ASYNC, m_process.get());
    
    // 恢复工作目录
    if (!workingDir.IsEmpty() && !currentDir.IsEmpty())
    {
        wxSetWorkingDirectory(currentDir);
    }
    
    if (m_pid == 0)
    {
        m_process.reset();
        return false;
    }
    
    // 启动定时器读取输出
    m_timer.Start(100, false);
    
    return true;
}

void ProcessManager::StopProcess()
{
    if (m_process)
    {
        m_timer.Stop();
        
        if (m_pid > 0)
        {
            wxProcess::Kill(m_pid, wxSIGTERM);
        }
        
        m_process.reset();
        m_pid = 0;
    }
}

void ProcessManager::OnProcessTerminate(wxProcessEvent& event)
{
    m_timer.Stop();
    
    // 读取剩余的输出
    ReadOutput();
    ReadError();
    
    int exitCode = event.GetExitCode();
    
    if (m_finishedCallback)
        m_finishedCallback(exitCode);
    
    m_process.reset();
    m_pid = 0;
}

void ProcessManager::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    if (m_process)
    {
        ReadOutput();
        ReadError();
    }
}

void ProcessManager::ReadOutput()
{
    if (!m_process)
        return;
    
    wxInputStream* stream = m_process->GetInputStream();
    if (!stream || !stream->CanRead())
        return;
    
    wxTextInputStream text(*stream);
    wxString line;
    
    while (stream->CanRead())
    {
        wxChar ch = stream->GetC();
        if (stream->LastRead() == 0)
            break;
        
        if (ch == '\n')
        {
            if (m_outputCallback)
                m_outputCallback(line + "\n");
            line.Clear();
        }
        else if (ch != '\r')
        {
            line += ch;
        }
    }
    
    // 处理剩余的字符
    if (!line.IsEmpty() && m_outputCallback)
    {
        m_outputCallback(line);
    }
}

void ProcessManager::ReadError()
{
    if (!m_process)
        return;
    
    wxInputStream* stream = m_process->GetErrorStream();
    if (!stream || !stream->CanRead())
        return;
    
    wxTextInputStream text(*stream);
    wxString line;
    
    while (stream->CanRead())
    {
        wxChar ch = stream->GetC();
        if (stream->LastRead() == 0)
            break;
        
        if (ch == '\n')
        {
            if (m_errorCallback)
                m_errorCallback(line + "\n");
            line.Clear();
        }
        else if (ch != '\r')
        {
            line += ch;
        }
    }
    
    // 处理剩余的字符
    if (!line.IsEmpty() && m_errorCallback)
    {
        m_errorCallback(line);
    }
}
