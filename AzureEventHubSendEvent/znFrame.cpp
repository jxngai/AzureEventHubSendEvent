#include "znConstants.h"
#include "znFrame.h"
#include "znControllerUi.h"

#include <wx/log.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/icon.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

znFrame::znFrame()
    : wxFrame(NULL, ID_ZN_FRAME, ZN_APP_TITLE)
{
    // SetWindowStyle(GetWindowStyle() & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	
    ////////////////////////////////////////////////////////
    // Status Bar
    ////////////////////////////////////////////////////////
	
    CreateStatusBar();
    SetStatusText(ZN_APP_TITLE);
	
    ////////////////////////////////////////////////////////
    // Panel
    ////////////////////////////////////////////////////////
	
    wxPanel *panel = new wxPanel(this, ID_ZN_PANEL);

	////////////////////////////////////////////////////////
    // Sizer
    ////////////////////////////////////////////////////////
	
    wxBoxSizer *top_sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(top_sizer);

    ////////////////////////////////////////////////////////
    // Common Assets
    ////////////////////////////////////////////////////////

    wxStaticBoxSizer * static_box_sizer;
    wxBoxSizer *row_sizer_inner;
    wxBoxSizer *row_sizer_outter;

    wxBoxSizer *col_sizer_inner;

    row_sizer_outter = new wxBoxSizer(wxHORIZONTAL);

    ////////////////////////////////////////////////////////
    // Event Hub Settings
    ////////////////////////////////////////////////////////
    
    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Event Hub")), wxVERTICAL);

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Service Bus Namespace")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_SERVICE_BUS_NAMESPACE), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Event Hub Name")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_EVENT_HUB_NAME), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_outter->Add(
        static_box_sizer,
        1,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        0);                                              // The surrounding padding. Only effective with wxALL present.

    row_sizer_outter->AddSpacer(4);

    ////////////////////////////////////////////////////////
    // Credential Settings
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Credential")), wxVERTICAL);

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Shared Access Policy Name")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_SHARED_ACCESS_POLICY_NAME), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Shared Access Policy Key")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_SHARED_ACCESS_POLICY_KEY), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_outter->Add(
        static_box_sizer,
        1,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        0);                                                 // The surrounding padding. Only effective with wxALL present.

    top_sizer->Add(
        row_sizer_outter,
        0,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);                                                 // The surrounding padding. Only effective with wxALL present.

    ////////////////////////////////////////////////////////
    // Message Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Message")), wxHORIZONTAL);

    static_box_sizer->Add(
        new wxTextCtrl(panel, ID_ZN_TXT_USER_MESSAGE, wxEmptyString, wxDefaultPosition, wxSize(420, 60), wxTE_MULTILINE),
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_SEND_MESSAGE, wxT("Send Message")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_SET_SAMPLE_MESSAGE, wxT("Sample Data")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_CLEAR_MESSAGE, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    static_box_sizer->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    top_sizer->Add(
        static_box_sizer,
        3,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    ////////////////////////////////////////////////////////
    // Status Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Status")), wxHORIZONTAL);

    static_box_sizer->Add(
        new wxTextCtrl(panel, ID_ZN_TXT_STATUS_MESSAGE, wxEmptyString, wxDefaultPosition, wxSize(420, 60), wxTE_MULTILINE),
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_CLEAR_STATUS_MESSAGE, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    static_box_sizer->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    top_sizer->Add(
        static_box_sizer,
        4,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    ////////////////////////////////////////////////////////
    // Bind all UI controls with panel sizer.
    ////////////////////////////////////////////////////////

	top_sizer->SetSizeHints(this);
	
    ////////////////////////////////////////////////////////
    // UI Initializations
    ////////////////////////////////////////////////////////
    SetIcon(wxIcon(wxT("ID_ZN_RC_ICON_01"), wxBITMAP_TYPE_ICO_RESOURCE));
	
    ////////////////////////////////////////////////////////
    // Data Initializations.
    ////////////////////////////////////////////////////////
    // MVC separation.
    // Add znControllerUi to handle all UI events.

    PushEventHandler(&znSingleton::GetInstance<znControllerUi>());
}

znFrame::~znFrame()
{
    wxLogDebug(wxT("<<< znFrame::~znFrame() >>>"));

    PopEventHandler();
}