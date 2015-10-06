#ifndef ZNCONSTANTS_H
#define ZNCONSTANTS_H

#define ZN_APP_TITLE wxT("Event Hub Message Sender")
#define ZN_APP_VERSION wxT("0.0.1")

enum
{
    ID_ZN_FRAME = 1,
    ID_ZN_PANEL,

    ID_ZN_QUIT,
    ID_ZN_ABOUT,

    ID_ZN_TXT_HTTPS_SERVICE_BUS_NAMESPACE,
    ID_ZN_TXT_HTTPS_EVENT_HUB_NAME,

    ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_NAME,
    ID_ZN_TXT_HTTPS_SHARED_ACCESS_POLICY_KEY,

    ID_ZN_TXT_HTTPS_USER_MESSAGE,
    ID_ZN_BTN_HTTPS_SEND_MESSAGE,
    ID_ZN_BTN_HTTPS_SET_SAMPLE_MESSAGE,
    ID_ZN_BTN_HTTPS_CLEAR_MESSAGE,

    ID_ZN_TXT_HTTPS_STATUS_MESSAGE,
    ID_ZN_BTN_HTTPS_CLEAR_STATUS_MESSAGE,

    ID_ZN_TXT_AMQPS_SERVICE_BUS_NAMESPACE,
    ID_ZN_TXT_AMQPS_EVENT_HUB_NAME,

    ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_NAME,
    ID_ZN_TXT_AMQPS_SHARED_ACCESS_POLICY_KEY,

    ID_ZN_TXT_AMQPS_USER_MESSAGE,
    ID_ZN_BTN_AMQPS_SEND_MESSAGE,
    ID_ZN_BTN_AMQPS_SET_SAMPLE_MESSAGE,
    ID_ZN_BTN_AMQPS_CLEAR_MESSAGE,

    ID_ZN_TXT_AMQPS_STATUS_MESSAGE,
    ID_ZN_BTN_AMQPS_CLEAR_STATUS_MESSAGE,

    ID_ZN_TXT_HMAC_SERVICE_BUS_NAMESPACE,
    ID_ZN_TXT_HMAC_EVENT_HUB_NAME,

    ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_NAME,
    ID_ZN_TXT_HMAC_SHARED_ACCESS_POLICY_KEY,
    ID_ZN_TXT_HMAC_TTL,

    ID_ZN_TXT_HMAC_SIGNATURE,
    ID_ZN_BTN_HMAC_SIGNATURE_GENERATE,
    ID_ZN_BTN_HMAC_SIGNATURE_COPY,

    ID_ZN_EVENT_SEND_EVENT_THREAD_STATUS_UPDATE
};

#endif
