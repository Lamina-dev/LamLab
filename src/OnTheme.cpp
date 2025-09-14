void MainFrame::OnTheme(wxCommandEvent& event)
{
    int themeIndex = event.GetId() - ID_THEME_START;
    const wxArrayString& themes = ThemeConfig::Get().GetAvailableThemes();
    
    if (themeIndex >= 0 && themeIndex < (int)themes.GetCount())
    {
        if (m_editor)
        {
            m_editor->ApplyTheme(themes[themeIndex]);
        }
    }
}