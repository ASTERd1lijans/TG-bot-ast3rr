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
        if(SQL_SUCCEEDED(retcode)){
            SQLGetData(hstmt, 1, SQL_C_ULONG, &countUser, 0, &lenghtCountUser);
            if (countUser > 0) {
                return true;
            }
        }
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

        return true;
    }
    else {
        cout << "Query execution error." << endl;
    }
    return false;
}

int MySQLConnection::selectIdLang(int idUser)
{
    int idLang = 1;
    SQLLEN lengthIdLang;
    if (!createCursor()) {
        return idLang;
    }
    wstring sqlQuery = L"SELECT id_lang FROM user WHERE id_user = " + to_wstring(idUser);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        retcode = SQLFetch(hstmt);
        if (SQL_SUCCEEDED(retcode)) {
            SQLGetData(hstmt, 1, SQL_C_ULONG, &idLang, 0, &lengthIdLang);
        }
    }
    return idLang;
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
    //return phrase;
    SQLLEN lengthIdLang, lengthTranslation;
    SQLWCHAR translation[1000];
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
                sqlQuery = L"SELECT translation FROM dictionary WHERE id_lang = " + to_wstring(idLang) + L" AND phrase = \"" + wsTmp + L"\"";
                retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
                if (retcode == SQL_SUCCESS) {
                    retcode = SQLFetch(hstmt);
                    if (SQL_SUCCEEDED(retcode)) {
                        SQLGetData(hstmt, 1, SQL_C_WCHAR, &translation, 1000, &lengthTranslation);
                        wstring str(translation);
                        string result = wstring_to_utf8(str);
                        return result;
                    }
                }
            }
        }
    }
    return phrase;
}

vector<Advertisment> MySQLConnection::getNonSentAdvertList()
{
    SQLINTEGER idMessage;
    SQLLEN lengthIdMessage, lengthMessageText;
    SQLWCHAR messageText[500];

    vector<Advertisment>nonSentAdvert;
    createCursor();

    wstring sqlQuery = L"SELECT id, message FROM advertisment WHERE send = 0";
    SQLRETURN retcode = SQLExecDirectW(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        while (retcode == SQL_SUCCESS) {

            retcode = SQLFetch(hstmt);
            if (SQL_SUCCEEDED(retcode)) {
                SQLGetData(hstmt, 1, SQL_C_ULONG, &idMessage, 0, &lengthIdMessage);
                SQLGetData(hstmt, 2, SQL_WCHAR, &messageText, 500, &lengthMessageText);
                wstring result = (const WCHAR*)messageText;
                string str = wstring_to_utf8(result);
                Advertisment adv(idMessage, 0, str);
                nonSentAdvert.push_back(adv);
            }

        }
    }
    return nonSentAdvert;
}

void MySQLConnection::addNewAdvert(wstring phrase)
{
    createCursor();
    wstring sqlQuery = L"INSERT INTO advertisment(message) values('" + phrase + L"')";
    SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
}

string MySQLConnection::wstring_to_utf8(const wstring& str)
{
    wstring_convert<codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}
vector<int>MySQLConnection::getIdUsers() {
    createCursor();
    SQLINTEGER idUser;
    SQLLEN lengthIdUser;

    vector<int>idUsers;

    wstring sqlQuery = L"SELECT id_user FROM user;";
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        while (retcode == SQL_SUCCESS) {

            retcode = SQLFetch(hstmt);
            if (SQL_SUCCEEDED(retcode)) {
                SQLGetData(hstmt, 1, SQL_C_ULONG, &idUser, 0, &lengthIdUser);
                idUsers.push_back(idUser);
            }

        }
    }
    return idUsers;
}

void MySQLConnection::updateSend(vector<Advertisment>advert)
{
    createCursor();
    wstring queryNumbers;
    for (int i = 0; i < advert.size() - 1; i++) {
        queryNumbers += to_wstring(advert.at(i).getId()) + L", ";
    }
    queryNumbers += to_wstring(advert.at(advert.size() - 1).getId());
    wstring sqlQuery = L"UPDATE advertisment SET send = 1 WHERE id IN (" + queryNumbers + L")";
    SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);

}
/**
 * changing the step position in tg_bot_data database in table user_step
 * 
 * @param idUser - user's id in telegram
 * @param idCommand - command's id from table command
 * @param step - 0-4 where 0 is the end of the command
*/

