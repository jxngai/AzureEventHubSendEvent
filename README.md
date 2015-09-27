# AzureEventHubSendEvent
This is an utility to send event to Azure Event Hub.

# Architecture
This is a real C++ application (without .Net) written to send event to Azure event hub.

The application calcualtes the HMAC-SHA256 signature based on the Azure's Shared Access Policy Key and a selected expiry timestamp.

# Dependencies
** wxWidgets 3.0.2 compiled as Unicode static linked library.
** OpenSSL compiled as static linked library.
** LibCurl compiled as static linked library.

# User Interface
