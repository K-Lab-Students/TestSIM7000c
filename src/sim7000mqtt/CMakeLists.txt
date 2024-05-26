set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.23)

project(sim7000mqtt)

add_library(${PROJECT_NAME} STATIC SIM7000MQTT.cpp SIM7000MQTT.hpp)

target_link_libraries(${PROJECT_NAME} PUBLIC stm32cubemx)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})