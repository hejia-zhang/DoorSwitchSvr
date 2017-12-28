#ifndef DOORSWITCHSVR_DOORSWITCHHANDLER_H
#define DOORSWITCHSVR_DOORSWITCHHANDLER_H
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string>
#include "DoorSwitchChannelController.h"
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <boost/asio.hpp>
#include <boost/asio/write.hpp>
#include <boost/array.hpp>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <Poco/Timer.h>
#include <Poco/Logger.h>
#include <Poco/NotificationQueue.h>
#include "Poco/Net/NetException.h"
#include "Poco/Thread.h"
#include "Poco/Notification.h"
#include "Param.h"

/*
 * One DoorSwitchHandler controls one switch,
 * so that it has the specific switch address, port information,
 * while on DoorSwitchChannelController controls one Door. The DoorSwitchChannelController was controlled by
 * a DoorSwitchHandler and it has channel information to make command and send it.
 */

class DoorSwitchHandler {
public:
    DoorSwitchHandler(Poco::Logger& logger, const SwitchInfo& switchInfo)
        : m_io_service{}
        , m_work{m_io_service}
        , m_socket{m_io_service}
        , m_logger{logger}
        , m_serverAddr{boost::asio::ip::address::from_string(switchInfo.ip), switchInfo.port}
        , m_reconnTimer{RECONN_START_INTERVAL, RECONN_START_PERIOD_INTERVAL}
        , m_isConnected{false}
        , m_isTimerOn{false}
        , m_switchInfo(switchInfo)
    {
        m_reconnTimerDo.m_do = std::bind(&DoorSwitchHandler::onReconnTimer, this, std::placeholders::_1);
        // 这是一个保证Handler io service 异步操作网络循环的线程
        boost::thread t(boost::bind(&boost::asio::io_service::run, &m_io_service));
        m_pIOThread.swap(t);
    }

    bool isConnect()
    {
        return m_isConnected;
    }

    bool connect();
    void close();
    /*
     * ask according connector to open door
     */
    int openDoor(const short channel, int closeAfterMs);

private:
    bool reconnect();
    void startReconnectTimer();
    void onReconnTimer(Poco::Timer& timer);
    void handleRWError();

private:
    std::map<unsigned short, std::unique_ptr<DoorSwitchChannelController>> m_mapControllers;
    Poco::Logger& m_logger;

private:
    class ReconnTimerDo {
    public:
        void onTimer(Poco::Timer& timer)
        {
            m_do(timer);
        }
        std::function<void(Poco::Timer&)> m_do;
    };

private:
    boost::asio::io_service m_io_service;
    boost::asio::io_service::work m_work;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::endpoint m_serverAddr;
    int m_interval = RECONN_START_PERIOD_INTERVAL;
    std::atomic<bool> m_isTimerOn;
    std::atomic<bool> m_isConnected;
    Poco::Timer m_reconnTimer;
    ReconnTimerDo m_reconnTimerDo;
    boost::thread m_pIOThread;
    const SwitchInfo& m_switchInfo;
};

#endif //DOORSWITCHSVR_DOORSWITCHHANDLER_H
