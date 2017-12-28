#ifndef PARAM_H
#define PARAM_H

#include <string>

const int S2MS = 1000;

const int MICRO2MS = 1000;

const int RECONN_START_INTERVAL = 256;

const int RECONN_START_PERIOD_INTERVAL = 2;

const int RECONN_MAX_PERIOD_INTERVAL = 256; // 这应该是RECONN_START_PERIOD_INTERVAL的K次幂

using onMqttReceive = void(const std::string &);

enum SWITCH_TYPE
{
    DIRECT_CONTROL_SWITCH = 0,
    INDIRECT_CONTROL_SWITCH,
};

struct MongoCfg // The config used in MongoDB
{
    std::string m_ip = "127.0.0.1"; // the mongodb ip
    short m_port = 27017; // the mongodb port
    std::string m_database = "system_db"; // database name of whole system
    std::string m_tb_switch = "t_switch"; // collection name of switch information
};

struct MqttCfg // The config used in Mqtt
{
    std::string m_ip = "127.0.0.1"; // the mosquitto ip
    short m_port = 1883; // the mosquitto port
    std::string m_topic = "seeta_switch_open"; // the mosquitto topic
};

struct SwitchInfo
{
    std::string oid;
    std::string ip;
    unsigned short channels;
    unsigned short port;
    SWITCH_TYPE type;
};

struct SvrCfg
{
    MongoCfg m_mongoCfg;
    MqttCfg m_mqttCfg;
    std::map<std::string, SwitchInfo> m_mapOidSwitchInfo;//key: mongodb id
    bool m_bSyncSuc;
    int m_syncS = 3600;
};

enum ERR_CODE
{
    ERR_OK = 0,
    ERR_PARSE_CONF,
    ERR_MONGO_LOAD,
    ERR_MOSQ_CONN,
    ERR_CHANNEL_NOT_INITIALIZED,
    ERR_SWITCH_NOT_CONN,
};

#endif // PARAM_H