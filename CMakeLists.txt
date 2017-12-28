cmake_minimum_required(VERSION 2.8.3)

project(DoorSwitchSvr)

set(CMAKE_CXX_STANDARD 11)

link_directories(/usr/local/lib /usr/lib /usr/lib64)

include_directories(/usr/local/include)

add_subdirectory(${CMAKE_HOME_DIRECTORY}/src)
add_subdirectory(${CMAKE_HOME_DIRECTORY}/testsuite/SeetaMockStreamNode)
add_subdirectory(${CMAKE_HOME_DIRECTORY}/testsuite/SeetaMockSwitch)