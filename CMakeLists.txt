cmake_minimum_required(VERSION 3.24)
project(green_final)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

add_executable(green_final main.cpp)

target_link_libraries(${PROJECT_NAME} pthread mysqlclient)
