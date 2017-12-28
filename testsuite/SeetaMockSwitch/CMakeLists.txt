project(DoorSwitchSvr)

include_directories(src)

aux_source_directory(src DIR_SRC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/install/SeetaMockSwitch)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(TARGET_OUTPUT_NAME SeetaMockSwitch_d)
    set(CONF_FILE src/SeetaMockSwitch_d.properties)
else()
    set(TARGET_OUTPUT_NAME SeetaMockSwitch)
    set(CONF_FILE src/SeetaMockSwitch.properties)
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

add_executable(${TARGET_OUTPUT_NAME} ${DIR_SRC})
target_link_libraries(${TARGET_OUTPUT_NAME} pthread PocoJSON PocoUtil PocoFoundation PocoNet)

install(FILES ${CONF_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})