#include "common.h"
#include "znConstants.h"
#include "znFrame.h"
#include "znControllerUi.h"
#include "znNotebook.h"

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
    // Notebook
    ////////////////////////////////////////////////////////

    znNotebook * notebook = new znNotebook(this, panel);

    ////////////////////////////////////////////////////////
    // Bind all UI controls with panel sizer.
    ////////////////////////////////////////////////////////

    top_sizer->Add(
        notebook,
        1,                                               // make vertically stretchable.
        wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL,    // Expand the list control to fill the entire client area.
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