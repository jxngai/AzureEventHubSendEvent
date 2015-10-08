#include "znUtils.h"
#include "znConstants.h"

#include <wx/base64.h>
#include <wx/msgdlg.h>
#include <wx/log.h>

#include <openssl/hmac.h>
#include <curl/curl.h>

std::string znUtils::m_curl_post_data;

wxString znUtils::UrlEncode(wxString input)
{
    wxString result;

    for (unsigned int ix = 0; ix < input.Length(); ix++)
    {
        if (isalnum(input[ix]) != 0 || input[ix] == '-' || input[ix] == '_' || input[ix] == '.' || input[ix] == '~')
        {
            result += input[ix];
        }
        else
        {
            // Note that the %% is to print a % character.
            result += wxString::Format("%%%02x", input[ix]);
        }
    }

    return result;
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
    // The function will send the HTTP Post content to the remote web server
    // one byte by one byte.

    std::string *data = (std::string *)userp;

    if (size*nmemb < 1)
        return 0;

    if (data->length() > 0)
    {
        *(char *)ptr = (*data)[0];              /* copy one single byte */
        data->erase(0, 1);
        return 1;                               /* we return 1 byte at a time! */
    }

    return 0;                                   /* no more data left to deliver */
}

int znUtils::CurlPostHttps(wxString params_url, wxString params_header, wxString params_post_data)
{
    // Use Curl Library to post the HTTPS request.

    long http_code = 0;

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
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &znUtils::m_curl_post_data);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlReadCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &znUtils::m_curl_post_data);

        ////////////////////////////////////////////////////////
        // Setup the URL.
        ////////////////////////////////////////////////////////

        // Somehow without (const char*) casting, it does not work.
        // Causing CURLE_COULDNT_RESOLVE_HOST issue.

        res = curl_easy_setopt(curl, CURLOPT_URL, (const char*) params_url.c_str());

        ////////////////////////////////////////////////////////
        // Setup the headers.
        ////////////////////////////////////////////////////////

        struct curl_slist *chunk = NULL;

        chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");

        chunk = curl_slist_append(chunk, params_header.c_str());

        std::string params_content_type("Content-Type: application/atom+xml;type=entry;charset=utf-8");

        chunk = curl_slist_append(chunk, params_content_type.c_str());

        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        ////////////////////////////////////////////////////////
        // Setup the content of the body.
        ////////////////////////////////////////////////////////

        // The content will be sent by function CurlReadCallback() one byte at a time.

        m_curl_post_data = params_post_data;

        ////////////////////////////////////////////////////////
        // Finaly submit the request to the target URL.
        ////////////////////////////////////////////////////////

        curl_easy_setopt(curl, CURLOPT_POST, 1);
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            // CURLE_UNSUPPORTED_PROTOCOL
            // CURLE_COULDNT_RESOLVE_HOST

            wxLogDebug(curl_easy_strerror(res));
        }

        // Get the HTTP Status code.

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
    }

    // Will only reach here after all the data in m_curl_post_data has been sent.
    curl_global_cleanup();

    return http_code;
}

