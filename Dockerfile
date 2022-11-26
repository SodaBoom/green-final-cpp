FROM centos:8

# # 赛题要求提交atec_project文件
COPY atec_project /home/admin/atec_project
RUN chmod +x /home/admin/atec_project/run.sh

COPY ./build/green_final /home/admin/atec_project/green_final

# # 拷贝当前目录
# RUN rm -rf /root/source/green
# COPY ./ /root/source/green

# RUN cd /etc/yum.repos.d/
# RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
# RUN sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*
# RUN yum makecache
# RUN yum update -y

# # 安装工具链
# # 安装高版本 gcc和g++
# WORKDIR /root/source/
# RUN dnf install centos-release-scl
# RUN dnf install devtoolset-9-gcc devtoolset-9-gcc-c++ devtoolset-9-binutils
# RUN echo -e "\nsource /opt/rh/devtoolset-9/enable" >>/etc/profile
# RUN source /etc/profile

# RUN yum install automake autoconf libtool make openssl-devel -y
# RUN yum install wget -y
# RUN wget -c https://github.com/Kitware/CMake/releases/download/v3.17.0-rc3/cmake-3.17.0-rc3.tar.gz
# RUN tar -zxvf cmake-3.17.0-rc3.tar.gz
# RUN cd cmake-3.17.0-rc3
# RUN ./bootstrap
# RUN gmake
# RUN gmake install

# # # 1. 安装mariadb-cpp-conn头文件和动态库
# # # https://mariadb.com/docs/ent/connect/programming-languages/c/install/
# RUN wget https://downloads.mariadb.com/MariaDB/mariadb_repo_setup
# RUN echo "367a80b01083c34899958cdd62525104a3de6069161d309039e84048d89ee98b  mariadb_repo_setup" \
#     | sha256sum -c -
# RUN chmod +x mariadb_repo_setup
# RUN ./mariadb_repo_setup \
#    --mariadb-server-version="mariadb-10.6"

# RUN yum install MariaDB-shared MariaDB-devel -y
# WORKDIR /root/source/green/mariadb-connector-cpp-1.1.1-beta-centos74-amd64

# # 1.1 头文件
# RUN cp -r include/mariadb /usr/include/
# # 1.2 动态库
# RUN install -d /usr/lib/mariadb
# RUN install -d /usr/lib/mariadb/plugin
# RUN install lib64/mariadb/libmariadbcpp.so /usr/lib
# RUN install lib64/mariadb/plugin/* /usr/lib/mariadb/plugin

# # 2. 编译项目
# WORKDIR /root/source/green
# RUN rm -rf build && mkdir -p build && cd build && cmake .. && make -j4

# # 3. 产物复制到提交目录
# WORKDIR /root/source/green
# RUN cp build/green_final /home/admin/atec_project/green_final





# https://cmake.org/files/v3.16/cmake-3.16.3-Linux-x86_64.tar.gz
