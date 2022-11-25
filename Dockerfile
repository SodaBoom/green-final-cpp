FROM ubuntu:20.04

# 赛题要求提交atec_project文件
COPY atec_project /home/admin/atec_project

# 安装工具链
RUN apt-get update -y
RUN apt install g++ cmake -y

# 拷贝当前目录
RUN rm -rf /root/source/green
COPY ./ /root/source/green
WORKDIR /root/source/green

# 1. 安装mariadb-cpp-conn头文件和动态库
# https://mariadb.com/docs/ent/connect/programming-languages/c/install/
RUN apt install libmariadb3 libmariadb-dev -y
WORKDIR mariadb-connector-cpp-1.0.2-ubuntu-focal-amd64
# 1.1 头文件
RUN cp -r include/mariadb /usr/include/
# 1.2 动态库
RUN install -d /usr/lib/mariadb
RUN install -d /usr/lib/mariadb/plugin
RUN install lib/mariadb/libmariadbcpp.so /usr/lib
RUN install lib/mariadb/plugin/* /usr/lib/mariadb/plugin

WORKDIR /root/source/green
RUN rm -rf build && mkdir -p build && cd build && cmake .. && make -j4
