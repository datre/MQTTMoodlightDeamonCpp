project(MQTTMoodlightDeamon)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
file(GLOB SRC . *.cpp)
find_package(Boost 1.58 REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} mosquittopp ${Boost_LIBRARIES})

