#include <iostream>
#include <mysql/mysql.h>
#include "inc/httplib.h"

using namespace std;
using namespace httplib;

// 标记启动成功
void activate_flag(){
    string activate_flag_file_path = "/home/admin/workspace/job/output/user_activated";
    system(("touch " + activate_flag_file_path).c_str());
}

int main() {
    Server svr;
    // POST /collect_energy/{userId}/{toCollectEnergyId}
    svr.Get(R"(/collect_energy/(\w+)/(\d+))", [](const Request &req, Response &res) {
        auto userId = req.matches[1];
        auto toCollectEnergyId = req.matches[2];
        res.set_content("true", "text/plain");
    });
    svr.listen("0.0.0.0", 8080);
    return 0;
}
