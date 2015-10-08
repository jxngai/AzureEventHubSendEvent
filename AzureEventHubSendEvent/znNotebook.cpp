#include "znFrame.h"
#include "znConstants.h"
#include "znNotebook.h"

#include <wx/window.h>
#include <wx/statbox.h>
#include <wx/hyperlink.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

znNotebook::znNotebook(znFrame *const frame, wxPanel* panel)
    : wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize), m_frame(frame)
{
    ConstructTab_HTTPS();
    ConstructTab_AMQPS();
    ConstructTab_Hmac_Generator();
    ConstructTab_About();

    SetSelection(0);
}

void znNotebook::ConstructTab_HTTPS()
{
    // wxWindow *parent;

    ////////////////////////////////////////////////////////
    // Panel
    ////////////////////////////////////////////////////////

    wxPanel* panel = new wxPanel(this);
    AddPage(panel, wxT("HTTPS Event"), true);

    ////////////////////////////////////////////////////////
    // Sizer
    ////////////////////////////////////////////////////////

    wxBoxSizer *panel_sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panel_sizer);

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
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HTTPS_SERVICE_BUS_NAMESPACE), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Event Hub Name")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HTTPS_EVENT_HUB_NAME), 1, wxALL | wxEXPAND, 2);

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
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_NAME), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Shared Access Policy Key")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_KEY), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_outter->Add(
        static_box_sizer,
        1,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        0);                                                 // The surrounding padding. Only effective with wxALL present.

    panel_sizer->Add(
        row_sizer_outter,
        0,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);                                                 // The surrounding padding. Only effective with wxALL present.

    ////////////////////////////////////////////////////////
    // Message Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Message")), wxHORIZONTAL);

    static_box_sizer->Add(
        new wxTextCtrl(panel, ID_ZN_TXT_HTTPS_USER_MESSAGE, wxEmptyString, wxDefaultPosition, wxSize(420, 60), wxTE_MULTILINE),
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HTTPS_SEND_MESSAGE, wxT("Send Message")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HTTPS_SET_SAMPLE_MESSAGE, wxT("Sample Data")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HTTPS_CLEAR_MESSAGE, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    static_box_sizer->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    panel_sizer->Add(
        static_box_sizer,
        3,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    ////////////////////////////////////////////////////////
    // Status Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Status")), wxHORIZONTAL);

    static_box_sizer->Add(
        new wxTextCtrl(panel, ID_ZN_TXT_HTTPS_STATUS_MESSAGE, wxEmptyString, wxDefaultPosition, wxSize(420, 60), wxTE_MULTILINE),
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HTTPS_CLEAR_STATUS_MESSAGE, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    static_box_sizer->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    panel_sizer->Add(
        static_box_sizer,
        4,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    panel->Layout();
}

void znNotebook::ConstructTab_AMQPS()
{
    // wxWindow *parent;

    ////////////////////////////////////////////////////////
    // Panel
    ////////////////////////////////////////////////////////

    wxPanel* panel = new wxPanel(this);
    AddPage(panel, wxT("AMQPS Event"), true);

    ////////////////////////////////////////////////////////
    // Sizer
    ////////////////////////////////////////////////////////

    wxBoxSizer *panel_sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panel_sizer);

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
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_AMQPS_SERVICE_BUS_NAMESPACE), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Event Hub Name")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_AMQPS_EVENT_HUB_NAME), 1, wxALL | wxEXPAND, 2);

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
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_NAME), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Shared Access Policy Key")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_KEY), 1, wxALL | wxEXPAND, 2);

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

    panel_sizer->Add(
        row_sizer_outter,
        0,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);                                                 // The surrounding padding. Only effective with wxALL present.

    ////////////////////////////////////////////////////////
    // Message Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Message")), wxHORIZONTAL);

    static_box_sizer->Add(
        new wxTextCtrl(panel, ID_ZN_TXT_AMQPS_USER_MESSAGE, wxEmptyString, wxDefaultPosition, wxSize(420, 60), wxTE_MULTILINE),
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_AMQPS_SEND_MESSAGE, wxT("Send Message")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_AMQPS_SET_SAMPLE_MESSAGE, wxT("Sample Data")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_AMQPS_CLEAR_MESSAGE, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    static_box_sizer->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    panel_sizer->Add(
        static_box_sizer,
        3,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    ////////////////////////////////////////////////////////
    // Status Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Status")), wxHORIZONTAL);

    static_box_sizer->Add(
        new wxTextCtrl(panel, ID_ZN_TXT_AMQPS_STATUS_MESSAGE, wxEmptyString, wxDefaultPosition, wxSize(420, 60), wxTE_MULTILINE),
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_AMQPS_CLEAR_STATUS_MESSAGE, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    static_box_sizer->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    panel_sizer->Add(
        static_box_sizer,
        4,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    panel->Layout();
}

void znNotebook::ConstructTab_Hmac_Generator()
{
    // wxWindow *parent;

    ////////////////////////////////////////////////////////
    // Panel
    ////////////////////////////////////////////////////////

    wxPanel* panel = new wxPanel(this);
    AddPage(panel, wxT("HMAC Generator"), true);

    ////////////////////////////////////////////////////////
    // Sizer
    ////////////////////////////////////////////////////////

    wxBoxSizer *panel_sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panel_sizer);

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
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SERVICE_BUS_NAMESPACE), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Event Hub Name")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HMAC_EVENT_HUB_NAME), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Publisher")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HMAC_PUBLISHER_NAME), 1, wxALL | wxEXPAND, 2);

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
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_NAME), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("Shared Access Policy Key")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_KEY), 1, wxALL | wxEXPAND, 2);


    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(new wxStaticText(panel, wxID_ANY, wxT("TTL in Seconds")), 1, wxALL | wxEXPAND, 2);
    row_sizer_inner->Add(new wxTextCtrl(panel, ID_ZN_TXT_HMAC_TTL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>()), 1, wxALL | wxEXPAND, 2);

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_outter->Add(
        static_box_sizer,
        1,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        0);                                                 // The surrounding padding. Only effective with wxALL present.

    panel_sizer->Add(
        row_sizer_outter,
        0,                                                  // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);                                                 // The surrounding padding. Only effective with wxALL present.

    ////////////////////////////////////////////////////////
    // HMAC Signature 1 Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("HMAC Signature 1")), wxVERTICAL);

    static_box_sizer->Add(new wxStaticText(panel, ID_ZN_TXT_HMAC_SIGNATURE_1_INPUT_STRING, wxT("Format : ")), 0, wxLEFT | wxRIGHT | wxEXPAND, 12);

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    wxTextCtrl *text_control = new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SIGNATURE_1);
    text_control->SetEditable(false);

    row_sizer_inner->Add(
        text_control,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HMAC_SIGNATURE_1_COPY, wxT("Copy")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    row_sizer_inner->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    text_control = new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SIGNATURE_1_AUTHORIZATION_STRING);
    text_control->SetEditable(false);

    row_sizer_inner->Add(
        text_control,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HMAC_SIGNATURE_1_AUTHORIZATION_COPY, wxT("Copy")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    row_sizer_inner->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    panel_sizer->Add(
        static_box_sizer,
        0,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxLEFT | wxRIGHT,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    ////////////////////////////////////////////////////////
    // HMAC Signature 2 Container
    ////////////////////////////////////////////////////////

    static_box_sizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("HMAC Signature 2")), wxVERTICAL);

    static_box_sizer->Add(new wxStaticText(panel, ID_ZN_TXT_HMAC_SIGNATURE_2_INPUT_STRING, wxT("Format : ")), 0, wxLEFT | wxRIGHT | wxEXPAND, 12);

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    text_control = new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SIGNATURE_2);
    text_control->SetEditable(false);

    row_sizer_inner->Add(
        text_control,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HMAC_SIGNATURE_2_COPY, wxT("Copy")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    row_sizer_inner->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    text_control = new wxTextCtrl(panel, ID_ZN_TXT_HMAC_SIGNATURE_2_AUTHORIZATION_STRING);
    text_control->SetEditable(false);

    row_sizer_inner->Add(
        text_control,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    col_sizer_inner = new wxBoxSizer(wxVERTICAL);

    col_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HMAC_SIGNATURE_2_AUTHORIZATION_COPY, wxT("Copy")),
        0,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    row_sizer_inner->Add(
        col_sizer_inner,
        0,                      // Stretchable OR not stretchable.
        wxALL,                  // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    static_box_sizer->Add(
        row_sizer_inner,
        1,                      // Stretchable OR not stretchable.
        wxALL | wxEXPAND,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        5                       // The surrounding padding. Only effective with wxALL present.
        );

    panel_sizer->Add(
        static_box_sizer,
        0,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxLEFT | wxRIGHT,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    ////////////////////////////////////////////////////////
    // Buttons Container
    ////////////////////////////////////////////////////////

    row_sizer_inner = new wxBoxSizer(wxHORIZONTAL);

    row_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HMAC_SIGNATURE_CLEAR, wxT("Clear")),
        0,                      // Stretchable OR not stretchable.
        wxALL,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    row_sizer_inner->AddSpacer(20);

    row_sizer_inner->Add(
        new wxButton(panel, ID_ZN_BTN_HMAC_SIGNATURE_GENERATE, wxT("Generate")),
        0,                      // Stretchable OR not stretchable.
        wxALL,       // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        2);                     // The surrounding padding. Only effective with wxALL present.

    panel_sizer->Add(
        row_sizer_inner,
        0,                                               // Stretchable OR not stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxALL,    // wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL
        8);

    panel->Layout();
}


void znNotebook::ConstructTab_About()
{
    ////////////////////////////////////////////////////////
    // Panel
    ////////////////////////////////////////////////////////

    wxPanel* panel = new wxPanel(this);
    AddPage(panel, wxT("About Us"), true);

    ////////////////////////////////////////////////////////
    // Sizer
    ////////////////////////////////////////////////////////

    wxBoxSizer *panel_sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panel_sizer);

    ////////////////////////////////////////////////////////
    // Image
    ////////////////////////////////////////////////////////

    wxBoxSizer *image_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBitmap *bitmap_about = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxT("BITMAP_ABOUT"), wxBITMAP_TYPE_BMP_RESOURCE));

    image_sizer->Add(bitmap_about, 0, wxALL, 0);

    ////////////////////////////////////////////////////////
    // Text
    ////////////////////////////////////////////////////////

    wxBoxSizer *info_sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* static_text = new wxStaticText(panel, wxID_ANY, ZN_APP_TITLE);
    wxFont bold_font(20, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    static_text->SetFont(bold_font);
    info_sizer->Add(static_text, 0, wxALIGN_CENTER | wxALL, 3);

    info_sizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Version : 0.0.5")), 0, wxALIGN_CENTER | wxALL, 3);
    info_sizer->Add(new wxStaticText(panel, wxID_ANY, wxT("by")), 0, wxALIGN_CENTER | wxALL, 3);
    info_sizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Web Sailor Production")), 0, wxALIGN_CENTER | wxALL, 3);
    info_sizer->Add(new wxHyperlinkCtrl(panel, wxID_ANY, wxT("http://www.khngai.com/"), wxT("http://www.khngai.com/")), 0, wxALIGN_CENTER | wxALL, 3 );

    ////////////////////////////////////////////////////////
    // Bind all UI controls with panel sizer.
    ////////////////////////////////////////////////////////

    panel_sizer->Add(
        image_sizer,
        0,                                      // Stretch factor
        wxALIGN_CENTER_HORIZONTAL | wxALL,      // Expand the list control to fill the entire client area.
        16);

    panel_sizer->Add(
        info_sizer,
        0,                                      // Stretch factor
        wxALIGN_CENTER_HORIZONTAL | wxALL,      // Expand the list control to fill the entire client area.
        8);

    panel->Layout();
}