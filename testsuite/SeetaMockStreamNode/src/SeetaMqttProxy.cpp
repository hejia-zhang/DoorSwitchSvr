//
// Created by heikki on 17-10-1.
//

#include "SeetaMqttProxy.h"
#include <iostream>

SeetaMqttProxy::SeetaMqttProxy(const std::string &id, const std::string &host, int port) : mosqpp::mosquittopp(id.c_str())
{
    // Connect to MQTT Broker
    connect(host.c_str(), port);

    // Start thread managing connection / publish / subscribe
    loop_start();
}

SeetaMqttProxy::~SeetaMqttProxy()
{
    disconnect();
    loop_stop();
}

void SeetaMqttProxy::on_connect(int rc)
{
    std::cout << "Connected with code " << rc  << "." << std::endl;

    if (rc == 0)
    {
        int res = subscribe(nullptr, "root", 2);
        std::cout << "Subscribe with code " << res << "." << std::endl;
    }
}

//void SeetaMqttProxy::on_subcribe(int mid, int qos_count, const int *granted_qos)
//{
//    std::cout << "Subscription succeeded." << std::endl;
//}

void SeetaMqttProxy::on_message(const struct mosquitto_message *message)
{
    std::cout << "Message received!" << std::endl;
    std::cout << message->topic << ": " << (char *)message->payload << std::endl;
    sleep(5);
}




