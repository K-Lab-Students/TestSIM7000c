set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.23)

project(sim7000mqtt)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SIM7000MQTT.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ATCommunicator.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC stm32cubemx)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME})