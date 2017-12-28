project(DoorSwitchSvr)

aux_source_directory(${CMAKE_HOME_DIRECTORY}/src SRC)

include_directories(${CMAKE_HOME_DIRECTORY}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/install/DoorSwitchSvr)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(TARGET_OUTPUT_NAME DoorSwitchSvr_d)
    set(CONF_FILE ${CMAKE_HOME_DIRECTORY}/src/DoorSwitchSvr_d.properties)
else()
    set(TARGET_OUTPUT_NAME DoorSwitchSvr)
    set(CONF_FILE ${CMAKE_HOME_DIRECTORY}/src/DoorSwitchSvr.properties)
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

add_executable(${TARGET_OUTPUT_NAME} ${SRC})
target_link_libraries(${TARGET_OUTPUT_NAME} PocoNet PocoXML PocoZip PocoData PocoJSON PocoUtil PocoCrypto PocoMongoDB
            PocoFoundation pthread boost_system boost_thread mosquittopp mosquitto)

install(FILES ${CONF_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
