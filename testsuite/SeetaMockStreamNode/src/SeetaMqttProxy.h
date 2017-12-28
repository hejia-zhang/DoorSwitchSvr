//
// Created by heikki on 17-10-1.
//

#ifndef SEETAMQTTWRAPPER_SEETAMQTTWRAPPER_H
#define SEETAMQTTWRAPPER_SEETAMQTTWRAPPER_H

#include <mosquittopp.h>
#include <string>
#include <unistd.h>

/*
 * The mosquittopp class is the official C++ interface.
 * It offers subscribe / publish function.
 * Inherit it and customize our own wrapper as desired.
 */
class SeetaMqttProxy : public mosqpp::mosquittopp
{
public:
    SeetaMqttProxy(const std::string& id, const std::string& host, int port);
    ~SeetaMqttProxy();

    /*
     * This method is called when mosquittopp is connected (after a call to mosquittopp::connect).
     */
    void on_connect(int rc) override;

    /*
     * This method is called when mosquittopp reveives a message from any of its topic subscriptions.
     *
     * param message: Message from the broker
     */
    void on_message(const struct mosquitto_message *message) override;

    // void on_subcribe(int mid, int qos_count, const int *granted_qos);

    // int subscribe(int *mid, const char *sub, int qos = 0);

private:
    SeetaMqttProxy(const SeetaMqttProxy& other) = delete;
    SeetaMqttProxy& operator= (const SeetaMqttProxy& other) = delete;
};

#endif //SEETAMQTTWRAPPER_SEETAMQTTWRAPPER_H


