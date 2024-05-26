set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.23)

project(main_app)

add_subdirectory(sim7000mqtt)

add_library(${PROJECT_NAME} STATIC main_app.cpp main_app.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE sim7000mqtt)
