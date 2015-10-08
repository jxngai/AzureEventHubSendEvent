#include "common.h"
#include "znControllerUi.h"
#include "znConstants.h"
#include "znModel.h"
#include "znThreadSendEventQpid.h"
#include "znUtils.h"

#include <wx/textctrl.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/base64.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/msgdlg.h>
#include <wx/clipbrd.h>
#include <wx/stattext.h>

znControllerUi::znControllerUi()
{
    wxLogDebug(wxT("<<< znControllerUi::znControllerUi() >>>"));

    InitializeUiControls();

    // Check if there is any user message during application start up.
    // If not initialize it with some random message.

    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        wxString message = text_control->GetValue();
        message.Trim();

        if (message == wxEmptyString)
        {
            wxCommandEvent event;
            OnBtnHttpsSetSampleMessage(event);
        }
    }
}

znControllerUi::~znControllerUi()
{
}

void znControllerUi::InitializeUiControls()
{
    // This routine is meant to be called after wxFrame has been created.
    // It is to initialize some widgets with user values stored in the ini file.

    for (std::map<int, znIniProperty>::const_iterator iter = g_ini_property.begin(); iter != g_ini_property.end(); iter++)
    {
        int key = iter->first;
        znIniProperty value = iter->second;

        wxWindow *window = wxWindow::FindWindowById(key);

        if (window == NULL) continue;

        wxString user_option = znSingleton::GetInstance<znModel>().GetUserOption(key).c_str();

        wxCheckBox * checkbox = wxDynamicCast(window, wxCheckBox);
        wxTextCtrl * textctrl = wxDynamicCast(window, wxTextCtrl);
        wxChoice * choice = wxDynamicCast(window, wxChoice);

        if (checkbox != NULL)
        {
            if (user_option == wxT("yes"))
            {
                checkbox->SetValue(true);
            }
            else
            {
                checkbox->SetValue(false);
            }
        }
        else if (textctrl != NULL)
        {
            textctrl->SetValue(user_option);
        }
        else if (choice != NULL)
        {
            choice->SetStringSelection(user_option);
        }
    }
}

void znControllerUi::SetStatusText(wxString message)
{
    wxFrame *frame = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_FRAME), wxFrame);
    if (frame != NULL)
    {
        frame->SetStatusText(message);
    }
}

void znControllerUi::OnClose(wxCloseEvent& event)
{
    // Save all the user's options into the model.
    // Note that it does not write the options into the physical ini file.
    // Instead that is done by the destructor of znModel.

    for (std::map<int, znIniProperty>::const_iterator iter = g_ini_property.begin(); iter != g_ini_property.end(); iter++)
    {
        int key = iter->first;
        znIniProperty value = iter->second;

        wxWindow *window = wxWindow::FindWindowById(key);

        if (window == NULL) continue;

        wxString user_option;

        wxCheckBox * checkbox = wxDynamicCast(window, wxCheckBox);
        wxTextCtrl * textctrl = wxDynamicCast(window, wxTextCtrl);
        wxChoice * choice = wxDynamicCast(window, wxChoice);

        if (checkbox != NULL)
        {
            if (checkbox->GetValue() == true)
            {
                user_option = wxT("yes");
                znSingleton::GetInstance<znModel>().SetUserOption(key, user_option);
            }
            else
            {
                user_option = wxT("no");
                znSingleton::GetInstance<znModel>().SetUserOption(key, user_option);
            }
        }
        else if (textctrl != NULL)
        {
            user_option = textctrl->GetValue();
            znSingleton::GetInstance<znModel>().SetUserOption(key, user_option);
        }
        else if (choice != NULL)
        {
            user_option = choice->GetStringSelection();
            znSingleton::GetInstance<znModel>().SetUserOption(key, user_option);
        }
    }

    // Need to Skip() so that the event can continue to be processed.
    event.Skip();
}

