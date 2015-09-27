#include "znModel.h"
#include "znConstants.h"

#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>

znModel::znModel()
    : m_ini_file(NULL)
{
    // Mapping the ENUM to its string value.
    m_options_enum_to_name[ID_ZN_TXT_SERVICE_BUS_NAMESPACE] = "Service_Bus_Namespace";
    m_options_enum_to_name[ID_ZN_TXT_EVENT_HUB_NAME] = "Event_Hub_Name";
    m_options_enum_to_name[ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME] = "Shared_Access_Policy_Name";
    m_options_enum_to_name[ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY] = "Shared_Access_Policy_Key";
    m_options_enum_to_name[ID_ZN_TXT_USER_MESSAGE] = "User_Message";

    wxString folder = wxStandardPaths::Get().GetAppDocumentsDir() + wxFileName::GetPathSeparator() + "Zailor" + wxFileName::GetPathSeparator();
    wxFileName::Mkdir(folder, 0777, wxPATH_MKDIR_FULL);

    m_ini_file_name = wxStandardPaths::Get().GetExecutablePath();
    m_ini_file_name = folder + wxFileName(m_ini_file_name).GetName() + wxT(".ini");

    if (::wxFileExists(m_ini_file_name) == true)
    {
        // An ini file has been created before.
        // Read its content.

        wxFileInputStream input_stream(m_ini_file_name);

        if (input_stream.IsOk() == true)
        {
            m_ini_file = new wxFileConfig(input_stream);

            // Initialize the missing options.
            if (m_ini_file->Exists(m_options_enum_to_name[ID_ZN_TXT_SERVICE_BUS_NAMESPACE]) == false)
            {
                SetUserOption(ID_ZN_TXT_SERVICE_BUS_NAMESPACE, wxT("zailorbus"));
            }

            if (m_ini_file->Exists(m_options_enum_to_name[ID_ZN_TXT_EVENT_HUB_NAME]) == false)
            {
                SetUserOption(ID_ZN_TXT_EVENT_HUB_NAME, wxT("mydevices"));
            }

            if (m_ini_file->Exists(m_options_enum_to_name[ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME]) == false)
            {
                SetUserOption(ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME, wxT("myname"));
            }

            if (m_ini_file->Exists(m_options_enum_to_name[ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY]) == false)
            {
                SetUserOption(ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY, wxT("ujzdCr0Y3eDWJ2d9y7Mc9XdVvSwJxxx/vMEKk3Tb5ao="));
            }

            if (m_ini_file->Exists(m_options_enum_to_name[ID_ZN_TXT_USER_MESSAGE]) == false)
            {
                SetUserOption(ID_ZN_TXT_USER_MESSAGE, wxEmptyString);
            }
        }
    }

    if (m_ini_file == NULL)
    {
        // No valid ini file is found.
        // Create a new one.
        m_ini_file = new wxFileConfig();

        // Set initial values since the ini file has been newly created.
        SetUserOption(ID_ZN_TXT_SERVICE_BUS_NAMESPACE, wxT("zailorbus")); // K_DEFAULT_WEB_SOCKET_PORT_NUMBER
        SetUserOption(ID_ZN_TXT_EVENT_HUB_NAME, wxT("mydevices"));

        SetUserOption(ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME, wxT("myname"));
        SetUserOption(ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY, wxT("ujzdCr0Y3eDWJ2d9y7Mc9XdVvSwJxxx/vMEKk3Tb5ao="));
        SetUserOption(ID_ZN_TXT_USER_MESSAGE, wxEmptyString);
    }
}
znModel::~znModel()
{
    wxLogDebug(wxT("<<< znModel::~znModel() >>>"));

    SaveUserOptions();

    delete m_ini_file;
}

void znModel::SaveUserOptions()
{
    // This is to write the user options actually into the hard disk.

    wxFileOutputStream output_stream(m_ini_file_name);

    if (output_stream.IsOk() == true)
    {
        m_ini_file->Save(output_stream);
    }
}

wxString znModel::GetUserOption(int key)
{
    return m_ini_file->Read(m_options_enum_to_name[key]);
}

void znModel::SetUserOption(int key, wxString value)
{
    m_ini_file->Write(m_options_enum_to_name[key], value);
}
