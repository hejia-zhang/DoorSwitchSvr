#include "DoorSwitchEngine.h"
#include "Poco/JSON/Parser.h"

struct OpenDoorRequest 
{
    std::string m_strSwitchId;
    int m_channel;
    int m_closeAfter;//milisecond
};


DoorSwitchEngine::~DoorSwitchEngine()
{
    for (const auto& ite : m_mapOidSwitchHandler)
    {
        ite.second->close();
    }
    m_mapOidSwitchHandler.clear();
}

// return this function as soon as possible to avoiding blo
void DoorSwitchEngine::onRecvMqttMessage(const std::string &message)
{    
    // msg can be {"switch_id":"string","channel":integer,"close":integer}
    m_logger.debug("Received message, %s: ", message);
    OpenDoorRequest req;
    try
    {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(message);
        Poco::JSON::Object::Ptr jObject = result.extract<Poco::JSON::Object::Ptr>();
        req.m_strSwitchId = jObject->getValue<std::string>("switch_id");
        req.m_channel = jObject->getValue<int>("channel");
        req.m_closeAfter = jObject->getValue<int>("close");
    }
    catch (const Poco::Exception& e)
    {
        m_logger.error("DoorSwitchEngine::onRecvMqttMessage: %s", e.displayText());
        return;
    }
    auto iteRes = m_config.m_mapOidSwitchInfo.find(req.m_strSwitchId);
    if (iteRes == m_config.m_mapOidSwitchInfo.end())
    {
        m_logger.warning("do not find the switch \"%s\"", req.m_strSwitchId);
        return;
    }
    if (req.m_channel <= 0 || iteRes->second.channels < req.m_channel)
    {
        m_logger.warning("Do not supported channel number: %d", req.m_channel);
        return;
    }
    int res;
    auto val = m_mapOidSwitchHandler.find(req.m_strSwitchId);
    if (val == m_mapOidSwitchHandler.end())
    {
        // The switch ip and port pair didn't appear.
        // create the switchhandler and insert into map
        createSwitchHandler(iteRes->second);
        if (!m_mapOidSwitchHandler.crbegin()->second->connect())
        {
            m_logger.warning("DoorSwitchEngine: Can not connect to switch");
            return;
        }
        res = m_mapOidSwitchHandler.crbegin()->second->openDoor(req.m_channel, req.m_closeAfter);
    }
    else
    {
        res = val->second->openDoor(req.m_channel, req.m_closeAfter);
    }
    if (res != ERR_OK)
    {
        m_logger.warning("DoorSwitchEngine::onRecvMqttMessage::72: Fail to open door");
    }
    else
    {
        m_logger.debug("DoorSwitchEngine::onRecvMqttMessage::76: Opened door successfully");
    }
}

void DoorSwitchEngine::createSwitchHandler(const SwitchInfo& info)
{
    m_mapOidSwitchHandler.insert(std::pair<std::string, std::unique_ptr<DoorSwitchHandler>>{info.oid,
                                                                                            std::unique_ptr<DoorSwitchHandler>(
                                                                                                    new DoorSwitchHandler(m_logger, info))});
}