# AzureEventHubSendEvent
This is an utility to send event to Azure Event Hub.

![Azure Event Hub Send Event](http://www.justuke.com/images/github/azureeventhubsendevent_01.png)

# Architecture
This is a real C++ application (without .Net) written to send event to Azure event hub. Currently, the application is only built for Windows. But I believe it can be built on Linux if you have all the required libraries installed.

There are 2 ways of sending events to Azure Event Hub.
* Through HTTPS, port 443, based on API defined at [Microsoft Azure Send Event Rest API](https://msdn.microsoft.com/en-us/library/azure/dn790664.aspx)
* Through AMQPS, port 5671

For HTTPS sending, there is a need to calculates the HMAC-SHA256 signature based on the Azure's Shared Access Policy Key and a selected expiry timestamp. Right now the signature is set to expire 1 minute after its creation. Please refers to [Shared Access Signature Authentication with Service Bus](https://msdn.microsoft.com/en-us/library/dn170477.aspx) for details.

This application also implemented a feature where HMAC-SHA256 can be generated with specific TTL.

# Dependencies
* wxWidgets 3.0.2 compiled as Unicode static linked library.
* OpenSSL 1.0.2 compiled as static linked library.
* LibCurl 7.44 compiled as static linked library.
* qpid-proton-0.9.1 compiled as static linked library.

For some reason, the latest qpid-proton-0.10 library does not work.

# Tools
* Visual Studio 2013 on Windows

# Know Issues
* AMQPS may not work behind company firewall
* Unsuccessful AMQPS event sending may cause memory leak. Suspected to be QPID Proton issue.