void znControllerUi::OnBtnHttpsSendMessage(wxCommandEvent& event)
{
    wxLogDebug("<<< znControllerUi::OnBtnHttpsSendMessage >>>");

    // Read all settings from the UI.
    wxString arg_service_bus_name;
    wxString arg_event_hub_name;
    wxString arg_shared_access_policy_name;
    wxString arg_shared_access_key;

    wxTextCtrl *text_control;

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_SERVICE_BUS_NAMESPACE), wxTextCtrl);

    if (text_control != NULL) arg_service_bus_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_EVENT_HUB_NAME), wxTextCtrl);

    if (text_control != NULL) arg_event_hub_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_NAME), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_policy_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_KEY), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_key = text_control->GetValue();

    znHmacSignature hmac_values = znUtils::GenerateHmacSignature(arg_service_bus_name, arg_event_hub_name, wxEmptyString,
        arg_shared_access_policy_name, arg_shared_access_key, 60 * 1);

    ////////////////////////////////////////////////////////
    // Setup the target URL.
    ////////////////////////////////////////////////////////

    // The target URL is of the form of
    //     https://zailorbus.servicebus.windows.net/mydevices/messages

    std::string params_url = "https://";
    params_url += hmac_values.m_url_1.mb_str(wxMBConvUTF8());
    params_url += "/";
    params_url += arg_event_hub_name.mb_str(wxMBConvUTF8());
    params_url += "/messages";

    ////////////////////////////////////////////////////////
    // Setup the headers.
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    // Setup the content of the body.
    ////////////////////////////////////////////////////////

    wxString params_data;

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        params_data = text_control->GetValue();

        if (params_data == wxEmptyString)
        {
            wxMessageBox("No data to send. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
            return;
        }
    }

    // Now use Curl to post the HTTPS request.

    int status_code = znUtils::CurlPostHttps(params_url, hmac_values.m_authorization_string_1, params_data);

    ////////////////////////////////////////////////////////
    // Construct the status message.
    ////////////////////////////////////////////////////////

    // Take the current time stamp.
    wxString status_message = wxDateTime::Now().Format(wxT("Time : %Y-%m-%d %H:%M:%S"));

    status_message += "\nURL : " + params_url;
    status_message += "\nHeader : " + hmac_values.m_authorization_string_1;
    status_message += "\n" + params_data;
    status_message += "\nStatus Code = " + wxString::Format("%d", status_code);

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_STATUS_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->SetValue(status_message);
    }
}

void znControllerUi::OnBtnHttpsSetSampleMessage(wxCommandEvent& event)
{
    // Generate some random data.

    srand(time(NULL));
    int id = 1 + (rand() % 50);
    int temperature = (rand() % 100);

    wxString value = wxString::Format("{ \"DeviceId\":%02d, \"Temperature\":%02d }", id, temperature);

    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->SetValue(value);
}

void znControllerUi::OnBtnHttpsClearMessage(wxCommandEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->Clear();
}

void znControllerUi::OnBtnHttpsClearStatusMessage(wxCommandEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_STATUS_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->Clear();
}

void znControllerUi::OnBtnAmqpsSendMessage(wxCommandEvent& event)
{
    wxLogDebug("<<< znControllerUi::OnBtnAmqpsSendMessage >>>");

    if (m_status_message_current != wxEmptyString)
    {
        m_status_message_accumulated = m_status_message_current + "\nStatus Code = " + wxString::Format("%d", m_status_code) + m_status_message_accumulated;
    }

    if (m_status_message_accumulated != wxEmptyString)
    {
        m_status_message_accumulated = "\n+-------------------------------------------+\n" + m_status_message_accumulated;
    }

    m_status_message_current = wxEmptyString;

    // Take the current time stamp.
    m_status_message_current = wxDateTime::Now().Format(wxT("Time : %Y-%m-%d %H:%M:%S"));

    // Read all settings from the UI.
    wxString arg_service_bus_name;
    wxString arg_event_hub_name;
    wxString arg_shared_access_policy_name;
    wxString arg_shared_access_key;
    wxString arg_user_message;

    wxTextCtrl *text_control;

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_SERVICE_BUS_NAMESPACE), wxTextCtrl);

    if (text_control != NULL) arg_service_bus_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_EVENT_HUB_NAME), wxTextCtrl);

    if (text_control != NULL) arg_event_hub_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_NAME), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_policy_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_KEY), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_key = text_control->GetValue();

    arg_service_bus_name.Trim();
    arg_event_hub_name.Trim();
    arg_shared_access_policy_name.Trim();
    arg_shared_access_key.Trim();

    arg_shared_access_key.Replace(wxT("="), wxT("%3d"));
    arg_shared_access_key.Replace(wxT("/"), wxT("%2f"));
    arg_shared_access_key.Replace(wxT("+"), wxT("%2b"));

    if (arg_service_bus_name == wxEmptyString || arg_event_hub_name == wxEmptyString ||
        arg_shared_access_policy_name == wxEmptyString || arg_shared_access_key == wxEmptyString)
    {
        // Error.
        wxMessageBox("One of the inputs is blank. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
        return;
    }

    m_status_code = 0;

    ////////////////////////////////////////////////////////
    // Setup the URL
    ////////////////////////////////////////////////////////

    // The correct format should be in the following formats.

    // "amqps://super:ujzdCr0Y3eDWJ2d9y7Mc9XdVvSwJ%2Fxx%2FvxEKk3xb5ao%3D@zailorbus.servicebus.windows.net/mydevices"
    // "amqps://super:n9kE5fTGzInxD61xWzPH2u2XsqRBRqKxxxxGnMLhNE%3D@zailorbus.servicebus.windows.net/myqueue"

    std::string amqps_url;

    amqps_url = "amqps://";
    amqps_url += arg_shared_access_policy_name.ToUTF8();
    amqps_url += ":";
    amqps_url += arg_shared_access_key.ToUTF8();
    amqps_url += "@";
    amqps_url += arg_service_bus_name.ToUTF8();
    amqps_url += ".servicebus.windows.net/";
    amqps_url += arg_event_hub_name.ToUTF8();

    m_status_message_current += "\n" + amqps_url;

    wxString str(amqps_url);
    str.Replace(wxT("%"), wxT("%%"));
    str.Truncate(4096);
    wxLogDebug(str);

    ////////////////////////////////////////////////////////
    // Setup the content of the body.
    ////////////////////////////////////////////////////////

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        arg_user_message = text_control->GetValue();

        if (arg_user_message == wxEmptyString)
        {
            wxMessageBox("No data to send. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
            return;
        }

        m_status_message_current += "\n" + arg_user_message;
    }

    std::string ampqs_content;

    ampqs_content = arg_user_message.ToUTF8();

    znSingleton::GetInstance<znThreadSendEventQpid>().SendQpidMessage(amqps_url, ampqs_content);

    wxThreadEvent null_event;
    OnAmqpsSendEvenThreadStatusUpdate(null_event);
}

