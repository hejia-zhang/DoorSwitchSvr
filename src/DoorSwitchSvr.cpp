#include "DoorSwitchSvr.h"
#include "DoorSwitchHandler.h"
#include "mosquittopp.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"

int DoorSwitchSvr::main(const std::vector<std::string> &args)
{
    int res = parseConfiguration();
    // Get parameters from configuration file
    if (res != EXIT_OK || !loadSwitchSetting(m_config.m_mapOidSwitchInfo))
    {
        return res;
    }
    logger().notice("load %d switches", (int)m_config.m_mapOidSwitchInfo.size());
    {
        DoorSwitchEngine DoorSwitchEngine(m_config, logger());
        std::function<onMqttReceive> onMqttRecvCbFunc = std::bind(&DoorSwitchEngine::onRecvMqttMessage, &DoorSwitchEngine, std::placeholders::_1);
        Poco::UUIDGenerator uuidGenerator;
        DoorSwitchMqttProxy DoorSwitchMqttProxy(m_config.m_mqttCfg, logger(), onMqttRecvCbFunc, uuidGenerator.create().toString());
        logger().notice("Initializing the server successfully.");
        waitForTerminationRequest();
    }
    return EXIT_OK;
}

int DoorSwitchSvr::parseConfiguration()
{
    int res = EXIT_OK;
    try
    {
        m_config.m_mongoCfg.m_ip = config().getString("mongodb.ip", m_config.m_mongoCfg.m_ip);
        m_config.m_mongoCfg.m_port = (short)config().getInt("mongodb.port", m_config.m_mongoCfg.m_port);
        m_config.m_mongoCfg.m_database = config().getString("mongodb.database", m_config.m_mongoCfg.m_database);
        m_config.m_mongoCfg.m_tb_switch = config().getString("mongodb.doorswitch", m_config.m_mongoCfg.m_tb_switch);
        m_config.m_syncS = config().getInt("mongodb.sync.interval", m_config.m_syncS);

        m_config.m_mqttCfg.m_ip = config().getString("mqtt.ip", m_config.m_mqttCfg.m_ip);
        m_config.m_mqttCfg.m_port = (short)config().getInt("mqtt.port", m_config.m_mqttCfg.m_port);
        m_config.m_mqttCfg.m_topic = config().getString("mqtt.topic", m_config.m_mqttCfg.m_topic);
    }
    catch (const Poco::Exception &e)
    {
        logger().error(e.displayText());
        res = EXIT_CONFIG;
    }
    return res;
}


bool DoorSwitchSvr::loadSwitchSetting(std::map<std::string, SwitchInfo> &mapOidSwitchInfo)
{
    bool bRes = true;
    try
    {
        Poco::MongoDB::Connection connection(m_config.m_mongoCfg.m_ip, m_config.m_mongoCfg.m_port);
        Poco::MongoDB::Cursor cursor(m_config.m_mongoCfg.m_database, m_config.m_mongoCfg.m_tb_switch);

        cursor.query().returnFieldSelector().add("_id", 1);
        cursor.query().returnFieldSelector().add("ip", 1);
        cursor.query().returnFieldSelector().add("port", 1);
        cursor.query().returnFieldSelector().add("channels", 1);
        cursor.query().returnFieldSelector().add("type", 1);
        Poco::MongoDB::ResponseMessage &response = cursor.next(connection);
        for (;;)
        {
            for (Poco::MongoDB::Document::Vector::iterator it = response.documents().begin();
                it != response.documents().end(); ++it)
            {
                SwitchInfo record;
                record.oid = ((*it)->get<Poco::MongoDB::ObjectId::Ptr>("_id"))->toString();
                if (!(*it)->exists("ip") || !(*it)->exists("port") || !(*it)->exists("channels") || !(*it)->exists("type"))
                {
                    continue;
                }
                record.ip = (*it)->get<std::string>("ip");
                // 发现问题
                // 在mongodb的collection中存储数据类型为int32时
                // 以下语句不会报错，为int64时则会报错
                record.port = (unsigned short)(*it)->get<int>("port");
                record.channels = (unsigned short)(*it)->get<int>("channels");
                record.type = (SWITCH_TYPE)(*it)->get<int>("type");
                mapOidSwitchInfo.insert(std::pair<std::string, SwitchInfo>(record.oid, record));
            }
            if (response.cursorID() == 0) 
            {
                break;
            }
            response = cursor.next(connection);
        }
    }
    catch (Poco::Exception &e)
    {
        logger().error(e.displayText());
        bRes = false;
    }
    return bRes;
}

