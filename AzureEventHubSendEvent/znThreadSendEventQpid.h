#ifndef ZNTHREADSENDEVENTQPID_H
#define ZNTHREADSENDEVENTQPID_H

#include "common.h"

#include <proton/message.h>
#include <proton/messenger.h>

#include "znSingleton.h"

#include <wx/thread.h>
#include <wx/defs.h>

class znThreadSendEventQpid : private wxThread, public znSingleton
{
private:
    friend znSingleton;

    znThreadSendEventQpid();
    wxDECLARE_NO_COPY_CLASS(znThreadSendEventQpid);
    ~znThreadSendEventQpid();

    wxThread::ExitCode Entry();
    void OnExit();

    bool TestDestroy();
    void DestroyThread();

public:
    void SendQpidMessage(std::string, std::string);
    void InterruptQpid();

private:
    wxCriticalSection			m_cs_abort;
    wxCriticalSection			m_cs_message;

    bool						m_flag_abort;

    pn_messenger_t              *m_qpid_messenger;
    pn_message_t                *m_qpid_message;
    pn_tracker_t                m_qpid_tracker;

    std::string                 m_message_url;
    std::string                 m_message_content;
};

#endif