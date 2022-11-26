#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <cmath>
#include "inc/httplib.h"

// #include "mariadb/conncpp.hpp"

using namespace std;
using namespace httplib;
// using namespace sql;
atomic_uint64_t thread_count = 0;
atomic_uint64_t request_count = 0;
// Instantiate Driver
// Driver *driver = mariadb::get_driver_instance();
// Configure Connection
// SQLString url("jdbc:mariadb://127.0.0.1/atec2022");
// Properties properties({{"user",     "root"},
                    //    {"password", "111111"}});
// vector<shared_ptr<Connection>> conn_list;

// 标记启动成功
void activate_flag() {
    string activate_flag_file_path = "/home/admin/workspace/job/output/";
    struct stat info = {};
    if (stat(activate_flag_file_path.c_str(), &info) != 0) {  // does not exist
        system(("mkdir -p " + activate_flag_file_path).c_str());
    }
    system(("touch " + activate_flag_file_path + "user_activated").c_str());
}

// shared_ptr<Connection> get_conn() {
//     thread_local uint64_t cur_thread_idx = -1;
//     if (-1 == cur_thread_idx) {
//         conn_list.emplace_back(driver->connect(url, properties));
//         cur_thread_idx = thread_count.fetch_add(1);
//     }
//     return conn_list.at(cur_thread_idx);
// }

void init_db() {
    // conn_list.reserve(100);
}

// void
// toCollectEnergy_update(const shared_ptr<Connection> &conn, int id, int toCollectEnergy, bool is_collected_by_other) {
//     unique_ptr<PreparedStatement> stmt(conn->prepareStatement(
//             "update to_collect_energy set to_collect_energy = ?, status = ?, gmt_modified = current_timestamp() where id = ?"
//     ));
//     stmt->setInt(1, toCollectEnergy);
//     stmt->setString(2, is_collected_by_other ? "collected_by_other" : "all_collected");
//     stmt->setInt(3, id);
//     stmt->executeUpdate();
// }

// void totalEnergy_update(const shared_ptr<Connection> &conn, const char *user_id, int totalEnergy) {
//     unique_ptr<PreparedStatement> stmt(conn->prepareStatement(
//             "update total_energy set total_energy = ?, gmt_modified = current_timestamp() where user_id = ?"
//     ));
//     stmt->setInt(1, totalEnergy);
//     stmt->setString(2, user_id);
//     stmt->executeUpdate();
// }

void collect(const char *user_id, int to_collect_energy_id) {
    // request_count.fetch_add(1);

    // shared_ptr<Connection> conn = get_conn();
    // conn->setAutoCommit(false);
    // // get to_collect_energy
    // // 自己来采集 或者 别人来采集但必须是未采集状态
    // unique_ptr<PreparedStatement> stmt1(conn->prepareStatement(
    //         "select user_id, to_collect_energy from to_collect_energy"
    //         " where id = ? and (user_id = ? or status is null)"
    // ));
    // stmt1->setInt(1, to_collect_energy_id);
    // stmt1->setString(2, user_id);
    // ResultSet *rs1 = stmt1->executeQuery();
    // if (!rs1->next()) return;
    // auto tce_user_id = rs1->getString("user_id");
    // auto tce_to_collect_energy = rs1->getInt("to_collect_energy");

    // // get total_energy
    // unique_ptr<PreparedStatement> stmt2(conn->prepareStatement(
    //         "select total_energy from total_energy where user_id = ?"
    // ));
    // stmt2->setString(1, user_id);
    // ResultSet *rs2 = stmt2->executeQuery();
    // if (!rs2->next()) return;
    // auto te_total_energy = rs2->getInt("total_energy");

    // int can_collect_energy; // 可以取多少
    // bool is_collected_by_other; // 是别人取吗
    // if (std::strcmp(tce_user_id.c_str(), user_id) == 0) {
    //     can_collect_energy = tce_to_collect_energy; // 自己取全部
    //     is_collected_by_other = false;
    // } else {
    //     can_collect_energy = (int) (floor((double) tce_to_collect_energy * 0.3));
    //     is_collected_by_other = true;
    // }
    // te_total_energy += can_collect_energy;
    // tce_to_collect_energy -= can_collect_energy;
    // totalEnergy_update(conn, user_id, te_total_energy);
    // toCollectEnergy_update(conn, to_collect_energy_id, tce_to_collect_energy, is_collected_by_other);

    // conn->commit();
}

int main() {
    // 初始化db
    init_db();
    // 初始化server
    Server svr;
    // POST /collect_energy/{userId}/{toCollectEnergyId}
    svr.Get(R"(/collect_energy/(\w+)/(\d+))", [&](const Request &req, Response &res) {
        thread_local char userId[4096];
        memcpy(userId, req.matches[1].str().c_str(), req.matches[1].str().length());
        auto toCollectEnergyId = atoi(req.matches[2].str().c_str());
        collect(userId, toCollectEnergyId);
        res.set_content("true", "text/plain");
    });
    svr.Post(R"(/collect_energy/(\w+)/(\d+))", [&](const Request &req, Response &res) {
        thread_local char userId[4096];
        memcpy(userId, req.matches[1].str().c_str(), req.matches[1].str().length());
        auto toCollectEnergyId = atoi(req.matches[2].str().c_str());
        collect(userId, toCollectEnergyId);
        res.set_content("true", "text/plain");
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
