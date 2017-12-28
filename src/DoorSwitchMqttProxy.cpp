#include "DoorSwitchMqttProxy.h"
#include <iostream>

DoorSwitchMqttProxy::DoorSwitchMqttProxy(const MqttCfg & cfg, Poco::Logger & logger, 
    const std::function<onMqttReceive>& onMqttRecvCbFunc, std::string mqttClientId)
    : m_logger(logger), mosqpp::mosquittopp(mqttClientId.c_str(), true)
{
    mosqpp::lib_init();
    m_onMqttRecvCbFunc = onMqttRecvCbFunc;
    m_mqttCfg = cfg;
    connect(cfg.m_ip.c_str(), cfg.m_port);
    loop_start();
}

DoorSwitchMqttProxy::~DoorSwitchMqttProxy()
{
    disconnect();
    loop_stop();
    mosqpp::lib_cleanup();
}

void DoorSwitchMqttProxy::on_connect(int rc)
{
    m_logger.notice("mosquitto connected with code: %d.", rc);
    if (rc == 0)
    {
        /* Only attempt to subscribe on a successful connect. */
        subscribe(nullptr, m_mqttCfg.m_topic.c_str());
    }
}

// for DoorSwitchMqttProxy it's a serial process that will cause block
// so we need to return this function as soon as possible
void DoorSwitchMqttProxy::on_message(const struct mosquitto_message *message)
{
    // 请在收到mqtt消息时，判断内容char *是否为NULL
    if (message->payload != NULL && strcmp(message->topic, m_mqttCfg.m_topic.c_str()) == 0)
    {
        m_onMqttRecvCbFunc((char *)message->payload);
    }
}

void DoorSwitchMqttProxy::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    m_logger.notice("mosquitto subscription succeeded.");
}
