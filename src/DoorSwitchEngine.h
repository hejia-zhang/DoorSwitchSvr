//
// Created by heikki on 17-10-27.
//

#ifndef DOORSWITCHSVR_DOORSWITCHTHREAD_H
#define DOORSWITCHSVR_DOORSWITCHTHREAD_H

#include <Poco/Runnable.h>
#include <Poco/RefCountedObject.h>
#include <Poco/AutoPtr.h>
#include <Poco/Logger.h>
#include <Poco/NotificationQueue.h>
#include <functional>
#include "DoorSwitchHandler.h"
#include "Param.h"
#include <mosquittopp.h>


class DoorSwitchEngine
{
public:
    DoorSwitchEngine(const SvrCfg& config, Poco::Logger& logger) :
            m_config(config), m_logger(logger)
    {
    }
    ~DoorSwitchEngine();
    void onRecvMqttMessage(const std::string &message);

private:
    void createSwitchHandler(const SwitchInfo& info);

private:
    const SvrCfg& m_config;
    Poco::Logger& m_logger;
    std::map<std::string, std::unique_ptr<DoorSwitchHandler>> m_mapOidSwitchHandler;
};

#endif //DOORSWITCHSVR_DOORSWITCHTHREAD_H
