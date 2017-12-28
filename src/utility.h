//
// Created by heikki on 17-10-27.
//

#ifndef DOORSWITCHSVR_UTILITY_H
#define DOORSWITCHSVR_UTILITY_H

#include <string>
#include <sstream>

template <class T>
std::string Obj2Str(T obj)
{
    std::ostringstream ss;
    ss << obj;
    return ss.str();
}
#endif //DOORSWITCHSVR_UTILITY_H