void znControllerUi::OnBtnAmqpsSetSampleMessage(wxCommandEvent& event)
{
    srand(time(NULL));
    int id = 1 + (rand() % 50);
    int temperature = (rand() % 100);

    wxString value = wxString::Format("{ \"DeviceId\":%02d, \"Temperature\":%02d }", id, temperature);

    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->SetValue(value);
}

void znControllerUi::OnBtnAmqpsClearMessage(wxCommandEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->Clear();
}

void znControllerUi::OnBtnAmqpsClearStatusMessage(wxCommandEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_STATUS_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->Clear();

    m_status_code = 0;
    m_status_message_current.clear();
    m_status_message_accumulated.clear();
}

void znControllerUi::OnAmqpsSendEvenThreadStatusUpdate(wxThreadEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_STATUS_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        // wxString status_message_org = text_control->GetValue();
        int status = 0;

        if (event.GetId() != wxEVT_NULL)
        {
            status = event.GetInt();
        }

        if (status != m_status_code)
        {
            m_status_code = status;

            wxString message = m_status_message_current;

            message += "\nStatus Code = " + wxString::Format("%d", m_status_code);

            if (m_status_message_accumulated != wxEmptyString)
            {
                message += m_status_message_accumulated;
            }

            text_control->SetValue(message);
        }
    }
}

void znControllerUi::OnBtnHmacCopySignature(wxCommandEvent& event)
{
    wxTextCtrl *text_control = NULL;

    if (event.GetId() == ID_ZN_BTN_HMAC_SIGNATURE_1_COPY)
    {
        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1), wxTextCtrl);
    }
    else if (event.GetId() == ID_ZN_BTN_HMAC_SIGNATURE_2_COPY)
    {
        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2), wxTextCtrl);
    }
    else if (event.GetId() == ID_ZN_BTN_HMAC_SIGNATURE_1_AUTHORIZATION_COPY)
    {
        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1_AUTHORIZATION_STRING), wxTextCtrl);
    }
    else if (event.GetId() == ID_ZN_BTN_HMAC_SIGNATURE_2_AUTHORIZATION_COPY)
    {
        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2_AUTHORIZATION_STRING), wxTextCtrl);
    }

    if (text_control == NULL)
    {
        return;
    }

    wxString signature = text_control->GetValue();

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->Clear();
        if (wxTheClipboard->AddData(new wxTextDataObject(signature)) == true)
        {
            // Flush will make the content available to others
            // even when this application has ended.
            wxTheClipboard->Flush();
        }

        wxTheClipboard->Close();
    }
}

