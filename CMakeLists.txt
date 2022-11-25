cmake_minimum_required(VERSION 3.10)
project(green_final)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

include_directories(deps/mariadb-connector-cpp/include)
include_directories(inc)
add_executable(green_final main.cpp)

link_directories(/mariadb)
add_executable(test_conn main.cpp)
target_link_libraries(test_conn mariadbcpp)