//todo сделать для всех команд -----------------------------
void MySQLConnection::changeStepPosition(int idUser, int idCommand, int step)
{
    createCursor();
    SQLINTEGER countUser;
    SQLLEN lengthCountUser;

    wstring sqlQuery = L"SELECT COUNT(*) FROM user_step WHERE id_user = " + to_wstring(idUser) + L";";
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {

        retcode = SQLFetch(hstmt);
        if (SQL_SUCCEEDED(retcode)) {
            SQLGetData(hstmt, 1, SQL_C_ULONG, &countUser, 0, &lengthCountUser);
            if (countUser == 0) {
                createCursor();
                sqlQuery = L"INSERT INTO user_step(id_user, id_command, step) values(" + 
                    to_wstring(idUser) + L", " + to_wstring(idCommand) + L", " + to_wstring(step) + L");";
                SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
            }
            else {
                createCursor();
                sqlQuery = L"UPDATE user_step SET id_command = " + 
                    to_wstring(idCommand) + L", step = " + to_wstring(step) + L" WHERE id_user = " + 
                    to_wstring(idUser) + L";";
                SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);

            }
        }

    }

}

void MySQLConnection::saveStickerPack(int idUser, string name)
{
    createCursor();
    
    wstring nameWstring(name.begin(), name.end());

    wstring sqlQuery1 = L"INSERT INTO sticker_pack(name) values('" + nameWstring + L"'); INSERT INTO user_sticker_pack(id_user, id_sticker_pack) values(" +
        to_wstring(idUser) + L",  LAST_INSERT_ID());";

    SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery1.c_str()), SQL_NTS);

}



map<string, vector<StickerSetUser*>> MySQLConnection::getObjectsStickerSets()
{
    createCursor();
    SQLWCHAR idUser[20];
    SQLWCHAR name[100];
    SQLWCHAR title[100];
    SQLWCHAR idStickerSet[100];
    SQLLEN lengthIdUser;

    map<string, vector<StickerSetUser*>>objectsStickers;

    wstring sqlQuery = L"SELECT usp.id_user, sp.name, sp.title, usp.id_sticker_pack FROM user_sticker_pack as usp LEFT JOIN sticker_pack as sp ON usp.id_sticker_pack = sp.id_sticker_pack";
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        while (retcode == SQL_SUCCESS) {

            retcode = SQLFetch(hstmt);
            if (SQL_SUCCEEDED(retcode)) {
                SQLGetData(hstmt, 1, SQL_WCHAR, &idUser, 20, &lengthIdUser);
                SQLGetData(hstmt, 2, SQL_WCHAR, &name, 100, NULL);
                SQLGetData(hstmt, 3, SQL_WCHAR, &title, 100, NULL); 
                SQLGetData(hstmt, 4, SQL_WCHAR, &idStickerSet, 100, NULL);
                wstring str =(const WCHAR*)idUser;
                string idUserStr = wstring_to_utf8(str);
                wstring str1 = (const WCHAR*)name;
                string nameStr = wstring_to_utf8(str1);
                wstring str2 = (const WCHAR*)title;
                string titleStr = wstring_to_utf8(str2);
                wstring str3 = (const WCHAR*)idStickerSet;
                string idStickerSetStr = wstring_to_utf8(str3);
                StickerSetUser *stickerObj = new StickerSetUser(nameStr, titleStr, idUserStr);
                stickerObj->setIdStickerSet(idStickerSetStr);
                if (objectsStickers.find(idUserStr) == objectsStickers.end()) {
                    vector<StickerSetUser*>objectsStickersVec;
                    objectsStickersVec.push_back(stickerObj);
                    objectsStickers.insert({ idUserStr, objectsStickersVec });
                }
                else {
                    objectsStickers.find(idUserStr)->second.push_back(stickerObj);
                }
            }

        }
    }
    return objectsStickers;
}

bool MySQLConnection::updateCurrentPack(int idUser, int idPack)
{
    if (!createCursor()) {
        return false;
    }
    wstring sqlQuery = L"UPDATE user set id_sticker_pack = " + to_wstring(idPack) + L" WHERE id_user = " + to_wstring(idUser);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);

    return retcode == SQL_SUCCESS;
}

