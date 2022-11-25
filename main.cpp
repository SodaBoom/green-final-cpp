#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <sstream>
#include <cmath>

#include "mariadb/conncpp.hpp"

int main(int argc, char** argv)
{
  // Instantiate Driver
  sql::Driver* driver = sql::mariadb::get_driver_instance();

  // Configure Connection
  sql::SQLString url("jdbc:mariadb://127.0.0.1/atec2022");
  sql::Properties properties({ {"user", argc > 1 ? argv[1] : "root"}, {"password", argc > 2 ? argv[2] : "111111"} });

  std::cerr << "Connecting using url: " << url << "with user " << properties["user"] << " and " << properties["password"].length();
  if (argc > 3) {
    properties["localSocket"]= argv[3];
    std::cerr << " via local socket " << argv[3] << std::endl;
  }
  else {
    std::cerr << " via default tcp port" << std::endl;
  }

  // Establish Connection
  try {
    std::unique_ptr<sql::Connection> con(driver->connect(url, properties));

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT 1, 'Hello world'"));
    if (rs->next()) {
     std::cout << rs->getInt(1) << rs->getString(2) << std::endl;
    }
  }
  catch (sql::SQLSyntaxErrorException & e) {
    std::cerr << "[" << e.getSQLState() << "] " << e.what() << "("<< e.getErrorCode() << ")" << std::endl;
  }
  catch (std::regex_error& e) {
    std::cerr << "Regex exception:" << e.what() << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "Standard exception:" << e.what() << std::endl;
  }

  return 0;
}
