//
// Created by heikki on 17-11-22.
//

#ifndef DOORSWITCHSVR_DOORSWITCHCHANNELCONNECTOR_H
#define DOORSWITCHSVR_DOORSWITCHCHANNELCONNECTOR_H
/*
 * One DoorSwitchChannelController controls one specific door
 */

#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include "Poco/ThreadPool.h"
#include "boost/asio.hpp"
#include "boost/asio/write.hpp"
#include "boost/array.hpp"
#include "Poco/Timer.h"
#include "Poco/Logger.h"
#include "Poco/Stopwatch.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread/thread.hpp"
#include "Param.h"

class DoorSwitchOpenNotification : public Poco::Notification
    // The notification sent to worker threads
{
public:
    typedef Poco::AutoPtr<DoorSwitchOpenNotification> Ptr;
    DoorSwitchOpenNotification(const int& closeAfterMs) : m_closeAfterMs(closeAfterMs)
    {
        m_sw.start();
    }
    int getCloseAfterMs()
    {
        return m_closeAfterMs;
    }
    int getElapsed()
    {
        // return unit (ms)
        return m_sw.elapsed() / MICRO2MS;
    }
private:
    const int m_closeAfterMs;
    Poco::Stopwatch m_sw;
};

using handleRWErrorCbFunc = void(void);

class DoorSwitchChannelController : public Poco::Runnable {
public:
    DoorSwitchChannelController(boost::asio::io_service& ios, Poco::Logger& logger,
                                boost::asio::ip::tcp::socket& socket, const SwitchInfo& switchInfo)
            : m_logger{ logger }
            , m_socket( socket )
            , m_write_ret_deadline_timer{ ios }
            , m_switchInfo(switchInfo)
    {
    }

    int Init(const unsigned short& channel, std::function<handleRWErrorCbFunc> func);

    void sendData(const std::vector<unsigned char> &data);

    static std::vector<unsigned char> make_open_command(const short channel);

    static std::vector<unsigned char> make_close_command(const short channel);

    void addTask(DoorSwitchOpenNotification::Ptr task)
    {
        m_taskNotiQue.enqueueNotification(task);
    }

    void Start()
    {
        if (!m_thread.isRunning())
        {
            m_thread.start(*this);
        }
    }

    void close()
    {
        m_stop = true;
        m_taskNotiQue.clear();
        m_taskNotiQue.wakeUpAll();
        m_thread.join();
    }

private:
    void set_read_ret_deadline()
    {
        m_write_ret_deadline_timer.expires_from_now(boost::posix_time::seconds(30));
        m_write_ret_deadline_timer.async_wait(boost::bind(&DoorSwitchChannelController::check_read_ret_deadline, this));
        boost::asio::async_read(m_socket, m_write_ret_buffer, boost::asio::transfer_at_least(3), boost::bind(&DoorSwitchChannelController::handle_read_ret, this, _1, _2));
    }
    void handle_read_ret(const boost::system::error_code& ec, size_t read_size)
    {
        m_write_ret_deadline_timer.expires_at(boost::posix_time::pos_infin);
        m_write_ret_deadline_timer.async_wait(boost::bind(&DoorSwitchChannelController::check_read_ret_deadline, this));
        if (ec || read_size == 0)
        {
            m_logger.warning("fail to read the socket");
            m_handleRWErrorCbFunc();
        }
    }
    void check_read_ret_deadline()
    {
        if (m_write_ret_deadline_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now())
        {
            m_logger.warning("read the socket timeout");
            m_handleRWErrorCbFunc();
            m_write_ret_deadline_timer.expires_at(boost::posix_time::pos_infin);
            m_write_ret_deadline_timer.async_wait(boost::bind(&DoorSwitchChannelController::check_read_ret_deadline, this));
        }
    }
    virtual void run() override;

    Poco::Logger& m_logger;
    boost::asio::ip::tcp::socket& m_socket;
    unsigned short m_channel = 1;
    boost::asio::streambuf m_write_ret_buffer;
    boost::asio::deadline_timer m_write_ret_deadline_timer;
    std::function<handleRWErrorCbFunc> m_handleRWErrorCbFunc;
    Poco::NotificationQueue m_taskNotiQue;
    const SwitchInfo& m_switchInfo;
    bool m_stop = false;
    Poco::Thread m_thread;
};

#endif //DOORSWITCHSVR_DOORSWITCHCHANNELCONNECTOR_H
