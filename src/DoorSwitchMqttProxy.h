#ifndef DOORSWITCHSVR_DOORSWITCHMQTTPROXY_H
#define DOORSWITCHSVR_DOORSWITCHMQTTPROXY_H

#include <mosquittopp.h>
#include <string>
#include <Poco/Logger.h>
#include <functional>
#include "Param.h"

class DoorSwitchMqttProxy : public mosqpp::mosquittopp
{
public:
    DoorSwitchMqttProxy(const MqttCfg &cfg, Poco::Logger& logger,
                        const std::function<onMqttReceive> &onMqttRecvCbFunc, std::string mqttClientId);
    ~DoorSwitchMqttProxy();
    void on_connect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);

private:
    MqttCfg m_mqttCfg;
    Poco::Logger& m_logger;
    std::function<onMqttReceive> m_onMqttRecvCbFunc;
};

#endif //DOORSWITCHSVR_DOORSWITCHMQTTPROXY_H
