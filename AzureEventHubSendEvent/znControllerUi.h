#ifndef ZNCONTROLLERUI_H
#define ZNCONTROLLERUI_H

// #define HAVE_SSIZE_T
// #define PN_NODEFINE_SSIZE_T

#include <proton/message.h>
#include <proton/messenger.h>
#include <proton/error.h>

#include "znSingleton.h"

#include <wx/defs.h>
#include <wx/event.h>

class znControllerUi : public wxEvtHandler, public znSingleton
{
private:
    friend znSingleton;

    znControllerUi();
    wxDECLARE_NO_COPY_CLASS(znControllerUi);
    ~znControllerUi();

public:
    void InitializeUiControls();

    void SetStatusText(wxString);

private:
    void OnClose(wxCloseEvent& event);

    void OnBtnHttpsSendMessage(wxCommandEvent& event);
    void OnBtnHttpsSetSampleMessage(wxCommandEvent& event);
    void OnBtnHttpsClearMessage(wxCommandEvent& event);
    void OnBtnHttpsClearStatusMessage(wxCommandEvent& event);

    void OnBtnAmqpsSendMessage(wxCommandEvent& event);
    void OnBtnAmqpsSetSampleMessage(wxCommandEvent& event);
    void OnBtnAmqpsClearMessage(wxCommandEvent& event);
    void OnBtnAmqpsClearStatusMessage(wxCommandEvent& event);

    void OnAmqpsSendEvenThreadStatusUpdate(wxThreadEvent& event);

public:
    std::string         m_curl_post_data;

    int                 m_status_code;
    wxString            m_status_message_current;
    wxString            m_status_message_accumulated;

private:
    wxDECLARE_EVENT_TABLE();
};

#endif