znHmacSignature znUtils::GenerateHmacSignature(wxString service_bus_name, wxString event_hub_name,
    wxString publisher_name, wxString shared_access_policy_name, wxString shared_access_key, int ttl)
{
    // This routine will generate 2 types of HMAC SHA256 signatures in 2 different formats.

    // Input data sanitization.

    service_bus_name.Trim().Trim(false);
    event_hub_name.Trim().Trim(false);
    publisher_name.Trim().Trim(false);
    shared_access_policy_name.Trim().Trim(false);
    shared_access_key.Trim().Trim(false);

    // Input data validity checks.

    if (service_bus_name == wxEmptyString || event_hub_name == wxEmptyString ||
        shared_access_policy_name == wxEmptyString || shared_access_key == wxEmptyString)
    {
        // Error.
        wxMessageBox("One of the inputs is blank. Aborting !", ZN_APP_TITLE, wxOK | wxICON_INFORMATION, NULL);
        return{ -1, -1 };
    }

    ////////////////////////////////////////////////////////
    // Get the current UTF time in seconds.
    ////////////////////////////////////////////////////////

    // Add a time offset so that the HMAC SHA256 signature will expire in near future
    // 1 minute later in this case

    wxDateTime time_0 = wxDateTime::UNow() + wxTimeSpan::Seconds(ttl);

    time_t params_utc_time = time_0.GetTicks();

    // For debugging. With time set at 1443266928, output should be
    // "r6UIOLS7t31sEDwFuJcIH9FO0mbPcNpdtKZaH2lC7uU%3d"
    //
    // params_utc_time = 1443266928;

    ////////////////////////////////////////////////////////
    // Construct the HMAC SHA256 signature - Format 1.
    ////////////////////////////////////////////////////////

    wxString params_url_1 = service_bus_name + ".servicebus.windows.net";

    wxString final_signature_1;

    {
        // The input needed for HMAC signature calculations are
        //
        //  - ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY
        //  - UTC time

        std::string params_key_std = shared_access_key.ToUTF8();
        std::string params_data_std = znUtils::UrlEncode(params_url_1) + wxString::Format("\n%d", (int)params_utc_time);

        // Initialize HMAC object.
        HMAC_CTX ctx;
        HMAC_CTX_init(&ctx);

        // Set HMAC key.

        HMAC_Init_ex(&ctx, params_key_std.c_str(), params_key_std.length(), EVP_sha256(), NULL);

        HMAC_Update(&ctx, (unsigned char*)params_data_std.c_str(), params_data_std.length());

        // Finish HMAC computation and fetch result.
        unsigned char hmac_result[1024 * 10];
        unsigned int hmac_result_len;

        int rvalue = HMAC_Final(&ctx, hmac_result, &hmac_result_len);

        // Done with HMAC object.
        HMAC_CTX_cleanup(&ctx);

        // Encode HMAC result with Base64.

        final_signature_1 = wxBase64Encode(hmac_result, hmac_result_len);
    }

    // Encode the result with URL formatting.

    final_signature_1.Replace(wxT("="), wxT("%3d"));
    final_signature_1.Replace(wxT("/"), wxT("%2f"));
    final_signature_1.Replace(wxT("+"), wxT("%2b"));

    ////////////////////////////////////////////////////////
    // Construct Authorization String
    ////////////////////////////////////////////////////////

    // Construct an Authorization string with the calculated HMAC SHA256 signature.
    // A valid Authorization string looks like this.
    //
    // "Authorization: SharedAccessSignature sr=zailorbus.servicebus.windows.net&
    //  sig=52HQdj3Z6HI3n0T93HkbSlZXLKOCMs4URQb5cxQE2Bo%3d&se=1443339673&skn=super"

    wxString final_authorization_1;

    final_authorization_1 = "Authorization: SharedAccessSignature ";
    final_authorization_1 += "sr=" + znUtils::UrlEncode(params_url_1);
    final_authorization_1 += "&sig=" + final_signature_1;
    final_authorization_1 += "&se=" + wxString::Format("%d", (int)params_utc_time);
    final_authorization_1 += "&skn=" + shared_access_policy_name;

    ////////////////////////////////////////////////////////
    // Construct the HMAC SHA256 signature - Format 2.
    ////////////////////////////////////////////////////////

    wxString params_url_2 = "https://" + service_bus_name + ".servicebus.windows.net/" +
        event_hub_name + "/publishers/" + publisher_name + "/messages";

    wxString final_signature_2;

    {
        // The input needed for HMAC signature calculations are
        //
        //  - ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY
        //  - UTC time

        std::string params_key_std = shared_access_key.ToUTF8();
        std::string params_data_std = znUtils::UrlEncode(params_url_2) + wxString::Format("\n%d", (int)params_utc_time);

        // Initialize HMAC object.
        HMAC_CTX ctx;
        HMAC_CTX_init(&ctx);

        // Set HMAC key.

        HMAC_Init_ex(&ctx, params_key_std.c_str(), params_key_std.length(), EVP_sha256(), NULL);

        HMAC_Update(&ctx, (unsigned char*)params_data_std.c_str(), params_data_std.length());

        // Finish HMAC computation and fetch result.
        unsigned char hmac_result[1024 * 10];
        unsigned int hmac_result_len;

        int rvalue = HMAC_Final(&ctx, hmac_result, &hmac_result_len);

        // Done with HMAC object.
        HMAC_CTX_cleanup(&ctx);

        // Encode HMAC result with Base64.

        final_signature_2 = wxBase64Encode(hmac_result, hmac_result_len);
    }

    // Encode the result with URL formatting.

    final_signature_2.Replace(wxT("="), wxT("%3d"));
    final_signature_2.Replace(wxT("/"), wxT("%2f"));
    final_signature_2.Replace(wxT("+"), wxT("%2b"));

    ////////////////////////////////////////////////////////
    // Construct Authorization String
    ////////////////////////////////////////////////////////

    wxString final_authorization_2;

    final_authorization_2 = "Authorization: SharedAccessSignature ";
    final_authorization_2 += "sr=" + znUtils::UrlEncode(params_url_2);
    final_authorization_2 += "&sig=" + final_signature_2;
    final_authorization_2 += "&se=" + wxString::Format("%d", (int)params_utc_time);
    final_authorization_2 += "&skn=" + shared_access_policy_name;

    return{ 0, params_utc_time,
        params_url_1, final_signature_1, final_authorization_1,
        params_url_2, final_signature_2, final_authorization_2 };
}