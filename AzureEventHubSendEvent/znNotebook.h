#ifndef ZNNOTEBOOK_H
#define ZNNOTEBOOK_H

#include <wx/panel.h>
#include <wx/notebook.h>

class znNotebook : public wxNotebook
{
public:
    znNotebook(znFrame*const, wxPanel*);

private:
    void ConstructTab_HTTPS();
    void ConstructTab_AMQPS();
    void ConstructTab_Hmac_Generator();
    void ConstructTab_About();

private:
    const wxFrame *m_frame;
};

#endif