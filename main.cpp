#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include "inc/httplib.h"

#include "mariadb/conncpp.hpp"

using namespace std;
using namespace httplib;
using namespace sql;

// Instantiate Driver
Driver *driver = sql::mariadb::get_driver_instance();
// Configure Connection
SQLString url("jdbc:mariadb://127.0.0.1/atec2022");
Properties properties({{"user",     "root"},
                       {"password", "111111"}});

// 标记启动成功
void activate_flag() {
    string activate_flag_file_path = "/home/admin/workspace/job/output/user_activated";
    system(("touch " + activate_flag_file_path).c_str());
}

unique_ptr<Connection> get_conn() {
    unique_ptr<Connection> con(driver->connect(url, properties));
    return con;
}

void init_db() {
}

void toCollectEnergy_findById(int id) {
    unique_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("select * from to_collect_energy where id = ?"));
    stmt->setInt(1, id);
    ResultSet *rs = stmt->executeQuery();
}

void toCollectEnergy_update(int id, int toCollectEnergy) {
    unique_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("update to_collect_energy set to_collect_energy = ? where id = ?"));
    stmt->setInt(1, toCollectEnergy);
    stmt->setInt(2, id);
    stmt->executeUpdate();
}

void totalEnergy_findByUserId(const char *user_id) {
    unique_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("select * from total_energy user_id = ?"));
    stmt->setString(1, user_id);
    ResultSet *rs = stmt->executeQuery();
}

void totalEnergy_update(const char *user_id, int totalEnergy) {
    unique_ptr<Connection> conn = get_conn();
    unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("update total_energy set total_energy = ? where user_id = ?"));
    stmt->setInt(1, totalEnergy);
    stmt->setString(2, user_id);
    ResultSet *rs = stmt->executeQuery();
}

void collect(const char *user_id, int to_collect_energy_id) {

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
    svr.listen("0.0.0.0", 8080);
    return 0;
}
