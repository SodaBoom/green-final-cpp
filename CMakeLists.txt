cmake_minimum_required(VERSION 3.10)
project(green_final)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
link_directories(/usr/local/lib64/mariadb/)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mariadbcpp pthread)
