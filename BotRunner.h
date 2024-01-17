#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "StickerSetUser.h"
#include <iostream>
#include <tgbot/tgbot.h>
#include <map>
#include "Base64.h"
#include "MySQLConnection.h"
#include "Advertisment.h"
#include <iomanip>
#include <ctime>

using namespace std;
using namespace cv;

class BotRunner
{
	static MySQLConnection* connector;
	static map<string, short>usersNewPackPosition;
	static map<string, vector<StickerSetUser*>> objectsStickerSets;
	static map<string, short>usersAddPosition;
	static vector<int> idUsersForDeleteSticker;
	static bool checkNamesLenght(string name);
	static string createImage(TgBot::Message::Ptr message, TgBot::Bot& bot);
	static void fillDataFromDataBase();
	static bool checkDuplicateKeyboard(TgBot::InlineKeyboardMarkup::Ptr keyboard1, TgBot::InlineKeyboardMarkup::Ptr keyboard2);
	static void startCommand(TgBot::Bot &bot, TgBot::Message::Ptr &message);
	static void newPackCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void deleteCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void addCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void langCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void changeSetCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void deleteCommandStep(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void addCommandStep(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void newPackCommandStep4(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void newPackCommandStep3(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void newPackCommandStep2(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void newPackCommandStep1(TgBot::Bot& bot, TgBot::Message::Ptr& message);
	static void languageCallBack(TgBot::Bot& bot, const TgBot::CallbackQuery::Ptr& query);
	static void currentSetCallBack(TgBot::Bot& bot, const TgBot::CallbackQuery::Ptr& query);
	static void nextSetCallBack(TgBot::Bot& bot, const TgBot::CallbackQuery::Ptr& query);


public:
	static void run();
};

