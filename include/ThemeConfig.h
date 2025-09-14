#pragma once

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <map>
#include <string>

class ThemeConfig
{
public:
    static ThemeConfig& Get();

    bool LoadTheme(const wxString& themeName = wxEmptyString);
    bool SaveTheme();
    
    wxColour GetColor(const wxString& category, const wxString& element = wxEmptyString) const;
    bool IsBold(const wxString& category, const wxString& element = wxEmptyString) const;
    bool IsItalic(const wxString& category, const wxString& element = wxEmptyString) const;
    
    wxString GetCurrentTheme() const { return m_currentTheme; }
    const wxArrayString& GetAvailableThemes() const { return m_availableThemes; }

private:
    ThemeConfig();
    ~ThemeConfig();

    bool LoadConfigFile();
    bool ParseThemeConfig(wxXmlNode* root);
    wxColour ParseColor(const wxString& colorStr) const;
    bool GetNodeValueBool(wxXmlNode* node, const wxString& childName, bool defaultValue = false) const;
    wxString GetNodeValueStr(wxXmlNode* node, const wxString& childName, const wxString& defaultValue = wxEmptyString) const;
    wxXmlNode* FindThemeNode(const wxString& themeName) const;
    wxXmlNode* FindStyleNode(wxXmlNode* themeNode, const wxString& category, const wxString& element = wxEmptyString) const;

private:
    wxString m_configPath;
    wxString m_currentTheme;
    wxArrayString m_availableThemes;
    wxXmlDocument m_config;
    
    static ThemeConfig* s_instance;
};