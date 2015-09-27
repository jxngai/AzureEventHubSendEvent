#include "znConstants.h"
#include "znControllerUi.h"
#include "znModel.h"

#include <wx/textctrl.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <wx/object.h> 
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

    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL)
    {
        wxString message = text_control->GetValue();
        message.Trim();

        if (message == wxEmptyString)
        {
            wxCommandEvent event;
            OnBtnSetSampleMessage(event);
        }
    }
}

void znControllerUi::InitializeUiControls()
{
    // This routine is meant to be called from znApp after wxFrame has been created.
    // It is to initialized some widgets with user values stored in the ini file.

    int enum_list[] = {
        ID_ZN_TXT_SERVICE_BUS_NAMESPACE,
        ID_ZN_TXT_EVENT_HUB_NAME,
        ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME,
        ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY,
        ID_ZN_TXT_USER_MESSAGE
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

znControllerUi::~znControllerUi()
{
}

void znControllerUi::OnQuit(wxCommandEvent& event)
{
}

void znControllerUi::OnAbout(wxCommandEvent& event)
{
}

void znControllerUi::OnClose(wxCloseEvent& event)
{
    // Save all the user's options into the database.

    int enum_list[] = {
        ID_ZN_TXT_SERVICE_BUS_NAMESPACE,
        ID_ZN_TXT_EVENT_HUB_NAME,
        ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME,
        ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY,
        ID_ZN_TXT_USER_MESSAGE
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

    if (obj->m_post_data.length() > 0)
    {
        *(char *)ptr = obj->m_post_data[0];     /* copy one single byte */
        obj->m_post_data.erase(0, 1);
        return 1;                               /* we return 1 byte at a time! */
    }

    return 0;                                   /* no more data left to deliver */
}

void znControllerUi::OnBtnSendMessage(wxCommandEvent& event)
{
    wxLogDebug("<<< znControllerUi::OnBtnSendMessage >>>");

    // Take the current time stamp.
    wxString status_message = wxDateTime::Now().Format(wxT("Time : %Y-%m-%d %H:%M:%S"));

    // Read all settings from the UI.
    wxString arg_service_bus_name;
    wxString arg_event_hub_name;
    wxString arg_shared_access_policy_name;
    wxString arg_shared_access_key;

    wxTextCtrl *text_control;

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_SERVICE_BUS_NAMESPACE), wxTextCtrl);

    if (text_control != NULL) arg_service_bus_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_EVENT_HUB_NAME), wxTextCtrl);

    if (text_control != NULL) arg_event_hub_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME), wxTextCtrl);

    if (text_control != NULL) arg_shared_access_policy_name = text_control->GetValue();

    text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY), wxTextCtrl);

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

        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_USER_MESSAGE), wxTextCtrl);

        if (text_control != NULL)
        {
            m_post_data = text_control->GetValue();

            if (m_post_data == wxEmptyString)
            {
                wxMessageBox("No data to send. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
                return;
            }

            status_message += "\n" + m_post_data;
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

        text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_STATUS_MESSAGE), wxTextCtrl);

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

    // Will only reach here after all the data in m_post_data has been sent.
    curl_global_cleanup();
}

void znControllerUi::OnBtnSetSampleMessage(wxCommandEvent& event)
{
    srand(time(NULL));
    int id = 1 + (rand() % 50);
    int temperature = (rand() % 100);

    wxString value = wxString::Format("{ \"DeviceId\":%02d, \"Temperature\":%02d }", id, temperature);

    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->SetValue(value);
}

void znControllerUi::OnBtnClearMessage(wxCommandEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_USER_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->Clear();
}

void znControllerUi::OnBtnClearStatusMessage(wxCommandEvent& event)
{
    wxTextCtrl* text_control = wxDynamicCast(wxWindow::FindWindowById(ID_ZN_TXT_STATUS_MESSAGE), wxTextCtrl);

    if (text_control != NULL) text_control->Clear();
}

wxBEGIN_EVENT_TABLE(znControllerUi, wxEvtHandler)
    EVT_MENU(ID_ZN_QUIT, znControllerUi::OnQuit)
    EVT_MENU(ID_ZN_ABOUT, znControllerUi::OnAbout)
    EVT_CLOSE(znControllerUi::OnClose)

    EVT_BUTTON(ID_ZN_BTN_SEND_MESSAGE, znControllerUi::OnBtnSendMessage)
    EVT_BUTTON(ID_ZN_BTN_SET_SAMPLE_MESSAGE, znControllerUi::OnBtnSetSampleMessage)
    EVT_BUTTON(ID_ZN_BTN_CLEAR_MESSAGE, znControllerUi::OnBtnClearMessage)
    EVT_BUTTON(ID_ZN_BTN_CLEAR_STATUS_MESSAGE, znControllerUi::OnBtnClearStatusMessage)
wxEND_EVENT_TABLE()