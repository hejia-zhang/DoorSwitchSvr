project(DoorSwitchSvr)

include_directories(src)

aux_source_directory(src DIR_SRC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/install/SeetaMockStreamNode)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(TARGET_OUTPUT_NAME SeetaMockStreamNode_d)
else()
    set(TARGET_OUTPUT_NAME SeetaMockStreamNode)
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

set(TEST_FILE test_file/test_csv.csv)

add_executable(${TARGET_OUTPUT_NAME} ${DIR_SRC} test_main)
target_link_libraries(${TARGET_OUTPUT_NAME} mosquittopp pthread PocoJSON PocoUtil PocoFoundation)

install(FILES ${TEST_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


