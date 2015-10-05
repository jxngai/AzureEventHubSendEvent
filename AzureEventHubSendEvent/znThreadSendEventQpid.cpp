#include "znThreadSendEventQpid.h"
#include "znControllerUi.h"
#include "znConstants.h"

// Problems with QPID Proton.
//
// Some times even after the data has been sent, the status is still PN_STATUS_PENDING.
// If a message is not sent successfully behind a firewall, there will be memory leak after application exit.

znThreadSendEventQpid::znThreadSendEventQpid()
    : wxThread(wxTHREAD_JOINABLE), m_flag_abort(false), m_qpid_messenger(NULL), m_qpid_message(NULL)
{
    wxLogDebug(wxT("<<< znThreadSendEventQpid::znThreadSendEventQpid() >>>"));

    if (Create() != wxTHREAD_NO_ERROR) return;

    if (Run() != wxTHREAD_NO_ERROR) return;
}

znThreadSendEventQpid::~znThreadSendEventQpid()
{
    wxLogDebug(wxT("<<< znThreadSendEventQpid::~znThreadSendEventQpid() >>>"));

    DestroyThread();

    Wait();
}

znThreadSendEventQpid::ExitCode znThreadSendEventQpid::Entry()
{
    wxLogDebug(wxT("<<< znThreadSendEventQpid::Entry() >>>"));

    // if (m_qpid_messenger == NULL) return (wxThread::ExitCode)-1;

    m_qpid_messenger = pn_messenger(NULL);

    if (m_qpid_messenger == NULL)
    {
        return (wxThread::ExitCode) - 1;
    }

    pn_messenger_start(m_qpid_messenger);

    pn_messenger_set_outgoing_window(m_qpid_messenger, 1);
    pn_messenger_outgoing_tracker(m_qpid_messenger);

    // Set the time out, so that pn_messenger_send() will NOT be blocking forever.
    // If pn_messenger_send() is blocking forever, exception will occur in QPID proton,
    // complaining io port closed.
    pn_messenger_set_timeout(m_qpid_messenger, 1000);

    m_qpid_message = NULL;

    while (TestDestroy() == false)
    {
        wxLogDebug(wxT("<<< znThreadSendEventQpid::Entry() >>> ... Loop"));

        if (m_qpid_message != NULL)
        {
            pn_tracker_t tracker = pn_messenger_outgoing_tracker(m_qpid_messenger);

            pn_status_t status = pn_messenger_status(m_qpid_messenger, tracker);

            wxLogDebug("Message Delivery Status = %d", status);

            if (status == PN_STATUS_ACCEPTED)
            {
                // Message has been delivered successfully.
                pn_message_free(m_qpid_message);
                m_qpid_message = NULL;
            }

            wxThreadEvent *my_event = new wxThreadEvent(wxEVT_THREAD, ID_ZN_EVENT_SEND_EVENT_THREAD_STATUS_UPDATE);
            my_event->SetInt(status);
            znSingleton::GetInstance<znControllerUi>().QueueEvent(my_event);
        }

        std::string amqps_url;
        std::string ampqs_content;

        {
            wxCriticalSectionLocker locker(m_cs_message);

            amqps_url = m_message_url;
            ampqs_content = m_message_content;

            m_message_url.clear();
            m_message_content.clear();
        }

        if (amqps_url.empty() == true || ampqs_content.empty() == true)
        {
            Sleep(500);
            continue;
        }

        pn_message_free(m_qpid_message);
        m_qpid_message = pn_message();
        pn_message_set_ttl(m_qpid_message, 1000 * 0.5);

        pn_message_set_address(m_qpid_message, amqps_url.c_str());
        pn_message_set_content_type(m_qpid_message, (char*) "application/octect-stream");
        pn_message_set_inferred(m_qpid_message, true);

        pn_data_t * body = pn_message_body(m_qpid_message);
        pn_data_put_binary(body, pn_bytes(ampqs_content.length(), ampqs_content.c_str()));
        // pn_data_put_string(body, pn_bytes(ampqs_content.length(), ampqs_content.c_str()));

        pn_messenger_put(m_qpid_messenger, m_qpid_message);

        int rvalue;

        if ((rvalue = pn_messenger_errno(m_qpid_messenger)) != 0)
        {
            // PN_ERR, -2, No internet connection will come here.
            wxLogDebug("Error Code = %d", rvalue);

            pn_message_free(m_qpid_message);

            continue;
        }

        pn_messenger_send(m_qpid_messenger, -1);

        if ((rvalue = pn_messenger_errno(m_qpid_messenger)) != 0)
        {
            wxLogDebug("Error Code = %d", rvalue);
            continue;;
        }
    }

    return (wxThread::ExitCode)0;                 // success
}

void znThreadSendEventQpid::OnExit()
{
    wxLogDebug(wxT("<<< znThreadSendEventQpid::OnExit() >>>"));

    pn_tracker_t tracker = pn_messenger_outgoing_tracker(m_qpid_messenger);

    pn_status_t status = pn_messenger_status(m_qpid_messenger, tracker);

    wxLogDebug("The tracker status is %d", status);

    if (PN_STATUS_PENDING == status)
    {
        pn_message_clear(m_qpid_message);
        // pn_messenger_interrupt(m_qpid_messenger);
    }

    pn_messenger_stop(m_qpid_messenger);

    pn_messenger_free(m_qpid_messenger);
    pn_message_free(m_qpid_message);

    // Post a thread completion event.
}

bool znThreadSendEventQpid::TestDestroy()
{
    if (wxThread::TestDestroy() == true) return true;

    wxCriticalSectionLocker locker(m_cs_abort);
    if (m_flag_abort == true) return true;

    return false;
}

void znThreadSendEventQpid::DestroyThread()
{
    wxLogDebug(wxT("<<< znThreadSendEventQpid::DestroyThread() >>>"));

    //pn_messenger_interrupt(m_qpid_messenger);

    wxCriticalSectionLocker locker(m_cs_abort);
    m_flag_abort = true;
}

void znThreadSendEventQpid::SendQpidMessage(std::string url, std::string content)
{
    wxCriticalSectionLocker locker(m_cs_message);

    m_message_url = url;
    m_message_content = content;
}

void znThreadSendEventQpid::InterruptQpid()
{
    pn_messenger_interrupt(m_qpid_messenger);
}