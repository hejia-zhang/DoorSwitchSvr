//
// Created by heikki on 17-11-23.
//

#ifndef SEETAMOCKSWITCH_SEETAMOCKSWITCH_H
#define SEETAMOCKSWITCH_SEETAMOCKSWITCH_H
#include "Poco/Util/ServerApplication.h"
#include "Poco/ThreadPool.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServer.h"
#include "Param.h"
#include "SeetaMockSwitch.h"
#include "SeetaMockSwitchRequestResponse.h"

class SeetaMockSwitch : public Poco::Util::ServerApplication
{
public:
    SeetaMockSwitch()
    {}

    virtual ~SeetaMockSwitch()
    {}

protected:
    void initialize(Poco::Util::Application& self)
    {
        loadConfiguration();
        Poco::Util::ServerApplication::initialize(self);
    }
    int main(const std::vector<std::string>& args);


    int parseConfiguration();

private:
    SwitchConfig m_switchCfg;
};

#endif //SEETAMOCKSWITCH_SEETAMOCKSWITCH_H