int MySQLConnection::getCurrentPackId(int idUser)
{
    createCursor();
    SQLINTEGER idStickerPack;
    SQLLEN lengthIdStickerPack;

    wstring sqlQuery = L"SELECT id_sticker_pack FROM user WHERE id_user = " + to_wstring(idUser);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {

        retcode = SQLFetch(hstmt);
        if (SQL_SUCCEEDED(retcode)) {
            SQLGetData(hstmt, 1, SQL_C_ULONG, &idStickerPack, 0, &lengthIdStickerPack);
        }

    }
    return idStickerPack;
}

pair<int, int> MySQLConnection::getCurrentIdCommandAndStepUser(int idUser)
{
    createCursor();

    SQLINTEGER idCommand, step;
    SQLLEN lengthIdCommand, lengthStep;

    wstring sqlQuery = L"SELECT id_command, step FROM user_step WHERE id_user = " + to_wstring(idUser);
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {

        retcode = SQLFetch(hstmt);
        if (SQL_SUCCEEDED(retcode)) {
            SQLGetData(hstmt, 1, SQL_C_ULONG, &idCommand, 0, &lengthIdCommand);
            SQLGetData(hstmt, 2, SQL_C_ULONG, &step, 0, &lengthStep);
        }

    }
    pair<int, int> commandAndStep { idCommand, step };
    return commandAndStep;
}

map<string, short> MySQLConnection::getUsersPosition(int idCommand)
{
    createCursor();
    SQLINTEGER idUser;
    SQLLEN lengthIdUser;
    SQLINTEGER step;
    SQLLEN lengthStep;

    map<string, short>result;

    wstring sqlQuery = L"SELECT id_user, step FROM user_step WHERE id_command = " + to_wstring(idCommand) + 
        L" AND step != 0; ";
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        while (retcode == SQL_SUCCESS) {

            retcode = SQLFetch(hstmt);
            if (SQL_SUCCEEDED(retcode)) {
                SQLGetData(hstmt, 1, SQL_C_ULONG, &idUser, 0, &lengthIdUser);
                SQLGetData(hstmt, 2, SQL_C_ULONG, &step, 0, &lengthStep);
                result.insert({ to_string(idUser), step });
            }

        }
    }
    return result;
}

int MySQLConnection::addNewStickerPack(StickerSetUser* stickerSet)
{
    createCursor();
    SQLINTEGER idStickerPack;
    SQLLEN lengthIdStickerPack;
    string name = stickerSet->getName();
    string title = stickerSet->getTitle();
    string id = stickerSet->getIdStickerSetTg();
    SQLINTEGER lastInsertId;

    wstring nameWstring(name.begin(), name.end());
    wstring titleWstring(title.begin(), title.end());
    wstring idWstring(id.begin(), id.end());

    wstring idUser = to_wstring(stoi(stickerSet->getIdUser()));

    wstring sqlQuery1 = L"INSERT INTO sticker_pack(name, title, id_sticker_pack_tg) values('" + nameWstring +
        L"', '" + titleWstring + L"', '" + idWstring + L"');";
    wstring sqlQuery4 = L"SELECT LAST_INSERT_ID();";
    //todo переделать для безопасности запроса
    SQLRETURN retcode = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery1.c_str()), SQL_NTS);
    if (retcode == SQL_SUCCESS) {
        SQLRETURN retcode2 = SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery4.c_str()), SQL_NTS);
        retcode2 = SQLFetch(hstmt);
        SQLGetData(hstmt, 1, SQL_C_ULONG, &lastInsertId, 0, NULL);
        wcout << idUser << endl;
        cout << lastInsertId << endl;
        wstring sqlQuery3 = L"UPDATE user SET id_sticker_pack = " + to_wstring(lastInsertId) + L" WHERE id_user = " + idUser + L"; ";
        wstring sqlQuery2 = L"INSERT INTO user_sticker_pack(id_user, id_sticker_pack) values(" +
            idUser + L",  "+ to_wstring(lastInsertId) + L");";
        createCursor();
        SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery3.c_str()), SQL_NTS);
        createCursor();
        SQLExecDirect(hstmt, const_cast<SQLWCHAR*>(sqlQuery2.c_str()), SQL_NTS);
        return lastInsertId;
    }
    return 0;
}

