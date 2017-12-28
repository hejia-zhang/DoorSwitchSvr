//
// Created by heikki on 17-11-23.
//

#include <Poco/Logger.h>
#include "SeetaMockSwitchRequestResponse.h"

Poco::Net::TCPServerConnection* SeetaMockSwitchRequestHandlerFactory::createConnection(
        const Poco::Net::StreamSocket &socket)
{
    return new SeetaMockSwitchRequestResponse(socket, m_switchCfg);
}

void SeetaMockSwitchRequestResponse::run()
{
    Poco::Logger::get(std::string("Application")).information(
            "SeetaMockSwitchRequestResponse::run: New connection from %s.",
            socket().peerAddress().host().toString());

    bool isOpen = true;

    Poco::Timespan timeout{10, 0};
    unsigned char incommingBuffer[1000];
    char *recvMsgCheck = NULL;

    std::string str;

    while (isOpen)
    {
        if (socket().poll(timeout, Poco::Net::Socket::SELECT_READ) == false)
        {
        }
        else
        {
            int nBytes = -1;

            try
            {
                nBytes = socket().receiveBytes(incommingBuffer, sizeof(incommingBuffer));

                if (nBytes > 0)
                {
                    Poco::Logger::get(std::string("Application")).information("Received msg: %s", std::string((char *)incommingBuffer));

                    if (isOpen)
                    {
                        std::string t_strRes{"OK!"};
                        socket().sendBytes(t_strRes.c_str(), t_strRes.length());
                    }
                }


            }
            catch (const Poco::Exception& err)
            {
                //Handle your network erros.
                Poco::Logger::get(std::string("Application")).information(
                        "SeetaMockSwitchRequestResponse::run: Network err: %s", err.displayText());
                isOpen = false;
            }
            if (nBytes == 0)
            {
                isOpen = false;
            }
        }
    }
    Poco::Logger::get(std::string("Application")).information(
            "SeetaMockSwitchRequestResponse::run: Connection closed.");
}