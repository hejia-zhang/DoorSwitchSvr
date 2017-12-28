#include "SeetaMqttProxy.h"
#include "Poco/ThreadPool.h"
#include "SeetaMockStreamNode.h"
#include "csv.h"
#include "tclap/CmdLine.h"
#include "Param.h"

bool parseOptions(int argc, char **argv, CmdParam& param)
{
    try
    {
        TCLAP::CmdLine cmd("SeetaMockStreamNode", ' ', "1.0");

        // Define a vlaue argument and add it to the command line
        TCLAP::ValueArg<std::string> ipArg("i", "ip", "the ip address for the mqtt broker", false, "127.0.0.1", "string", cmd);
        TCLAP::ValueArg<int> portArg("p", "port", "the port for the mqtt broker", false, 1883, "integer", cmd);
        TCLAP::ValueArg<std::string> topicArg("t", "topic", "the open door topic", true, "", "string", cmd);
        TCLAP::ValueArg<std::string> fileArg("f", "file_path", "the test file path", true, "", "string", cmd);

        cmd.parse(argc, argv);
        param.m_mosqIp = ipArg.getValue();
        param.m_mosqPort = (unsigned short)portArg.getValue();
        param.m_mosqTopic = topicArg.getValue();
        param.m_test_file_path = fileArg.getValue();
        return true;
    }
    catch(TCLAP::ArgException& e) // catch any exceptions
    {
        std::cout << "[command line] error: " << e.error() << '\n';
        return false;
    }
}

int main(int argc, char **argv)
{
    CmdParam cmdParam{};
    if (!parseOptions(argc, argv, cmdParam))
    {
        return 0;
    }

    // Initialize libmosquitto
    mosqpp::lib_init();
    SeetaMqttProxy mqttProxyHandler("SeetaMockStreamNode", cmdParam.m_mosqIp, cmdParam.m_mosqPort);

    Poco::ThreadPool m_threadPool{std::string{"SeetaMockStreamNodeThreadPool"}};

    std::vector<SeetaMockStreamNode> t_vecSeetaMockStreamNode;

    try
    {
        // read the switch information from csv file
        io::CSVReader<4> in(cmdParam.m_test_file_path);
        in.read_header(io::ignore_extra_column, "switch_id", "channel", "close", "interval");
        std::string t_switch_id;
        std::string t_channel;
        std::string t_close;
        std::string t_interval;
        while (in.read_row(t_switch_id, t_channel, t_close, t_interval))
        {
            t_vecSeetaMockStreamNode.emplace_back(mqttProxyHandler, t_switch_id, stoi(t_channel), stoi(t_close), stoi(t_interval), cmdParam);
        }
    }
    catch (const std::exception& err)
    {
        std::cout << err.what() << std::endl;
    }

    for (auto& val : t_vecSeetaMockStreamNode)
    {
        try
        {
            m_threadPool.start(val);
        }
        catch (const Poco::NoThreadAvailableException& e)
        {
            m_threadPool.addCapacity(m_threadPool.capacity());
            m_threadPool.start(val);
        }
    }
    m_threadPool.joinAll();
    mosqpp::lib_cleanup();
    return 0;
}