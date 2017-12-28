//
// Created by heikki on 17-10-16.
//
#ifndef DOOR_SWITCH_SVR_H
#define DOOR_SWITCH_SVR_H
#include "Poco/MongoDB/Database.h"
#include "Poco/MongoDB/Cursor.h"
#include "Poco/MongoDB/ObjectId.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/Util/ServerApplication.h"
#include "Param.h"
#include "DoorSwitchEngine.h"
#include "DoorSwitchMqttProxy.h"
#include <memory>

class DoorSwitchSvr : public Poco::Util::ServerApplication
{
public:
protected:
    void initialize(Poco::Util::Application& self)
    {
        loadConfiguration(); // load default configuration files, if present
        Poco::Util::ServerApplication::initialize(self);
    }
    int main(const std::vector<std::string>& args);

    int parseConfiguration();

    bool loadSwitchSetting(std::map<std::string, SwitchInfo> &mapOidSwitchInfo);

    void syncSwitchSetting(SvrCfg& cfg);

private:
    SvrCfg m_config;
};
#endif //DOOR_SWITCH_SVR_H
