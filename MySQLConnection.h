#pragma once
#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <iostream>
#include <stdexcept>


using namespace std;

class MySQLConnection
{
    SQLHENV henv; //environment
    SQLHDBC hdbc; //odbc
    SQLHSTMT hstmt; //result query
    SQLWCHAR OutConnStr[255]; //result connection
    SQLSMALLINT OutConnStrLen;

    SQLLEN cbTestStr, cbTestStr2;
    SQLFLOAT dTestFloat;
    SQLCHAR szTestStr[200], szTestStr2[200];
protected:
	static MySQLConnection* connection;
    const wchar_t connection_line[82] = L"Dsn=TelegramDataSource;uid=ast3rr;server=localhost;database=tg_bot_data;port=3306";
	MySQLConnection();
public:
	MySQLConnection(MySQLConnection& other) = delete;
	void operator=(const MySQLConnection&) = delete;
	static MySQLConnection* getInstance();
    bool selectUserById(int id);
};

