#pragma once
#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <codecvt>
#include <vector>
#include <map>
#include "Advertisment.h"
#include <algorithm>
#include "StickerSetUser.h"

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
    bool createCursor();
    string wstring_to_utf8(const wstring& str);
	static MySQLConnection* connection;
    const wchar_t connection_line[82] = L"Dsn=TelegramDataSource;uid=ast3rr;server=localhost;database=tg_bot_data;port=3306";
	MySQLConnection();
public:
	MySQLConnection(MySQLConnection& other) = delete;
	void operator=(const MySQLConnection&) = delete;
	static MySQLConnection* getInstance();
    bool checkUserById(int id);
    bool createNewUser(int id);
    int selectIdLang(int idUser);
    map<int, string> selectAllLangs();
    bool updateLang(int idLang, int idUser);
    string translator(int idUser, string phrase);
    void addNewAdvert(wstring phrase);
    vector<Advertisment>getNonSentAdvertList();
    vector<int>getIdUsers();
    void updateSend(vector<Advertisment>advert);
    int addNewStickerPack(StickerSetUser* stickerSet);
    void changeStepPosition(int idUser, int idCommand, int step);
    map<string, short>getUsersPosition(int idCommand);
    void saveStickerPack(int idUser, string name);
    map<string, vector<StickerSetUser*>> getObjectsStickerSets();
    bool updateCurrentPack(int idUser, int idPack);
    int getCurrentPackId(int idUser);
    pair<int, int> getCurrentIdCommandAndStepUser(int idUser);
};

