#include "ThemeConfig.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/file.h>
#include <wx/sstream.h>
#include <wx/dir.h>

ThemeConfig* ThemeConfig::s_instance = nullptr;

ThemeConfig& ThemeConfig::Get()
{
    if (!s_instance)
        s_instance = new ThemeConfig();
    return *s_instance;
}

ThemeConfig::ThemeConfig()
{   
    // 设置配置文件路径
        wxFileName configPath(wxStandardPaths::Get().GetExecutablePath());
        configPath.SetPath(configPath.GetPath() + wxFileName::GetPathSeparator() + "config");
        configPath.SetFullName("themes.xml");
        m_configPath = configPath.GetFullPath();

    
    LoadConfigFile();
}

ThemeConfig::~ThemeConfig()
{
    if (s_instance == this)
        s_instance = nullptr;
}

// 内置的默认主题配置
static const char* DEFAULT_THEME_CONFIG = R"(
<?xml version="1.0" encoding="UTF-8"?>
<themes>
    <current>default</current>
    <theme name="default">
        <!-- 基本样式 -->
        <default>
            <foreground>#24292E</foreground>
            <background>#FFFFFF</background>
        </default>
        <keywords>
            <foreground>#D73A49</foreground>
            <bold>true</bold>
        </keywords>
        <identifiers>
            <foreground>#24292E</foreground>
        </identifiers>
        <comments>
            <foreground>#6A737D</foreground>
            <italic>true</italic>
        </comments>
        <strings>
            <foreground>#032F62</foreground>
        </strings>
        <numbers>
            <foreground>#005CC5</foreground>
        </numbers>
        <operators>
            <foreground>#24292E</foreground>
            <bold>false</bold>
        </operators>
        <functions>
            <foreground>#6F42C1</foreground>
        </functions>
        <constants>
            <foreground>#005CC5</foreground>
            <bold>true</bold>
        </constants>
        
        <!-- 编辑器界面样式 -->
        <linenumber>
            <foreground>#2B91AF</foreground>
            <background>#FFFFFF</background>
        </linenumber>
        <currentLine>
            <background>#E8F2FF</background>
        </currentLine>
        <braceMatch>
            <foreground>#0000FF</foreground>
            <background>#CCCCFF</background>
            <bold>true</bold>
        </braceMatch>
        <braceMismatch>
            <foreground>#FF0000</foreground>
            <background>#FFE0E0</background>
            <bold>true</bold>
        </braceMismatch>
    </theme>
</themes>
)";

bool ThemeConfig::LoadConfigFile()
{
    bool loadedFromFile = false;
    
    // 尝试从文件加载
    if (wxFile::Exists(m_configPath))
    {
        loadedFromFile = m_config.Load(m_configPath);
    }
    
    // 如果从文件加载失败，使用内置配置
    if (!loadedFromFile)
    {
        wxStringInputStream stream(DEFAULT_THEME_CONFIG);
        if (!m_config.Load(stream))
            return false;
            
        // 尝试保存到文件，但不影响程序运行
        wxString configDir = wxFileName(m_configPath).GetPath();
        if (!wxDirExists(configDir))
            wxMkdir(configDir);
        m_config.Save(m_configPath);
    }

    // 获取根节点
    wxXmlNode* root = m_config.GetRoot();
    if (!root || root->GetName() != "themes")
        return false;

    return ParseThemeConfig(root);
}

bool ThemeConfig::ParseThemeConfig(wxXmlNode* root)
{
    m_availableThemes.Clear();

    // 读取当前主题
    wxXmlNode* currentNode = root->GetChildren();
    while (currentNode && currentNode->GetName() != "current")
        currentNode = currentNode->GetNext();

    if (currentNode)
        m_currentTheme = currentNode->GetNodeContent();

    // 读取主题列表
    wxXmlNode* themeNode = root->GetChildren();
    while (themeNode)
    {
        if (themeNode->GetName() == "theme")
        {
            wxString themeName = themeNode->GetAttribute("name");
            if (!themeName.IsEmpty())
                m_availableThemes.Add(themeName);
        }
        themeNode = themeNode->GetNext();
    }

    if (m_currentTheme.IsEmpty() && !m_availableThemes.IsEmpty())
        m_currentTheme = m_availableThemes[0];

    return !m_availableThemes.IsEmpty();
}

