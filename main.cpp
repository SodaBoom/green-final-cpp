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

void init_db() {
    std::unique_ptr<sql::Connection> con(driver->connect(url, properties));
}

//void toCollectEnergyRepository_findById(int to_collect_energy_id){
//    std::unique_ptr<sql::Statement> stmt(conn->createStatement());
//}
//
//void collect(char *user_id, int to_collect_energy_id){
//    try {
//        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
//        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT 1, 'Hello world'"));
//    }
//    catch (sql::SQLSyntaxErrorException &e) {
//        std::cerr << "[" << e.getSQLState() << "] " << e.what() << "(" << e.getErrorCode() << ")" << std::endl;
//    }
//    catch (std::regex_error &e) {
//        std::cerr << "Regex exception:" << e.what() << std::endl;
//    }
//    catch (std::exception &e) {
//        std::cerr << "Standard exception:" << e.what() << std::endl;
//    }
//}

int main() {
    // 初始化db
    init_db();
    // 初始化server
    Server svr;
    // POST /collect_energy/{userId}/{toCollectEnergyId}
    svr.Get(R"(/collect_energy/(\w+)/(\d+))", [](const Request &req, Response &res) {
        auto userId = req.matches[1];
        auto toCollectEnergyId = req.matches[2];
//        collect(userId, toCollectEnergyId.first);
        res.set_content("true", "text/plain");
    });
    svr.listen("0.0.0.0", 8080);
    return 0;
}
