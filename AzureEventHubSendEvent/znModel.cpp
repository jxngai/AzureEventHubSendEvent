#include "common.h"
#include "znModel.h"
#include "znConstants.h"

#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>

std::map<int, znIniProperty> g_ini_property =
{
    {
        ID_ZN_TXT_HTTPS_SERVICE_BUS_NAMESPACE,
        {
            "HTTPS_Service_Bus_Namespace",
            "zailorbus"
        }
    },
    {
        ID_ZN_TXT_HTTPS_EVENT_HUB_NAME,
        {
            "HTTPS_Event_Hub_Name",
            "mydevices"
        }
    },
    {
        ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_NAME,
        {
            "HTTPS_Shared_Access_Policy_Name",
            "myname"
        }
    },
    {
        ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_KEY,
        {
            "HTTPS_Shared_Access_Policy_Key",
            "ujzdCr0Y3eDWJ2d9y7Mc9XdVvSwJxxx/vMEKk3Tb5ao="
        }
    },
    {
        ID_ZN_TXT_HTTPS_USER_MESSAGE,
        {
            "HTTPS_User_Message",
            wxEmptyString
        }
    },
    {
        ID_ZN_TXT_AMQPS_SERVICE_BUS_NAMESPACE,
        {
            "AMQPS_Service_Bus_Namespace",
            "zailorbus"
        }
    },
    {
        ID_ZN_TXT_AMQPS_EVENT_HUB_NAME,
        {
            "AMQPS_Event_Hub_Name",
            "mydevices"
        }
    },
    {
        ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_NAME,
        {
            "AMQPS_Shared_Access_Policy_Name",
            "myname"
        }
    },
    {
        ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_KEY,
        {
            "AMQPS_Shared_Access_Policy_Key",
            "ujzdCr0Y3eDWJ2d9y7Mc9XdVvSwJxxx/vMEKk3Tb5ao="
        }
    },
    {
        ID_ZN_TXT_AMQPS_USER_MESSAGE,
        {
            "AMQPS_User_Message",
            wxEmptyString
        }
    },
    {
        ID_ZN_TXT_HMAC_SERVICE_BUS_NAMESPACE,
        {
            "HMAC_Service_Bus_Namespace",
            "zailorbus"
        }
    },
    {
        ID_ZN_TXT_HMAC_EVENT_HUB_NAME,
        {
            "HMAC_Event_Hub_Name",
            "mydevices"
        }
    },
    {
        ID_ZN_TXT_HMAC_PUBLISHER_NAME,
        {
            "HMAC_Publisher_Name",
            "mypublisher"
        }
    },
    {
        ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_NAME,
        {
            "HMAC_Shared_Access_Policy_Name",
            "myname"
        }
    },
    {
        ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_KEY,
        {
            "HMAC_Shared_Access_Policy_Key",
            "ujzdCr0Y3eDWJ2d9y7Mc9XdVvSwJxxx/vMEKk3Tb5ao="
        }
    },
    {
        ID_ZN_TXT_HMAC_TTL,
        {
            "HMAC_TTL",
            "3600"
        }
    }

};

znModel::znModel()
    : m_ini_file(NULL)
{
    // Use ini file to save all user settings.

    wxString folder = wxStandardPaths::Get().GetAppDocumentsDir() + wxFileName::GetPathSeparator() + "Zailor" + wxFileName::GetPathSeparator();
    wxFileName::Mkdir(folder, 0777, wxPATH_MKDIR_FULL);

    m_ini_file_name = wxStandardPaths::Get().GetExecutablePath();
    m_ini_file_name = folder + wxFileName(m_ini_file_name).GetName() + wxT(".ini");

    if (::wxFileExists(m_ini_file_name) == true)
    {
        // An ini file is found, it has been created before.
        // Read its content.

        wxFileInputStream input_stream(m_ini_file_name);

        if (input_stream.IsOk() == true)
        {
            m_ini_file = new wxFileConfig(input_stream);

            // The ini file has been read correctly.
            // Initialize the missing options if any.

            for (std::map<int, znIniProperty>::const_iterator iter = g_ini_property.begin(); iter != g_ini_property.end(); iter++)
            {
                int key = iter->first;
                znIniProperty value = iter->second;

                if (m_ini_file->Exists(value.m_name) == false)
                {
                    // The option is missing from the existing ini file.
                    // Probably a newly added option.
                    // Set it to its default value.
                    SetUserOption(key, value.m_default_value);
                }
            }
        }
    }

    if (m_ini_file == NULL)
    {
        // No valid ini file is found.
        // Create a new one.
        m_ini_file = new wxFileConfig();

        // Set initial values for all the options since the ini file has been newly created.

        for (std::map<int, znIniProperty>::iterator iter = g_ini_property.begin(); iter != g_ini_property.end(); iter++)
        {
            int key = iter->first;
            znIniProperty value = iter->second;

            SetUserOption(key, value.m_default_value);
        }
    }
}
znModel::~znModel()
{
    wxLogDebug(wxT("<<< znModel::~znModel() >>>"));

    // Save all options into a physical ini file.
    wxFileOutputStream output_stream(m_ini_file_name);

    if (output_stream.IsOk() == true)
    {
        m_ini_file->Save(output_stream);
    }

    delete m_ini_file;
}

wxString znModel::GetUserOption(int key)
{
    return m_ini_file->Read(g_ini_property[key].m_name);
}

void znModel::SetUserOption(int key, wxString value)
{
    m_ini_file->Write(g_ini_property[key].m_name, value);
}
