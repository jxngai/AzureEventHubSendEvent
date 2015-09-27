#ifndef ZNMODEL_H
#define ZNMODEL_H

#include "znSingleton.h"

#include <wx/fileconf.h>

#include <map>

class znModel : public znSingleton
{
private:
    friend znSingleton;

    znModel();
    wxDECLARE_NO_COPY_CLASS(znModel);
    ~znModel();

    void SaveUserOptions();

public:
    // User options are properties that need to be stored
    // persistently in the option database.

    wxString GetUserOption(int);
    void SetUserOption(int, wxString);

    // There should be others properties that live
    // only during the application life time.
    // This is probably to facilitate MVC model where
    // Controllers other than UiController should not talk to
    // View directly.

private:
    wxString                        m_ini_file_name;
    wxFileConfig                   *m_ini_file;

    std::map<int, wxString>         m_options_enum_to_name;
};

#endif