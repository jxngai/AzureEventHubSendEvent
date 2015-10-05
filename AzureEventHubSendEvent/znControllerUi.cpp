#include "common.h"
#include "znControllerUi.h"
#include "znConstants.h"
#include "znModel.h"
#include "znThreadSendEventQpid.h"

#include <wx/textctrl.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/base64.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/msgdlg.h>

#include <openssl/hmac.h>
#include <curl/curl.h>

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
    // This routine is meant to be called from znApp after wxFrame has been created.
    // It is to initialize some widgets with user values stored in the ini file.

    int enum_list[] = {
        ID_ZN_TXT_HTTPS_SERVICE_BUS_NAMESPACE,
        ID_ZN_TXT_HTTPS_EVENT_HUB_NAME,
        ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_NAME,
        ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_KEY,
        ID_ZN_TXT_HTTPS_USER_MESSAGE,
        ID_ZN_TXT_AMQPS_SERVICE_BUS_NAMESPACE,
        ID_ZN_TXT_AMQPS_EVENT_HUB_NAME,
        ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_NAME,
        ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_KEY,
        ID_ZN_TXT_AMQPS_USER_MESSAGE
    };

    for (int ix = 0; ix < sizeof(enum_list) / sizeof(int); ix++)
    {
        wxWindow *window = wxWindow::FindWindowById(enum_list[ix]);

        if (window == NULL) continue;

        wxString value = znSingleton::GetInstance<znModel>().GetUserOption(enum_list[ix]).c_str();

        wxCheckBox * checkbox = wxDynamicCast(window, wxCheckBox);
        wxTextCtrl * textctrl = wxDynamicCast(window, wxTextCtrl);
        wxChoice * choice = wxDynamicCast(window, wxChoice);

        if (checkbox != NULL)
        {
            if (value == wxT("yes"))
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
            textctrl->SetValue(value);
        }
        else if (choice != NULL)
        {
            choice->SetStringSelection(value);
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
    // Save all the user's options into the database.

    int enum_list[] = {
        ID_ZN_TXT_HTTPS_SERVICE_BUS_NAMESPACE,
        ID_ZN_TXT_HTTPS_EVENT_HUB_NAME,
        ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_NAME,
        ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_KEY,
        ID_ZN_TXT_HTTPS_USER_MESSAGE,
        ID_ZN_TXT_AMQPS_SERVICE_BUS_NAMESPACE,
        ID_ZN_TXT_AMQPS_EVENT_HUB_NAME,
        ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_NAME,
        ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_KEY,
        ID_ZN_TXT_AMQPS_USER_MESSAGE
    };

    for (int ix = 0; ix < sizeof(enum_list) / sizeof(int); ix++)
    {
        wxWindow *window = wxWindow::FindWindowById(enum_list[ix]);

        if (window != NULL)
        {
            wxString value;

            wxCheckBox * checkbox = wxDynamicCast(window, wxCheckBox);
            wxTextCtrl * textctrl = wxDynamicCast(window, wxTextCtrl);
            wxChoice * choice = wxDynamicCast(window, wxChoice);

            if (checkbox != NULL)
            {
                if (checkbox->GetValue() == true)
                {
                    value = wxT("yes");
                    znSingleton::GetInstance<znModel>().SetUserOption(enum_list[ix], value);
                }
                else
                {
                    value = wxT("no");
                    znSingleton::GetInstance<znModel>().SetUserOption(enum_list[ix], value);
                }
            }
            else if (textctrl != NULL)
            {
                value = textctrl->GetValue();
                znSingleton::GetInstance<znModel>().SetUserOption(enum_list[ix], value);
            }
            else if (choice != NULL)
            {
                value = choice->GetStringSelection();
                znSingleton::GetInstance<znModel>().SetUserOption(enum_list[ix], value);
            }
        }
    }

    // Need to Skip() so that the event can continue to be processed.
    event.Skip();
}

static size_t CurlWriteCallback(char* buf, size_t size, size_t nmemb, void* userp)
{
    // TRACE("CURL - Response received:\n%s", buf);
    // TRACE("CURL - Response handled %d bytes:\n%s", size*nmemb);

    // tell curl how many bytes we handled
    return size*nmemb;
}

static size_t CurlReadCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    znControllerUi *obj = (znControllerUi *)userp;

    if (size*nmemb < 1)
        return 0;

    if (obj->m_curl_post_data.length() > 0)
    {
        *(char *)ptr = obj->m_curl_post_data[0];     /* copy one single byte */
        obj->m_curl_post_data.erase(0, 1);
        return 1;                               /* we return 1 byte at a time! */
    }

    return 0;                                   /* no more data left to deliver */
}

