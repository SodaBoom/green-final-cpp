#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include "inc/httplib.h"

#include "mariadb/conncpp.hpp"

using namespace std;
using namespace httplib;
using namespace sql;
atomic_uint64_t thread_count = 0;
atomic_uint64_t request_count = 0;
// Instantiate Driver
Driver *driver = sql::mariadb::get_driver_instance();
// Configure Connection
SQLString url("jdbc:mariadb://127.0.0.1/atec2022");
Properties properties({{"user",     "root"},
                       {"password", "111111"}});
vector<shared_ptr<Connection>> conn_list;

// 标记启动成功
void activate_flag() {
    string activate_flag_file_path = "/home/admin/workspace/job/output/user_activated";
    system(("touch " + activate_flag_file_path).c_str());
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

void toCollectEnergy_findById(int id) {
    shared_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("select * from to_collect_energy where id = ?"));
    stmt->setInt(1, id);
    ResultSet *rs = stmt->executeQuery();
}

void toCollectEnergy_update(int id, int toCollectEnergy) {
    shared_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("update to_collect_energy set to_collect_energy = ? where id = ?"));
    stmt->setInt(1, toCollectEnergy);
    stmt->setInt(2, id);
    stmt->executeUpdate();
}

void totalEnergy_findByUserId(const char *user_id) {
    shared_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("select * from total_energy user_id = ?"));
    stmt->setString(1, user_id);
    ResultSet *rs = stmt->executeQuery();
}

void totalEnergy_update(const char *user_id, int totalEnergy) {
    shared_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("update total_energy set total_energy = ? where user_id = ?"));
    stmt->setInt(1, totalEnergy);
    stmt->setString(2, user_id);
    ResultSet *rs = stmt->executeQuery();
}

void collect(const char *user_id, int to_collect_energy_id) {
    uint64_t request_idx = request_count.fetch_add(1);
    cout << request_idx << endl;
}

int main() {
    // 初始化db
    init_db();
    // 初始化server
    Server svr;
    // POST /collect_energy/{userId}/{toCollectEnergyId}
    svr.Get(R"(/collect_energy/(\w+)/(\d+))", [](const Request &req, Response &res) {
        auto userId = req.matches[1].str().c_str();
        auto toCollectEnergyId = atoi(req.matches[2].str().c_str());
        collect(userId, toCollectEnergyId);
        res.set_content("true", "text/plain");
    });
    svr.Post(R"(/collect_energy/(\w+)/(\d+))", [](const Request &req, Response &res) {
        auto userId = req.matches[1].str().c_str();
        auto toCollectEnergyId = atoi(req.matches[2].str().c_str());
        collect(userId, toCollectEnergyId);
        res.set_content("true", "text/plain");
    });
    svr.listen("0.0.0.0", 8080);
    return 0;
}
