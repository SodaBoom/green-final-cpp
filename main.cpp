#include <iostream>
#include <mysql/mysql.h>

using namespace std;

int main() {
    std::cout << "Hello, World!" << std::endl;
    MYSQL *conn;
    if (!(conn = mysql_init(0))) {
        fprintf(stderr, "unable to initialize connection struct\n");
        exit(1);
    }

    // Connect to the database
    if (!mysql_real_connect(
            conn,                 // Connection
            "127.0.0.1",// Host
            "root",            // User account
            "111111",   // User password
            "atec2022",               // Default database
            3306,                 // Port number
            nullptr,                 // Path to socket file
            0                     // Additional options
    )) {
        // Report the failed-connection error & close the handle
        fprintf(stderr, "Error connecting to Server: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }

    // Use the Connection
    // ...
    int res = mysql_query(conn, "select 1;");
    if (res) {
        printf("error\n");
    } else {
        printf("OK\n");
        MYSQL_RES *result = mysql_store_result(conn);
        //得到查询到的数据条数
        int row_count = mysql_num_rows(result);
        cout<<"all data number: "<< row_count << endl;
        //得到字段的个数和字段的名字
        int field_count = mysql_num_fields(result);
        cout << "filed count: " <<field_count << endl;
        //得到所有字段名
        MYSQL_FIELD *field = nullptr;
        for(int i=0;i<field_count;++i) {
            field = mysql_fetch_field_direct(result, i);
            cout << field->name << "\t";
        }
    }
    // Close the Connection
    mysql_close(conn);

    return 0;
}
