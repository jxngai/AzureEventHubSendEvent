#include "znApp.h"
#include "znFrame.h"

#if defined _MSC_VER

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif

bool znApp::OnInit()
{
#if defined _MSC_VER

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // _CrtSetBreakAlloc(37266);

#endif

    // Frame will delete itself upon application exit.
    znFrame *frame = new znFrame();
    frame->Center();
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

wxIMPLEMENT_APP(znApp);