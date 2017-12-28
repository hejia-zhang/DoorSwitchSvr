//
// Created by heikki on 17-11-23.
//
#include "SeetaMockSwitch.h"

int SeetaMockSwitch::main(const std::vector<std::string> &args)
{
    int nRes = parseConfiguration();
    if (nRes != EXIT_OK)
    {
        return nRes;
    }
    logger().notice("The switch ip: %s, port: %d", m_switchCfg.m_switch_ip, (int)m_switchCfg.m_port);

    int maxQueued = config().getInt("MAX_LISTEN_QUEUE", 4096);
    int maxThreads = config().getInt("MAX_POCO_THREADS", 1000);

    Poco::ThreadPool::defaultPool().addCapacity(maxThreads);

    Poco::Net::TCPServerParams* pParams = new Poco::Net::TCPServerParams;
    pParams->setMaxQueued(maxQueued);
    pParams->setMaxThreads(maxThreads);

    Poco::Net::ServerSocket svs(m_switchCfg.m_port);
    Poco::Net::TCPServer srv(new SeetaMockSwitchRequestHandlerFactory(m_switchCfg), svs, pParams);

    logger().information("SeetaMockSwitch::main: Started TCP Server with Port NUmber: %d", (int)m_switchCfg.m_port);

    srv.start();
    waitForTerminationRequest();
    srv.stop();
    return EXIT_OK;
}

int SeetaMockSwitch::parseConfiguration()
{
    int res = EXIT_OK;
    try
    {
        if (!config().has("switch.ip") || !config().has("switch.port"))
        {
            logger().error("SeetaMockSwitch::parseConfiguration: A switch must have ip and port!");
            res = EXIT_CONFIG;
        }
        m_switchCfg.m_switch_ip = config().getString("switch.ip");
        m_switchCfg.m_port = (unsigned short)config().getInt("switch.port");
    }
    catch (const Poco::Exception& err)
    {
        logger().error("SeetaMockSwitch::parseConfiguration: %s", err.displayText());
        res = EXIT_CONFIG;
    }
    return res;
}