void znControllerUi::OnBtnHttpsSendMessage(wxCommandEvent& event)
{
    wxLogDebug("<<< znControllerUi::OnBtnHttpsSendMessage >>>");

    // Take the current time stamp.
    wxString status_message = wxDateTime::Now().Format(wxT("Time : %Y-%m-%d %H:%M:%S"));

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

    arg_service_bus_name.Trim();
    arg_event_hub_name.Trim();
    arg_shared_access_policy_name.Trim();
    arg_shared_access_key.Trim();

    if (arg_service_bus_name == wxEmptyString || arg_event_hub_name == wxEmptyString ||
        arg_shared_access_policy_name == wxEmptyString || arg_shared_access_key == wxEmptyString)
    {
        // Error.
        wxMessageBox("One of the inputs is blank. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
        return;
    }

    ////////////////////////////////////////////////////////
    // Get the current UTF time in seconds.
    ////////////////////////////////////////////////////////

    // Add a time offset so that the HMAC SHA256 signature will expire in near future
    // 1 minute later in this case

    // wxDateTime time_0 = wxDateTime::UNow() + wxTimeSpan::Seconds(60 * 60 * 24 * 365 * 2);
    wxDateTime time_0 = wxDateTime::UNow() + wxTimeSpan::Seconds(60 * 1);

    time_t params_seconds = time_0.GetTicks();

    ////////////////////////////////////////////////////////
    // Construct the HMAC SHA256 signature.
    ////////////////////////////////////////////////////////

    // The input needed are

    //  - ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY
    //  - UTC time

    std::string hmac_input_key = arg_shared_access_key.mb_str(wxMBConvUTF8());
    std::string hmac_input_data = std::string(arg_service_bus_name.mb_str(wxMBConvUTF8())) + ".servicebus.windows.net\n" + std::to_string(params_seconds);

    // Initialize HMAC object.
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);

    // Set HMAC key.
    HMAC_Init_ex(&ctx, hmac_input_key.c_str(), hmac_input_key.length(), EVP_sha256(), NULL);

    HMAC_Update(&ctx, (unsigned char*)hmac_input_data.c_str(), hmac_input_data.length());

    // Finish HMAC computation and fetch result.
    unsigned char hmac_result[1024 * 10];
    unsigned int hmac_result_len;

    int rvalue = HMAC_Final(&ctx, hmac_result, &hmac_result_len);

    // Done with HMAC object.
    HMAC_CTX_cleanup(&ctx);

    // Encode HMAC result with Base64.

    wxString base64_string = wxBase64Encode(hmac_result, hmac_result_len);

    // Encode the result with URL formatting.

    base64_string.Replace(wxT("="), wxT("%3d"));
    base64_string.Replace(wxT("/"), wxT("%2f"));
    base64_string.Replace(wxT("+"), wxT("%2b"));

    // The final signature is stored in "params_signature".
    // A valid signature looks something like this.
    // "52HQdj3Z6HI3n0T93HkbSlZXLKOCMs4URQb5cxQE2Bo%3d"

    std::string params_signature = base64_string.mb_str(wxMBConvUTF8());

    // Construct an Authorization string with the calculated HMAC SHA256 signature.
    // A valid Authorization string looks like this.
    // "Authorization: SharedAccessSignature sr=zailorbus.servicebus.windows.net&
    //  sig=52HQdj3Z6HI3n0T93HkbSlZXLKOCMs4URQb5cxQE2Bo%3d&se=1443339673&skn=super"

    std::string params_authorization;

    params_authorization = "Authorization: SharedAccessSignature sr=";
    params_authorization += arg_service_bus_name.mb_str(wxMBConvUTF8());
    params_authorization += ".servicebus.windows.net&sig=";
    params_authorization += params_signature;
    params_authorization += "&se=" + std::to_string(params_seconds);
    params_authorization += "&skn=";
    params_authorization += arg_shared_access_policy_name.mb_str(wxMBConvUTF8());

    // Now use Curl Library to post the HTTPS request.

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        ////////////////////////////////////////////////////////
        // Setup the read and write call back functions.
        ////////////////////////////////////////////////////////

        // Pass this object to the read and write call back functions.

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlReadCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, this);

        ////////////////////////////////////////////////////////
        // Setup the target URL.
        ////////////////////////////////////////////////////////

        // The target URL is of the form of
        //     https://zailorbus.servicebus.windows.net/mydevices/messages

        std::string params_url = "https://";
        params_url += arg_service_bus_name.mb_str(wxMBConvUTF8());
        params_url += ".servicebus.windows.net/";
        params_url += arg_event_hub_name.mb_str(wxMBConvUTF8());
        params_url += "/messages";

        status_message += "\nURL : " + params_url;

        res = curl_easy_setopt(curl, CURLOPT_URL, params_url.c_str());

        ////////////////////////////////////////////////////////
        // Setup the headers.
        ////////////////////////////////////////////////////////

        struct curl_slist *chunk = NULL;

        chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");

        status_message += "\n" + params_authorization;

        chunk = curl_slist_append(chunk, params_authorization.c_str());

        std::string params_content_type("Content-Type:application/atom+xml;type=entry;charset=utf-8");

        status_message += "\n" + params_content_type;

        chunk = curl_slist_append(chunk, params_content_type.c_str());

        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        ////////////////////////////////////////////////////////
        // Setup the content of the body.
        ////////////////////////////////////////////////////////

        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_USER_MESSAGE), wxTextCtrl);

        if (text_control != NULL)
        {
            m_curl_post_data = text_control->GetValue();

            if (m_curl_post_data == wxEmptyString)
            {
                wxMessageBox("No data to send. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
                return;
            }

            status_message += "\n" + m_curl_post_data;
        }

        ////////////////////////////////////////////////////////
        // Finaly submit the request to the target URL.
        ////////////////////////////////////////////////////////

        curl_easy_setopt(curl, CURLOPT_POST, 1);

        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            // CURLE_UNSUPPORTED_PROTOCOL;

            wxLogDebug(curl_easy_strerror(res));
        }

        // Get the HTTP Status code.

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        status_message += "\nStatus Code : " + std::to_string(http_code);

        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_HTTPS_STATUS_MESSAGE), wxTextCtrl);

        if (text_control != NULL)
        {
            wxString status_message_org = text_control->GetValue();

            if (status_message_org != wxEmptyString)
            {
                status_message += "\n+-------------------------------------------+\n";
                status_message += status_message_org;
            }

            text_control->SetValue(status_message);
        }

        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
    }

    // Will only reach here after all the data in m_curl_post_data has been sent.
    curl_global_cleanup();
}

void znControllerUi::OnBtnHttpsSetSampleMessage(wxCommandEvent& event)
{
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

    std::string amqps_url;

    amqps_url = "amqps://" + arg_shared_access_policy_name + ":";
    amqps_url += arg_shared_access_key + "@";
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

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_AMQPS_STATUS_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        // wxString status_message_org = text_control->GetValue();

        wxString message = m_status_message_current;

        message += "\nStatus Code = " + wxString::Format("%d", m_status_code);

        if (m_status_message_accumulated != wxEmptyString)
        {
            message += m_status_message_accumulated;
        }

        text_control->SetValue(message);
    }
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

        int status = event.GetInt();

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
wxEND_EVENT_TABLE()