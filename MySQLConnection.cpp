#include "MySQLConnection.h"

MySQLConnection::MySQLConnection()
{
    bool errorFlag = false;
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
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
                    255,
                    OutConnStr,
                    255,
                    &OutConnStrLen,
                    SQL_DRIVER_COMPLETE
                );
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                   /* retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
                    retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT * FROM actor limit 50", SQL_NTS);
                    if (retcode == SQL_SUCCESS) {

                        while (TRUE) {
                            retcode = SQLFetch(hstmt);
                            if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
                                cout << "An error occurred";
                            }
                            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                                SQLGetData(hstmt, 1, SQL_C_ULONG, &sTestInt, 0, &cbTestInt);
                                SQLGetData(hstmt, 2, SQL_C_CHAR, szTestStr, 200, &cbTestStr);
                                SQLGetData(hstmt, 3, SQL_C_CHAR, szTestStr2, 200, &cbTestStr2);
                                cout << "Row " << sTestInt << ":";
                                cout << szTestStr << " ";
                                cout << szTestStr2 << endl;
                                iCount++;
                            }
                            else {
                                break;
                            }
                        }
                        cout << iCount - 1 << endl;
                    }
                    else {
                        cout << "Query execution error." << endl;
                    }*/

                    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                    SQLDisconnect(hdbc);
                }
                else {
                    cout << "Connection error" << endl;
                }
                SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            }
            else { errorFlag = true; }
        }
        else { errorFlag = true; }

        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
    else {
        errorFlag = true;
    }
    if (errorFlag) {
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
