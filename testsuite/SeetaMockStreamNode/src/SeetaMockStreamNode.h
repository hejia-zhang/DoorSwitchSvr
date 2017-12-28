//
// Created by heikki on 17-11-23.
//

#ifndef SEETAMOCKSTREAMNODE_SEETAMOCKSTREAMNODE_H
#define SEETAMOCKSTREAMNODE_SEETAMOCKSTREAMNODE_H
#include "Poco/Runnable.h"
#include "SeetaMqttProxy.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"
#include "Param.h"

class SeetaMockStreamNode : public Poco::Runnable
{
public:
    SeetaMockStreamNode(SeetaMqttProxy& seetaMqttProxy, const std::string& switchId,
                        const unsigned short& channel, const unsigned short& closeAfterMs,
                        const unsigned short& intervalMs, const CmdParam& cmdParam)
        : m_seetaMqttProxy(seetaMqttProxy), m_switchId(switchId), m_channel(channel), m_closeAfterMs(closeAfterMs),
          m_intervalMs(intervalMs), m_param(cmdParam)
    {
    }
    virtual void run()
    {
        Poco::JSON::Object::Ptr rootObj = new Poco::JSON::Object();
        rootObj->set("switch_id", m_switchId);
        rootObj->set("channel", (int)m_channel);
        rootObj->set("close", (int)m_closeAfterMs);
        std::ostringstream oss;
        rootObj->stringify(oss);
        std::string msg = oss.str();
        for (;;)
        {
            // pass in topic, message, qos level, retain_flag
            // 在向DoorSwitchSvr发送消息时，请将retained flag设为false。
            // 否则mqtt broker会一直retained最后一条published的消息,并在
            // client启动时发送
            // 可用mosquitto_pub -t <topic_name> -r -n清除
            m_seetaMqttProxy.publish(nullptr, m_param.m_mosqTopic.c_str(), msg.length(), msg.c_str(), 0, false);
            std::cout << "Published message: " <<  msg << std::endl;
            sleep(m_intervalMs / S2MS);
        }
    }

private:
    SeetaMqttProxy& m_seetaMqttProxy;
    const std::string m_switchId;
    const unsigned short m_channel;
    const unsigned short m_closeAfterMs;
    const unsigned short m_intervalMs;
    const CmdParam& m_param;
};
#endif //SEETAMOCKSTREAMNODE_SEETAMOCKSTREAMNODE_H
