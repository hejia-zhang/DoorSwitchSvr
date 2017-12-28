//
// Created by heikki on 17-11-27.
//

#ifndef DOORSWITCHSVR_PARAM_H
#define DOORSWITCHSVR_PARAM_H

struct CmdParam
{
    std::string m_mosqIp;
    unsigned short m_mosqPort;
    std::string m_test_file_path;
    std::string m_mosqTopic;
};

#endif //DOORSWITCHSVR_PARAM_H
