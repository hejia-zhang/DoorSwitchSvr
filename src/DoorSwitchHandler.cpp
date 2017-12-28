#include "DoorSwitchHandler.h"
bool DoorSwitchHandler::reconnect()
{
    boost::system::error_code error;
    m_socket.connect(m_serverAddr, error);
    if (error)
    {
        m_logger.notice("reconnect fail: %s", error.message());
        m_socket.close();
        m_isConnected = false;
    }
    else
    {
        m_logger.information("connect ok");
        m_isConnected = true;
    }
    return m_isConnected;
}

bool DoorSwitchHandler::connect()
{
    if (!reconnect())
    {
        startReconnectTimer();
    }
    return m_isConnected;
}

void DoorSwitchHandler::startReconnectTimer()
{
    m_isTimerOn = true;
    m_interval = RECONN_START_PERIOD_INTERVAL;
    m_reconnTimer.stop();
    m_reconnTimer.setPeriodicInterval(m_interval * S2MS);
    m_reconnTimer.start(Poco::TimerCallback<ReconnTimerDo>(m_reconnTimerDo, &ReconnTimerDo::onTimer));
}

void DoorSwitchHandler::onReconnTimer(Poco::Timer& timer)
{
    if (reconnect())
    {
        m_reconnTimer.restart(0);
        m_interval = RECONN_START_PERIOD_INTERVAL;
        m_isTimerOn = false;
    }
    else
    {
        if (m_interval * RECONN_START_PERIOD_INTERVAL < RECONN_MAX_PERIOD_INTERVAL + 1)
        {
            m_interval = m_interval * RECONN_START_PERIOD_INTERVAL;
        }
        m_reconnTimer.setPeriodicInterval(m_interval * S2MS);
    }
}

void DoorSwitchHandler::close()
{
    m_socket.close();
    m_io_service.stop();
    m_reconnTimer.stop();
    m_pIOThread.join();
    for (auto &iter : m_mapControllers)
    {
        iter.second->close();
    }
}

int DoorSwitchHandler::openDoor(const short channel, int closeAfterMs)
{
    if (m_isConnected)
    {
        auto iter = m_mapControllers.find(channel);
        if (iter == m_mapControllers.end())
        {
            m_mapControllers.insert(std::pair<unsigned short, std::unique_ptr<DoorSwitchChannelController>>
                                            (channel, std::unique_ptr<DoorSwitchChannelController>(
                                                    new DoorSwitchChannelController(m_io_service, m_logger, m_socket, m_switchInfo))));
            if (ERR_OK != m_mapControllers.crbegin()->second->Init(channel, std::bind(&DoorSwitchHandler::handleRWError, this)))
            {
                m_logger.error("DoorSwitchHandler::openDoor Err: The specified channel %d can not been initialized",
                               (int)channel);
                return ERR_CHANNEL_NOT_INITIALIZED;
            }
            m_mapControllers.crbegin()->second->Start();

            DoorSwitchOpenNotification::Ptr pTask = new DoorSwitchOpenNotification(closeAfterMs);
            m_mapControllers.crbegin()->second->addTask(pTask);
        }
        else
        {
            DoorSwitchOpenNotification::Ptr pTask = new DoorSwitchOpenNotification(closeAfterMs);
            iter->second->addTask(pTask);
        }
        return ERR_OK;
    }
    else
    {
        return ERR_SWITCH_NOT_CONN;
    }
}

void DoorSwitchHandler::handleRWError()
{
    m_isConnected = false;
    if (false == m_isTimerOn)
    {
        m_socket.close();
        startReconnectTimer();
    }
}