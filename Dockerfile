FROM centos:8

# 赛题要求提交atec_project文件
COPY atec_project /home/admin/atec_project
RUN chmod +x /home/admin/atec_project/run.sh

# 拷贝当前目录
RUN rm -rf /root/source/green
COPY ./ /root/source/green

# 2. 编译项目
WORKDIR /root/source/green
RUN rm -rf build && mkdir -p build && cd build && cmake .. && make -j4

# 3. 产物复制到提交目录
WORKDIR /root/source/green
RUN cp build/green_final /home/admin/atec_project/green_final