bool ThemeConfig::LoadTheme(const wxString& themeName)
{
    wxString theme = themeName;
    if (theme.IsEmpty())
        theme = m_currentTheme;
    
    if (m_currentTheme != theme)
    {
        m_currentTheme = theme;

        // 更新配置文件中的当前主题
        wxXmlNode* root = m_config.GetRoot();
        if (root)
        {
            wxXmlNode* currentNode = root->GetChildren();
            while (currentNode)
            {
                if (currentNode->GetName() == "current")
                {
                    currentNode->GetChildren()->SetContent(theme);
                    break;
                }
                currentNode = currentNode->GetNext();
            }
        }

        return m_config.Save(m_configPath);
    }

    return true;
}

bool ThemeConfig::SaveTheme()
{
    return m_config.Save(m_configPath);
}

wxString ThemeConfig::GetNodeValueStr(wxXmlNode* node, const wxString& childName, const wxString& defaultValue) const
{
    if (!node)
        return defaultValue;

    wxXmlNode* child = node->GetChildren();
    while (child)
    {
        if (child->GetName() == childName && child->GetChildren())
            return child->GetChildren()->GetContent();
        child = child->GetNext();
    }

    return defaultValue;
}

bool ThemeConfig::GetNodeValueBool(wxXmlNode* node, const wxString& childName, bool defaultValue) const
{
    wxString value = GetNodeValueStr(node, childName);
    if (value.IsEmpty())
        return defaultValue;
    return value == "true" || value == "1";
}

wxXmlNode* ThemeConfig::FindThemeNode(const wxString& themeName) const
{
    wxXmlNode* root = m_config.GetRoot();
    if (!root)
        return nullptr;

    wxXmlNode* node = root->GetChildren();
    while (node)
    {
        if (node->GetName() == "theme" && node->GetAttribute("name") == themeName)
            return node;
        node = node->GetNext();
    }

    return nullptr;
}

wxXmlNode* ThemeConfig::FindStyleNode(wxXmlNode* themeNode, const wxString& category, const wxString& element) const
{
    if (!themeNode)
        return nullptr;

    wxXmlNode* categoryNode = themeNode->GetChildren();
    while (categoryNode)
    {
        if (categoryNode->GetName() == category)
        {
            if (element.IsEmpty())
                return categoryNode;

            wxXmlNode* elementNode = categoryNode->GetChildren();
            while (elementNode)
            {
                if (elementNode->GetName() == element)
                    return elementNode;
                elementNode = elementNode->GetNext();
            }
        }
        categoryNode = categoryNode->GetNext();
    }

    return nullptr;
}

wxColour ThemeConfig::GetColor(const wxString& category, const wxString& element) const
{
    wxXmlNode* themeNode = FindThemeNode(m_currentTheme);
    if (!themeNode)
        return *wxBLACK;

    wxXmlNode* styleNode = FindStyleNode(themeNode, category, element);
    if (!styleNode)
        return *wxBLACK;

    wxString colorStr = GetNodeValueStr(styleNode, "foreground", "#000000");
    return ParseColor(colorStr);
}

bool ThemeConfig::IsBold(const wxString& category, const wxString& element) const
{
    wxXmlNode* themeNode = FindThemeNode(m_currentTheme);
    if (!themeNode)
        return false;

    wxXmlNode* styleNode = FindStyleNode(themeNode, category, element);
    if (!styleNode)
        return false;

    return GetNodeValueBool(styleNode, "bold", false);
}

bool ThemeConfig::IsItalic(const wxString& category, const wxString& element) const
{
    wxXmlNode* themeNode = FindThemeNode(m_currentTheme);
    if (!themeNode)
        return false;

    wxXmlNode* styleNode = FindStyleNode(themeNode, category, element);
    if (!styleNode)
        return false;

    return GetNodeValueBool(styleNode, "italic", false);
}

wxColour ThemeConfig::ParseColor(const wxString& colorStr) const
{
    if (colorStr.StartsWith("#"))
    {
        unsigned long color;
        if (colorStr.Mid(1).ToULong(&color, 16))
        {
            if (colorStr.Length() == 7) // #RRGGBB
            {
                return wxColour((color >> 16) & 0xFF,
                              (color >> 8) & 0xFF,
                              color & 0xFF);
            }
            else if (colorStr.Length() == 4) // #RGB
            {
                int r = (color >> 8) & 0xF;
                int g = (color >> 4) & 0xF;
                int b = color & 0xF;
                return wxColour(r | (r << 4),
                              g | (g << 4),
                              b | (b << 4));
            }
        }
    }
    return *wxBLACK;
}