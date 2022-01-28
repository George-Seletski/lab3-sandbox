#include <iostream>
#include "sqlite3.h"
#include <string>

using namespace std;

int logging_cl(string user, string stg)
{
    int counter = 0;

    char* err;

    sqlite3* db_e;
    //sqlite3_stmt* stmt;

    sqlite3_open("ev.sqlite", &db_e);

    string sql = "CREATE TABLE IF NOT EXISTS CLIENTS (id TEXT, "\
        "stage TEXT,"\
        "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    int rc = sqlite3_exec(db_e, sql.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        cout << "sql_E:" << err << "\n";
    }
    else {
        string sqlstatement = "INSERT INTO CLIENTS (id,stage) VALUES ('" + user + "','" + stg + "'); ";

        rc = sqlite3_exec(db_e, sqlstatement.c_str(), NULL, NULL, &err);

    }




    sqlite3_close(db_e);


    return 0;

}