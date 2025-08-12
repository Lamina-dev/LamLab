#include "LaminaApp.h"
#include "MainFrame.h"

bool LaminaApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    
    // 创建主窗口
    auto* frame = new MainFrame();
    frame->Show(true);
    
    return true;
}
