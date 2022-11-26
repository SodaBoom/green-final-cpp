FROM centos:8

# 赛题要求提交atec_project文件
COPY atec_project /home/admin/atec_project
RUN chmod +x /home/admin/atec_project/run.sh

# 安装工具链
RUN cd /etc/yum.repos.d/
RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
RUN sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*
RUN yum update -y
RUN yum install gcc-c++ cmake make openssl-devel -y

# 拷贝当前目录
RUN rm -rf /root/source/green
COPY ./ /root/source/green
WORKDIR /root/source/green/deps

RUN cd mariadb-connector-c-3.3 && mkdir build && cd build && cmake .. && make -j4 && make install && cd ../../

RUN mkdir build && cd build
WORKDIR /root/source/green/deps/build
RUN cmake ../mariadb-connector-cpp/ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCONC_WITH_UNIT_TESTS=Off -DCMAKE_INSTALL_PREFIX=/usr/local -DWITH_SSL=OPENSSL
RUN cmake --build . --config RelWithDebInfo
RUN make install
RUN rm -rf /root/source/green/deps
# 2. 编译项目
WORKDIR /root/source/green
RUN rm -rf build && mkdir -p build && cd build && cmake .. && make -j4

# 3. 产物复制到提交目录
WORKDIR /root/source/green
RUN cp build/green_final /home/admin/atec_project/green_final
