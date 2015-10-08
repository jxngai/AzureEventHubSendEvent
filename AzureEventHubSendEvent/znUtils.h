#ifndef ZNUTILS_H
#define ZNUTILS_H

#include <wx/string.h>

class znHmacSignature
{
public:
    int             m_error;
    int             m_utc_time;

    wxString        m_url_1;
    wxString        m_signature_1;
    wxString        m_authorization_string_1;

    wxString        m_url_2;
    wxString        m_signature_2;
    wxString        m_authorization_string_2;

};

class znUtils
{
public:
    static wxString UrlEncode(wxString);
    static int CurlPostHttps(wxString, wxString, wxString);
    static znHmacSignature GenerateHmacSignature(wxString, wxString, wxString, wxString, wxString, int);

public:
    static std::string         m_curl_post_data;
};

#endif