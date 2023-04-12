#include "MySQLConnection.h"

MySQLConnection* MySQLConnection::connection = nullptr;

bool MySQLConnection::createCursor()
{
    hstmt = NULL;
    SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }
    return true;
}

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

bool MySQLConnection::checkUserById(int id)
{
    if (!createCursor()) {
        return false;
    }
    SQLINTEGER countUser;
    SQLLEN lenghtCountUser;
    wstring sqlQuery = L"SELECT COUNT(*) FROM user WHERE id_user = " + to_wstring(id);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) { 

        retcode = SQLFetch(hstmt);
        if (retcode == SQL_ERROR) {
            cout << "An error occurred";
        }
        //if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        if(SQL_SUCCEEDED(retcode)){
            SQLGetData(hstmt, 1, SQL_C_ULONG, &countUser, 0, &lenghtCountUser);
            if (countUser > 0) {
                return true;
            }
        }//todo доработать функционал
    }
    else {
        cout << "Query execution error." << endl;
    }
    return false;
}

bool MySQLConnection::createNewUser(int id)
{
    if (!createCursor()) {
        return false;
    }
    wstring sqlQuery = L"INSERT INTO user(id_user, id_lang) values(" + to_wstring(id) + L", 1)";
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    SQLWCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT i = 1;

    while (SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, i, sqlState, &nativeError, message, SQL_MAX_MESSAGE_LENGTH, NULL) == SQL_SUCCESS) {
        i++;
    }
    if (retcode == SQL_SUCCESS) {

        return true;//todo понять, почему не добавляет 
    }
    else {
        cout << "Query execution error." << endl;
    }
    return false;
}

int MySQLConnection::selectIdLang(int idUser)
{
    if (!createCursor()) {
        return 0;
    }

}

map<int, string> MySQLConnection::selectAllLangs()
{
    map<int, string> langs;
    SQLINTEGER idLang;
    SQLCHAR langName[255];
    SQLLEN lengthIdLang, lengthName;
    createCursor();

    wstring sqlQuery = L"SELECT * FROM lang;";
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        while (retcode == SQL_SUCCESS) {

            retcode = SQLFetch(hstmt);
            if (SQL_SUCCEEDED(retcode)) {
                SQLGetData(hstmt, 1, SQL_C_ULONG, &idLang, 0, &lengthIdLang);
                SQLGetData(hstmt, 2, SQL_C_CHAR, &langName, 255, &lengthName);
                string str((const char*)langName);
                langs[idLang] = str;
            }
        }
    }
    return langs;

}

bool MySQLConnection::updateLang(int idLang, int idUser)
{
    if (!createCursor()) {
        return false;
    }
    wstring sqlQuery = L"UPDATE user set id_lang = " + to_wstring(idLang) + L" WHERE id_user = " + to_wstring(idUser);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);

    return retcode == SQL_SUCCESS;
}

string MySQLConnection::translator(int idUser, string phrase)
{
    SQLLEN lengthIdLang, lengthTranslation;
    SQLWCHAR translation[255];
    int idLang;
    if (!createCursor()) {
       return phrase;
    } 
    wstring sqlQuery = L"SELECT id_lang FROM user WHERE id_user = " + to_wstring(idUser);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        retcode = SQLFetch(hstmt);
        if (SQL_SUCCEEDED(retcode)) {
            SQLGetData(hstmt, 1, SQL_C_ULONG, &idLang, 0, &lengthIdLang);
            if (idLang == 1) {
                return phrase;
            }
            else {
                if (!createCursor()) {
                    return phrase;
                }
                wstring wsTmp(phrase.begin(), phrase.end());
                sqlQuery = L"SELECT translation FROM dictionary WHERE id_lang = " + to_wstring(idLang) + L" AND phrase = '" + wsTmp + L"'";
                retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
                if (retcode == SQL_SUCCESS) {
                    retcode = SQLFetch(hstmt);
                    if (SQL_SUCCEEDED(retcode)) {
                        SQLGetData(hstmt, 1, SQL_C_WCHAR, &translation, 255, &lengthTranslation);
                        wstring str(translation);
                        str = str + L"\0";
                        string result = string(str.begin(), str.end());
                        return result;
                    }
                }
            }
        }
    }
    return phrase;
}

string MySQLConnection::WStringToString(const wstring& wstr)
{
    string str;
    size_t size;
    str.resize(wstr.length());
    wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
    return str;
}
