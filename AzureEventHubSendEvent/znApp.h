#ifndef ZNAPP_H
#define ZNAPP_H

#include "znSingletonParent.h"

#include <wx/app.h>

class znApp : public wxApp, public znSingletonParent
{
private:
    virtual bool OnInit();
};

wxDECLARE_APP(znApp);

#endif