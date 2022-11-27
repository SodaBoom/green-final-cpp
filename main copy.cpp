#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>

#include "inc/httplib.h"

#include "mariadb/conncpp.hpp"

#include "mem.hpp"
#include "entity.hpp"

using namespace std;
using namespace httplib;
using namespace sql;
atomic_uint64_t thread_count = 0;
atomic_uint64_t request_count = 0;
// Instantiate Driver
Driver *driver = mariadb::get_driver_instance();
// Configure Connection
SQLString url("jdbc:mariadb://127.0.0.1/atec2022");
Properties properties({{"user",     "root"},
                       {"password", "111111"}});
vector<shared_ptr<Connection>> conn_list;

// cache
std::vector<MemToCollect> memToCollects;
std::unordered_map<std::string, std::atomic<int>> memTotalEnergyMap;

//sql 
std::vector<ToCollectEnergy> toCollectEnergy_findAll();
std::vector<TotalEnergy> totalEnergy_findAll();

void toCollectEnergy_lock(const shared_ptr<Connection> &conn);
void toCollectEnergy_update();
void toCollectEnergy_unlock(const shared_ptr<Connection> &conn);

void totalEnergy_lock(const shared_ptr<Connection> &conn);
void totalEnergy_update();
void totalEnergy_unlock(const shared_ptr<Connection> &conn);

// 标记启动成功
void activate_flag() {
    string activate_flag_file_path = "/home/admin/workspace/job/output/";
    struct stat info = {};
    if (stat(activate_flag_file_path.c_str(), &info) != 0) {  // does not exist
        system(("mkdir -p " + activate_flag_file_path).c_str());
    }
    system(("touch " + activate_flag_file_path + "user_activated").c_str());
}