void znControllerUi::OnBtnHmacGenerateSignature(wxCommandEvent& event)
{
    // Read all settings from the UI.
    wxString arg_service_bus_name;
    wxString arg_event_hub_name;
    wxString arg_publisher_name;
    wxString arg_shared_access_policy_name;
    wxString arg_shared_access_key;
    wxString arg_ttl;

    wxTextCtrl *text_control;

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SERVICE_BUS_NAMESPACE), wxTextCtrl);

    if (text_control != NULL) arg_service_bus_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_EVENT_HUB_NAME), wxTextCtrl);

    if (text_control != NULL) arg_event_hub_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_PUBLISHER_NAME), wxTextCtrl);

    if (text_control != NULL) arg_publisher_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_NAME), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_policy_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_KEY), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_key = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_TTL), wxTextCtrl);

    if (text_control != NULL) arg_ttl = text_control->GetValue();

    znHmacSignature hmac_results = znUtils::GenerateHmacSignature(arg_service_bus_name, arg_event_hub_name, arg_publisher_name,
        arg_shared_access_policy_name, arg_shared_access_key, wxAtoi(arg_ttl));

    wxStaticText *static_text_control;
    
    static_text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1_INPUT_STRING), wxStaticText);

    if (static_text_control != NULL)
    {
        wxString message;

        message = "Format : " + hmac_results.m_url_1;

        message.Replace("\n", "\\n");

        static_text_control->SetLabel(message);
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->SetValue(hmac_results.m_signature_1);
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1_AUTHORIZATION_STRING), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->SetValue(hmac_results.m_authorization_string_1);
    }

    static_text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2_INPUT_STRING), wxStaticText);

    if (static_text_control != NULL)
    {
        wxString message;

        message = "Format : " + hmac_results.m_url_2;

        message.Replace("\n", "\\n");

        static_text_control->SetLabel(message);
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->SetValue(hmac_results.m_signature_2);
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2_AUTHORIZATION_STRING), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->SetValue(hmac_results.m_authorization_string_2);
    }
}

void znControllerUi::OnBtnHmacClearSignature(wxCommandEvent& event)
{
    wxStaticText *static_text_control;

    static_text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1_INPUT_STRING), wxStaticText);

    if (static_text_control != NULL)
    {
        static_text_control->SetLabel("Format : ");
    }

    static_text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2_INPUT_STRING), wxStaticText);

    if (static_text_control != NULL)
    {
        static_text_control->SetLabel("Format : ");
    }

    wxTextCtrl *text_control;

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->Clear();
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_1_AUTHORIZATION_STRING), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->Clear();
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->Clear();
    }

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HMAC_SIGNATURE_2_AUTHORIZATION_STRING), wxTextCtrl);

    if (text_control != NULL)
    {
        text_control->Clear();
    }
}

wxBEGIN_EVENT_TABLE(znControllerUi, wxEvtHandler)
    EVT_CLOSE(znControllerUi::OnClose)

    EVT_BUTTON(ID_ZN_BTN_HTTPS_SEND_MESSAGE, znControllerUi::OnBtnHttpsSendMessage)
    EVT_BUTTON(ID_ZN_BTN_HTTPS_SET_SAMPLE_MESSAGE, znControllerUi::OnBtnHttpsSetSampleMessage)
    EVT_BUTTON(ID_ZN_BTN_HTTPS_CLEAR_MESSAGE, znControllerUi::OnBtnHttpsClearMessage)
    EVT_BUTTON(ID_ZN_BTN_HTTPS_CLEAR_STATUS_MESSAGE, znControllerUi::OnBtnHttpsClearStatusMessage)

    EVT_BUTTON(ID_ZN_BTN_AMQPS_SEND_MESSAGE, znControllerUi::OnBtnAmqpsSendMessage)
    EVT_BUTTON(ID_ZN_BTN_AMQPS_SET_SAMPLE_MESSAGE, znControllerUi::OnBtnAmqpsSetSampleMessage)
    EVT_BUTTON(ID_ZN_BTN_AMQPS_CLEAR_MESSAGE, znControllerUi::OnBtnAmqpsClearMessage)
    EVT_BUTTON(ID_ZN_BTN_AMQPS_CLEAR_STATUS_MESSAGE, znControllerUi::OnBtnAmqpsClearStatusMessage)

    EVT_THREAD(ID_ZN_EVENT_SEND_EVENT_THREAD_STATUS_UPDATE, znControllerUi::OnAmqpsSendEvenThreadStatusUpdate)

    EVT_BUTTON(ID_ZN_BTN_HMAC_SIGNATURE_1_COPY, znControllerUi::OnBtnHmacCopySignature)
    EVT_BUTTON(ID_ZN_BTN_HMAC_SIGNATURE_2_COPY, znControllerUi::OnBtnHmacCopySignature)
    EVT_BUTTON(ID_ZN_BTN_HMAC_SIGNATURE_1_AUTHORIZATION_COPY, znControllerUi::OnBtnHmacCopySignature)
    EVT_BUTTON(ID_ZN_BTN_HMAC_SIGNATURE_2_AUTHORIZATION_COPY, znControllerUi::OnBtnHmacCopySignature)

    EVT_BUTTON(ID_ZN_BTN_HMAC_SIGNATURE_GENERATE, znControllerUi::OnBtnHmacGenerateSignature)
    EVT_BUTTON(ID_ZN_BTN_HMAC_SIGNATURE_CLEAR, znControllerUi::OnBtnHmacClearSignature)
wxEND_EVENT_TABLE()