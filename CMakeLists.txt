cmake_minimum_required(VERSION 3.10)
project(green_final)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -Ofast")

include_directories(deps/mariadb-connector-cpp/include)
include_directories(inc)
add_executable(green_final main.cpp)

link_directories(/mariadb)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mariadbcpp)
