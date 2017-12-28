//
// Created by heikki on 17-11-22.
//
#include "DoorSwitchChannelController.h"

int DoorSwitchChannelController::Init(const unsigned short& channel, std::function<handleRWErrorCbFunc> func)
{
    m_channel = channel;
    m_handleRWErrorCbFunc = func;
    // 将发送TCP信号给继电器的返回确认时间初始化为无穷大，表示当前没有需要返回但还没返回的写操作
    m_write_ret_deadline_timer.expires_at(boost::posix_time::pos_infin);
    m_write_ret_deadline_timer.async_wait(boost::bind(&DoorSwitchChannelController::check_read_ret_deadline, this));
    return ERR_OK;
}

void DoorSwitchChannelController::sendData(const std::vector<unsigned char> &data)
{
    boost::system::error_code error;
    size_t len = boost::asio::write(m_socket, boost::asio::buffer(data), error);
    if (error || len < data.size())
    {
        m_logger.error("fail to write to socket: %s", error.message());
        m_handleRWErrorCbFunc();
        return;
    }

    // 如果
    if (m_write_ret_deadline_timer.expires_at() == boost::posix_time::pos_infin)
    {
        set_read_ret_deadline();
    }
}

std::vector<unsigned char> DoorSwitchChannelController::make_open_command(const short channel)
{
    unsigned char H1 = 0xcc;
    unsigned char H2 = 0xdd;
    unsigned char W = 0xa1;
    unsigned char Address = 0x01;
    int16_t D1D0 = pow(2, channel - 1);
    unsigned char D1 = D1D0 >> 8;
    unsigned char D0 = D1D0;
    unsigned char CE1 = D1;
    unsigned char CE0 = D0;
    unsigned char ADD1 = W + Address + D1 + D0 + CE1 + CE0;
    unsigned char ADD2 = ADD1 + ADD1;
    std::vector<unsigned char> res = {H1, H2, W, Address, D1, D0, CE1, CE0, ADD1, ADD2};
    return res;
}

std::vector<unsigned char> DoorSwitchChannelController::make_close_command(const short channel)
{
    unsigned char H1 = 0xcc;
    unsigned char H2 = 0xdd;
    unsigned char W = 0xa1;
    unsigned char Address = 0x01;
    int16_t CE1CE0 = pow(2, channel - 1);
    unsigned char D1 = 0X00;
    unsigned char D0 = 0X00;
    unsigned char CE1 = CE1CE0 >> 8;
    unsigned char CE0 = CE1CE0;
    unsigned char ADD1 = W + Address + D1 + D0 + CE1 + CE0;
    unsigned char ADD2 = ADD1 + ADD1;
    std::vector<unsigned char> res = {H1, H2, W, Address, D1, D0, CE1, CE0, ADD1, ADD2};
    return res;
}

void DoorSwitchChannelController::run()
{
    // 表示当前处理的任务是否是在队列里的，也即跟在别人后面
    // 在完成当前任务的处理后，会对该变量进行重新赋值
    bool bNotiInQ = false;
    int realWait;
    for (;;)
    {
        if (m_stop)
        {
            break;
        }

        Poco::Notification::Ptr pNf(m_taskNotiQue.waitDequeueNotification());
        if (!pNf)
        {
            break;
        }
        DoorSwitchOpenNotification::Ptr pWorkNf = pNf.cast<DoorSwitchOpenNotification>();
        if (!pWorkNf)
        {
            continue;
        }

        realWait = pWorkNf->getCloseAfterMs() - pWorkNf->getElapsed();
        if( realWait <= 0 )
        {
            // 此时，该人已在队列里等待超时，
            // 不处理，若他能跟随前人进入门
            // 则可以，否则需要他去摄像头前重新识别
            bNotiInQ = !m_taskNotiQue.empty();
            continue;
        }

        // 当前任务在队列里面没有等待超时
        // 当当前的继电器类型为INDIRECT_CONTROL_SWITCH的时候
        if (m_switchInfo.type == INDIRECT_CONTROL_SWITCH)
        {
            sendData(DoorSwitchChannelController::make_open_command(m_channel));
            Poco::Thread::sleep(realWait);
            sendData(DoorSwitchChannelController::make_close_command(m_channel));
            bNotiInQ = false;
        }
        else //当当前继电器类型为DIRECT_CONTROL_SWITCH的时候
        {
            // 如果当前任务非跟在别的任务之后的
            if (!bNotiInQ)
            {
                sendData(DoorSwitchChannelController::make_open_command(m_channel));
            }
            Poco::Thread::sleep(realWait);
            bNotiInQ = !m_taskNotiQue.empty();
            if (!bNotiInQ)
            {
                sendData(DoorSwitchChannelController::make_close_command(m_channel));
            }
        }
    }
}