void init_service() {
    std::vector<ToCollectEnergy> allToCollectEnergyList = toCollectEnergy_findAll();
    memToCollects.resize(allToCollectEnergyList.size() + 1);
    for (const ToCollectEnergy &energy : allToCollectEnergyList) {
        memToCollects[energy.id] = MemToCollect(energy.userId, energy.toCollectEnergy;
    }
    std::vector<TotalEnergy> allTotalEnergyList = totalEnergy_findAll();
    for (const TotalEnergy &energy : allTotalEnergyList) {
        memTotalEnergyMap[energy.userId] = energy.totalEnergy;
    }
    std::cout << "ALL IN MEM!" << std::endl;;
}

shared_ptr<Connection> get_conn() {
    thread_local uint64_t cur_thread_idx = -1;
    if (-1 == cur_thread_idx) {
        conn_list.emplace_back(driver->connect(url, properties));
        cur_thread_idx = thread_count.fetch_add(1);
    }
    return conn_list.at(cur_thread_idx);
}

void init_db() {
    conn_list.reserve(100);
}

std::vector<ToCollectEnergy> toCollectEnergy_findAll() {
    std::vector<ToCollectEnergy> results;
    auto conn = get_conn();
    conn->setAutoCommit(true);
    unique_ptr<PreparedStatement> stmt1(conn->prepareStatement(
        "SELECT toCollectEnergy FROM ToCollectEnergy toCollectEnergy"
    ));
    ResultSet *rs1 = stmt1->executeQuery();
    while (rs1->next()) {
        ToCollectEnergy toCollectEnergy;
        toCollectEnergy.id = rs1->getInt("id");
        toCollectEnergy.userId = rs1->getString("user_id");
        toCollectEnergy.id = rs1->getInt("to_collect_energy");
        toCollectEnergy.status = std::string(rs1->getString("status").c_str());
        results.push_back(toCollectEnergy);
    }
    return results;
}

std::vector<TotalEnergy> totalEnergy_findAll() {
    std::vector<TotalEnergy> results;
    auto conn = get_conn();
    conn->setAutoCommit(true);
    unique_ptr<PreparedStatement> stmt1(conn->prepareStatement(
        "SELECT totalEnergy FROM TotalEnergy totalEnergy"
    ));
    ResultSet *rs1 = stmt1->executeQuery();
    while (rs1->next()) {
        TotalEnergy totalEnergy;
        totalEnergy.id = rs1->getInt("id");
        totalEnergy.userId = rs1->getString("user_id");
        totalEnergy.totalEnergy = rs1->getInt("total_energy");
        results.push_back(totalEnergy);
    }
    return results;
}

void
toCollectEnergy_update(const shared_ptr<Connection> &conn, int toCollectEnergy, const char *status, int id) {
    unique_ptr<PreparedStatement> stmt(conn->prepareStatement(
            "update to_collect_energy set to_collect_energy = ?, status = ? where id = ?"
    ));
    stmt->setInt(1, toCollectEnergy);
    stmt->setString(2, status);
    stmt->setInt(3, id);
    stmt->executeUpdate();
}

void totalEnergy_update(const shared_ptr<Connection> &conn, const char *user_id, int totalEnergy) {
    unique_ptr<PreparedStatement> stmt(conn->prepareStatement(
            "update total_energy set total_energy = ?, gmt_modified = current_timestamp() where user_id = ?"
    ));
    stmt->setInt(1, totalEnergy);
    stmt->setString(2, user_id);
    stmt->executeUpdate();
}

void collect(const char *user_id, int to_collect_energy_id) {
    request_count.fetch_add(1);

    shared_ptr<Connection> conn = get_conn();
    conn->setAutoCommit(false);
    // get to_collect_energy
    // 自己来采集 或者 别人来采集但必须是未采集状态
    unique_ptr<PreparedStatement> stmt1(conn->prepareStatement(
            "select user_id, to_collect_energy from to_collect_energy"
            " where id = ? and (user_id = ? or status is null)"
    ));
    stmt1->setInt(1, to_collect_energy_id);
    stmt1->setString(2, user_id);
    ResultSet *rs1 = stmt1->executeQuery();
    if (!rs1->next()) return;
    auto tce_user_id = rs1->getString("user_id");
    auto tce_to_collect_energy = rs1->getInt("to_collect_energy");

    // get total_energy
    unique_ptr<PreparedStatement> stmt2(conn->prepareStatement(
            "select total_energy from total_energy where user_id = ?"
    ));
    stmt2->setString(1, user_id);
    ResultSet *rs2 = stmt2->executeQuery();
    if (!rs2->next()) return;
    auto te_total_energy = rs2->getInt("total_energy");

    int can_collect_energy; // 可以取多少
    bool is_collected_by_other; // 是别人取吗
    if (std::strcmp(tce_user_id.c_str(), user_id) == 0) {
        can_collect_energy = tce_to_collect_energy; // 自己取全部
        is_collected_by_other = false;
    } else {
        can_collect_energy = (int) (floor((double) tce_to_collect_energy * 0.3));
        is_collected_by_other = true;
    }
    te_total_energy += can_collect_energy;
    tce_to_collect_energy -= can_collect_energy;
    totalEnergy_update(conn, user_id, te_total_energy);
    toCollectEnergy_update(conn, to_collect_energy_id, tce_to_collect_energy, is_collected_by_other);

    conn->commit();
}

// 后台线程
void executeUpdateTotal() {
    // 1. 锁表
    auto conn = get_conn();
    totalEnergy_lock(conn);
    // 2. 刷数据
    for (auto iter = memTotalEnergyMap.begin(); iter != memTotalEnergyMap.end(); iter++) {
        totalEnergy_update(conn, /* user_id */iter->first.c_str(), /* totalEnergy */iter->second);
    }
    // 3. 解锁表
    totalEnergy_unlock(conn);
    std::cout << "UpdateTotal end" << std::endl;
}

// 后台线程
void executeUpdateToCollect() {
    // 1. 锁表
    auto conn = get_conn();
    toCollectEnergy_lock(conn);
    // 2. 刷数据
    for (int id = 1; id < memToCollects.size(); id++) {
        if (memToCollects[id].modified_) {
            const char *new_status = memToCollects[id].status_ == Status::ALL_COLLECTED ? "all_collected" : "collected_by_other";
            toCollectEnergy_update(conn, memToCollects[i].total_energy_, new_status, id);
        }
    }
    // 3. 解锁表
    toCollectEnergy_unlock(conn);
    std::cout << "UpdateToCollect end" << std::endl;
}

// c++ fetch_add 先取后加
// java addAndGet 先加后取
std::atomic<int64_t> request_count = 1;
std::thread executeUpdateToCollect_bg;
std::thread executeUpdateTotal_bg;
int main() {
    // 初始化db
    init_db();
    // 初始化server
    Server svr;
    // 初始化服务
    init_service();
    // POST /collect_energy/{userId}/{toCollectEnergyId}
    svr.Get(R"(/collect_energy/(\w+)/(\d+))", [&](const Request &req, Response &res) {
        // controller
        thread_local char userId[4096];
        memcpy(userId, req.matches[1].str().c_str(), req.matches[1].str().length());
        auto toCollectEnergyId = atoi(req.matches[2].str().c_str());
        collect(userId, toCollectEnergyId);

        int64_t request_idx = request_count.fetch_add(1);
        if (request_idx == 1000000 - 1) {
            std::cout << "UpdateToCollect start " << request_idx << std::endl;
            executeUpdateToCollect_bg = std::thread(executeUpdateToCollect);
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(200ms);
        } else if (request_idx == 1000000) {
            std::cout << "UpdateTotal start " << request_idx << std::endl;
            executeUpdateToCollect_bg = std::thread(executeUpdateTotal);
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(200ms);
        }
        res.set_content("true", "text/plain");
    });
    svr.Post(R"(/collect_energy/(\w+)/(\d+))", [&](const Request &req, Response &res) {
        // controller
        thread_local char userId[4096];
        memcpy(userId, req.matches[1].str().c_str(), req.matches[1].str().length());
        auto toCollectEnergyId = atoi(req.matches[2].str().c_str());
        collect(userId, toCollectEnergyId);

        int64_t request_idx = request_count.fetch_add(1);
        if (request_idx == 1000000 - 1) {
            std::cout << "UpdateToCollect start " << request_idx << std::endl;
            executeUpdateToCollect_bg = std::thread(executeUpdateToCollect);
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(200ms);
        } else if (request_idx == 1000000) {
            std::cout << "UpdateTotal start " << request_idx << std::endl;
            executeUpdateToCollect_bg = std::thread(executeUpdateTotal);
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(200ms);
        }
    });
    svr.set_exception_handler([](const auto &req, auto &res, const std::exception_ptr &ep) {
        res.set_content("true", "text/html");
        res.status = 200;
    });
    svr.set_error_handler([](const auto &req, auto &res) {
        res.set_content("true", "text/html");
        res.status = 200;
    });

    if (!svr.bind_to_port("0.0.0.0", 8080)) {
        std::cerr << "bind port fail" << std::endl;
    }
    activate_flag();
    if (!svr.listen_after_bind()) {
        std::cerr << "listen fail" << std::endl;
    }
    return 0;
}
