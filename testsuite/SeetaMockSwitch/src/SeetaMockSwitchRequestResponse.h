//
// Created by heikki on 17-11-23.
//
#ifndef SEETAMOCKSWITCH_SEETAMOCKSWITCHREQUESTRESPONSE_H
#define SEETAMOCKSWITCH_SEETAMOCKSWITCHREQUESTRESPONSE_H
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Param.h"

class SeetaMockSwitchRequestResponse : public Poco::Net::TCPServerConnection
{
public:
    SeetaMockSwitchRequestResponse(const Poco::Net::StreamSocket& socket, const SwitchConfig& switchConfig)
            : Poco::Net::TCPServerConnection(socket), m_switchCfg(switchConfig)
    {
    }

    virtual ~SeetaMockSwitchRequestResponse()
    {
    }

    void run(void);

private:
    const SwitchConfig& m_switchCfg;
};

class SeetaMockSwitchRequestHandlerFactory : public Poco::Net::TCPServerConnectionFactory
{
public:
    SeetaMockSwitchRequestHandlerFactory(const SwitchConfig& switchConfig) :
            m_switchCfg(switchConfig)
    {
    }
    virtual ~SeetaMockSwitchRequestHandlerFactory()
    {
    }

    Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket);

private:
    const SwitchConfig& m_switchCfg;
};
#endif //SEETAMOCKSWITCH_SEETAMOCKSWITCHREQUESTRESPONSE_H
