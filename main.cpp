#include<cstdio>
#include<unistd.h>
#include<cstdlib>
#include<cstring>
#include<sys/epoll.h>
#include <sys/stat.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <cmath>
#include "mem.hpp"

#define MAXSIZE 1000
using namespace std;

std::mutex mtx;
std::condition_variable cv;
bool init_server_done = false;
atomic_uint64_t has_count = 0;
atomic_uint64_t done_count = 0;
auto start = chrono::steady_clock::now();
auto stop = chrono::steady_clock::now();

void handle_request(char *line, int len);


void send_response_head(int cfd) {
    const char *buf = "HTTP/1.1 200 OK\r\nContent-Type:application/json\r\nContent-Length:4\r\n\r\ntrue";
    send(cfd, buf, strlen(buf), 0);
}


void func_http(int cfd) {
    thread_local uint64_t count = 0;
    while (done_count < 100 * 10000) {
        char line[1024] = {0};
        ssize_t len = recv(cfd, line, sizeof(line), 0);
        if (len <= 0) {
            break;
        }
        send_response_head(cfd);//先返回响应,减少io开销
        handle_request(line, (int) len);
        uint64_t cur_done_count = done_count.fetch_add(1);
        count++;
        if ((cur_done_count + 1) % 10000 == 0) {
            stop = chrono::steady_clock::now();
            chrono::duration<double> diff = stop - start;
            cout << "cur_done_count:" << cur_done_count + 1 << " " << count << " " << diff.count() << endl;
        }
    }
    close(cfd);
}

void handle_request(char *line, int len) {
    for (int i = 5; i < len; ++i) {
        if (line[i] == ' ' || line[i] == '\r' || line[i] == '\n') {
            line[i] = '\0';
            break;
        }
    }
    if (memcmp("/collect_energy/", line + 5, 16) != 0) {
        return;
    }
    line += 5;//去掉POST
    char user_id[64] = {0};
    int user_id_len = 0;
    for (; user_id_len < len; ++user_id_len) {
        if (line[user_id_len + 16] == '/') {
            break;
        }
        user_id[user_id_len] = line[user_id_len + 16];
    }
    int to_collect_energy_id = (int) strtol(line + user_id_len + 17, nullptr, 10);
//    cout << user_id << "->" << to_collect_energy_id << endl;
    MemToCollect *memToCollect = memToCollects[to_collect_energy_id];
    MemTotalEnergy *memTotalEnergy = memTotalEnergies[memTotalEnergyMap[string(user_id)]];
    uint8_t last_status = memToCollect->status_;//能量的上一个状态
    if (strcmp(memToCollect->user_id_.data(), user_id) == 0 && last_status != ALL_COLLECTED) {
        //自己的能量并且没被自己收走
        uint8_t status = ALL_COLLECTED;
        while (atomic_compare_exchange_weak(&memToCollect->status_, &last_status, status)) {}
        if (last_status == EMPTY) {
            //没人偷过能量
            memTotalEnergy->totalEnergy_.fetch_add(memToCollect->total_energy_);
        } else {
            //能量被偷过了
            //这里不能用memToCollect->total_energy_,会有线程安全问题
            int to_collect_energy_count = (int) floor((double) memToCollect->total_energy_ * 0.3);
            memTotalEnergy->totalEnergy_.fetch_add(memToCollect->total_energy_ - to_collect_energy_count);
        }
    } else if (last_status == EMPTY) {
        //别人的能量
        uint8_t status = COLLECTED_BY_OTHER;
        if (atomic_compare_exchange_weak(&memToCollect->status_, &last_status, status)) {
            //成功偷走能量
            int to_collect_energy_count = (int) floor((double) memToCollect->total_energy_ * 0.3);
            memTotalEnergy->totalEnergy_.fetch_add(to_collect_energy_count);
            //这里不修改memToCollect->total_energy_,会有线程安全问题
        }
    }
    memToCollect->modified_ = true; //标记为修改过
}

void func() {
    int ep_fd = epoll_create(1);
    if (ep_fd == 1) {
        perror("epllo_create error");
    }
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket error");
    }
    struct sockaddr_in serv{};
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    //端口复用
    int flag = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    int ret = bind(lfd, (struct sockaddr *) &serv, sizeof(serv));
    if (ret == -1) {
        perror("bind error");
    }
    //设置监听
    ret = listen(lfd, 200);
    if (ret == -1) {
        perror("listen error");
    }
    struct epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, lfd, &ev);
    struct epoll_event all[MAXSIZE];
    init_server_done = true;
    cv.notify_all();
    struct sockaddr_in client{};
    socklen_t len = sizeof(client);
    while (true) {
        ret = epoll_wait(ep_fd, all, MAXSIZE, -1);
        if (ret == -1) {
            perror("epllo_wait error");
            continue;
        }
        //遍历发生变化的节点
        for (int i = 0; i < ret; i++) {
            //只处理读事件，其他默认不处理
            struct epoll_event *pev = &all[i];
            if (!(pev->events & EPOLLIN))   //不是读事件
            {
                continue;
            }
            if (pev->data.fd == lfd) {
                uint64_t cur_has_count = has_count.fetch_add(1);
                if (cur_has_count == 0) {
                    start = chrono::steady_clock::now();
                }
                cout << "cur_has_count:" << cur_has_count + 1 << endl;
                int cfd = accept(lfd, (struct sockaddr *) &client, &len);
                (new thread(func_http, cfd))->detach();
            }
        }
    }
}

// 标记启动成功
void activate_flag() {
    string activate_flag_file_path = "/home/admin/workspace/job/output/";
    struct stat info = {};
    if (stat(activate_flag_file_path.c_str(), &info) != 0) {  // does not exist
        system(("mkdir -p " + activate_flag_file_path).c_str());
    }
    system(("touch " + activate_flag_file_path + "user_activated").c_str());
}

int main(int argc, char *argv[]) {
    thread thread_main(func);
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [] {
        return init_server_done;
    });
    activate_flag();
    cout << "init done" << endl;
    thread_main.join();
    return 0;
}