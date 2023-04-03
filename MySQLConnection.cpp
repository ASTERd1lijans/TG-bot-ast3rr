#include "MySQLConnection.h"

MySQLConnection* MySQLConnection::connection = nullptr;

MySQLConnection::MySQLConnection()
{
    bool errorFlag = false;
    SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
                retcode = SQLDriverConnect(
                    hdbc,
                    0,
                    (SQLWCHAR*)connection_line,
                    _countof(connection_line),
                    OutConnStr,
                    255,
                    &OutConnStrLen,
                    SQL_DRIVER_COMPLETE
                );
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
                    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
                        errorFlag = true;
                        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
                        SQLFreeHandle(SQL_HANDLE_ENV, henv);
                    }
                }
                else {
                    errorFlag = true;
                    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
                    SQLFreeHandle(SQL_HANDLE_ENV, henv);
                }

            }
            else { 
                errorFlag = true;
                SQLFreeHandle(SQL_HANDLE_ENV, henv);
            }
        }
        else { 
            errorFlag = true; 
            SQLFreeHandle(SQL_HANDLE_ENV, henv);
        }
    }
    else {
        errorFlag = true;
    }
    if (errorFlag) {
        cout << "Error";
        throw runtime_error("Error with database");
    }
}

MySQLConnection* MySQLConnection::getInstance()
{
	if (connection == nullptr) {
		connection = new MySQLConnection();
	}
	return connection;
}

bool MySQLConnection::selectUserById(int id)
{
    SQLINTEGER countUser;
    SQLLEN lenghtCountUser;
    SQLRETURN retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT COUNT(*) FROM user WHERE id_user = " + id , SQL_NTS);
    if (retcode == SQL_SUCCESS) { 

        retcode = SQLFetch(hstmt);
        if (retcode == SQL_ERROR) {
            cout << "An error occurred";
        }
        //if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        if(SQL_SUCCEEDED(retcode)){
            SQLGetData(hstmt, 1, SQL_C_ULONG, &countUser, 0, &lenghtCountUser);
            cout << countUser << ":";
        }//todo доработать функционал
    }
    else {
        cout << "Query execution error." << endl;
    }
    return false;
}
