#ifndef ZNCONTROLLERUI_H
#define ZNCONTROLLERUI_H

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
    void OnBtnSendMessage(wxCommandEvent& event);
    void OnBtnSetSampleMessage(wxCommandEvent& event);
    void OnBtnClearMessage(wxCommandEvent& event);
    void OnBtnClearStatusMessage(wxCommandEvent& event);

public:
    std::string         m_post_data;

private:
    wxDECLARE_EVENT_TABLE();
};

#endif