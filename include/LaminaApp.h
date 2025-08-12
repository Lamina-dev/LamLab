#pragma once

#include <wx/wx.h>

class LaminaApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

wxDECLARE_APP(LaminaApp);
