# AzureEventHubSendEvent
This is an utility to send event to Azure Event Hub.

![Azure Event Hub Send Event](http://www.justuke.com/images/github/azureeventhubsendevent_01.png)

# Architecture
This is a real C++ application (without .Net) written to send event to Azure event hub. Currently, the application is only built for Windows. But I believe it can be built on Linux if you have all the required libraries installed.

This utility is developed using the [Microsoft Azure Send Event Rest API](https://msdn.microsoft.com/en-us/library/azure/dn790664.aspx). It calcualtes the HMAC-SHA256 signature based on the Azure's Shared Access Policy Key and a selected expiry timestamp. Right now the signature is set to expire 1 minute after its creation. Please refers to [Shared Access Signature Authentication with Service Bus](https://msdn.microsoft.com/en-us/library/dn170477.aspx) for details.

Since the calculated HMAC-SHA256 is displayed at the status message area of the application, you may use this application as an utility to calculate HMAC-SHA256 signature as well.

# Dependencies
* wxWidgets 3.0.2 compiled as Unicode static linked library.
* OpenSSL 1.0.2 compiled as static linked library.
* LibCurl 7.44 compiled as static linked library.

# Tools
* Visual Studio 2013 on Windows
