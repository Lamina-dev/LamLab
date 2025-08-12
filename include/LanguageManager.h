#pragma once

#include <wx/wx.h>
#include <wx/intl.h>
#include <map>
#include <memory>

class LanguageManager
{
public:
    enum Language
    {
        LANG_ENGLISH = 0,
        LANG_CHINESE_SIMPLIFIED,
        LANG_COUNT
    };

    static LanguageManager& GetInstance();
    
    bool Initialize();
    bool SetLanguage(Language lang);
    Language GetCurrentLanguage() const { return m_currentLanguage; }
    
    wxString GetText(const wxString& key) const;
    wxString _(const wxString& key) const { return GetText(key); }
    
    wxArrayString GetAvailableLanguages() const;
    wxString GetLanguageName(Language lang) const;
    
    void LoadSettings();
    void SaveSettings();
    
private:
    LanguageManager() = default;
    ~LanguageManager() = default;
    LanguageManager(const LanguageManager&) = delete;
    LanguageManager& operator=(const LanguageManager&) = delete;
    
    void InitializeTranslations();
    void InitializeEnglish();
    void InitializeChineseSimplified();
    
private:
    Language m_currentLanguage = LANG_ENGLISH;
    std::unique_ptr<wxLocale> m_locale;
    std::map<Language, std::map<wxString, wxString>> m_translations;
};
