FROM centos:8

# # 赛题要求提交atec_project文件
COPY atec_project /home/admin/atec_project
RUN chmod +x /home/admin/atec_project/run.sh

# 拷贝当前目录
RUN rm -rf /root/source/green
COPY ./ /root/source/green

# 更新yum，安装工具链
RUN cd /etc/yum.repos.d/
RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
RUN sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*
# RUN yum makecache
RUN yum update -y

RUN yum install gcc-c++ cmake make wget -y

# # 1. 安装mariadb-cpp-conn头文件和动态库
# # https://mariadb.com/docs/ent/connect/programming-languages/c/install/
WORKDIR /root/source/green
# RUN wget https://downloads.mariadb.com/MariaDB/mariadb_repo_setup
RUN echo "367a80b01083c34899958cdd62525104a3de6069161d309039e84048d89ee98b  mariadb_repo_setup" \
    | sha256sum -c -
RUN chmod +x mariadb_repo_setup
RUN ./mariadb_repo_setup \
   --mariadb-server-version="mariadb-10.6"
RUN yum install MariaDB-shared MariaDB-devel -y

# # 1.1 头文件
WORKDIR /root/source/green/mariadb-connector-cpp-1.0.2-rhel8-amd64
RUN cp -r include/mariadb /usr/include/

# 1.2 动态库
RUN install -d /usr/lib64/mariadb
RUN install -d /usr/lib64/mariadb/plugin
RUN install lib64/mariadb/libmariadbcpp.so /usr/lib64
RUN install lib64/mariadb/plugin/* /usr/lib64/mariadb/plugin

# # 2. 编译项目
WORKDIR /root/source/green
RUN rm -rf build && mkdir -p build && cd build && cmake .. && make -j4

# # 3. 产物复制到提交目录
WORKDIR /root/source/green
RUN mv build/green_final /home/admin/atec_project/green_final

RUN chmod +x /home/admin/atec_project/green_final
