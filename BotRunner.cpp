#include "BotRunner.h"


MySQLConnection* BotRunner::connector = nullptr;
map<string, short> BotRunner::usersNewPackPosition = {};
map<string, vector<StickerSetUser*>> BotRunner::objectsStickerSets = {};
map<string, short> BotRunner::usersAddPosition = {};
vector<int> BotRunner::idUsersForDeleteSticker = {};


void BotRunner::run()
{
    TgBot::Bot bot("5860207024:AAEpaxdfaiBGFz91Ht6dtsKvE20Nz0irJ7s");
    connector = MySQLConnection::getInstance();
    ofstream logFile;

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {

        startCommand(bot, message);

        });
    bot.getEvents().onCommand("newpack", [&bot](TgBot::Message::Ptr message) {
        
        newPackCommand(bot, message);

        });
    bot.getEvents().onCommand("delete", [&bot](TgBot::Message::Ptr message) {
        
        deleteCommand(bot, message);

        });
    bot.getEvents().onCommand("changeset", [&bot](TgBot::Message::Ptr message) {
        
        changeSetCommand(bot, message);

        });

    bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
        if (usersNewPackPosition.find(to_string(message->from->id)) == usersNewPackPosition.end() &&
            usersAddPosition.find(to_string(message->from->id)) == usersAddPosition.end()) {

            deleteCommandStep(bot, message);
        }
        else if (usersNewPackPosition.find(to_string(message->from->id)) != usersNewPackPosition.end() &&
            usersNewPackPosition.find(to_string(message->from->id))->second != 0) {
            if (usersNewPackPosition[to_string(message->from->id)] == 4) {
                newPackCommandStep4(bot, message);
                
            }
            else if (usersNewPackPosition[to_string(message->from->id)] == 3) {
                newPackCommandStep3(bot, message);
            }
            else if (usersNewPackPosition[to_string(message->from->id)] == 2) {
                newPackCommandStep2(bot, message);
            }
            else if (usersNewPackPosition[to_string(message->from->id)] == 1) {
                newPackCommandStep1(bot, message);
            }
        }
        else if (usersAddPosition.find(to_string(message->from->id)) != usersAddPosition.end() &&
            usersAddPosition.find(to_string(message->from->id))->second != 0) {

            addCommandStep(bot, message);
        }
        
        });

    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        string textMessage = connector->translator(message->from->id, "ASTER bot's commands:\n/newpack - creating a new sticker pack\n/add - adding the new sticker to a pack\n/delete - deleting the sticker from the pack\n/lang - changing the language\n/changeset - changing current pack");
        bot.getApi().sendMessage(message->chat->id, textMessage);
        });

    bot.getEvents().onCommand("add", [&bot](TgBot::Message::Ptr message) {
        addCommand(bot, message);
        });

    bot.getEvents().onCommand("lang", [&bot](TgBot::Message::Ptr message) {
        langCommand(bot, message);
        });
    bot.getEvents().onCallbackQuery([&bot](const TgBot::CallbackQuery::Ptr& query) {
        if (query->data.find("Language: ") != std::string::npos) {
            languageCallBack(bot, query);
        }
        else if (query->data.find("Current set: ") != std::string::npos) {
            currentSetCallBack(bot, query);
        }
        else if (query->data.find("Next sets: ") != std::string::npos) {
            nextSetCallBack(bot, query);
        }
                
        });
    while (true) {
        try {
            fillDataFromDataBase();
            printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
            TgBot::TgLongPoll longPoll(bot);
            while (true) {
                printf("Long poll started\n");
                vector<Advertisment>nonSentAdv = connector->getNonSentAdvertList();
                vector<int>idUsers = connector->getIdUsers();
                if (nonSentAdv.size() != 0) {
                    for (int i = 0; i < nonSentAdv.size(); i++) {
                        for (int j = 0; j < idUsers.size(); j++) {
                            bot.getApi().sendMessage(idUsers.at(j), nonSentAdv.at(i).getMessage());
                        }
                    }
                    connector->updateSend(nonSentAdv);
                }
                longPoll.start();
                //throw exception();
            }
        }
        catch (TgBot::TgException& e) {
            printf("error: %s\n", e.what());
            logFile.open("logFile.txt", ios_base::app);
            auto t = std::time(nullptr);
            auto tm = *localtime(&t);
            logFile << put_time(&tm, "%d-%m-%Y %H-%M-%S") <<" " << e.what() << endl;
            logFile.close();
        }
        catch (exception& e) {
            printf("error: %s\n", e.what());
            logFile.open("logFile.txt", ios_base::app);
            auto t = std::time(nullptr);
            auto tm = *localtime(&t);
            logFile << put_time(&tm, "%d-%m-%Y %H-%M-%S") << " " << e.what() << endl;
            logFile.close();
        }
    }
}

bool BotRunner::checkNamesLenght(string name) {
    return name.size() <= 64;
}

string BotRunner::createImage(TgBot::Message::Ptr message, TgBot::Bot& bot) {
    string stickerId;
    if (message->document) {
        stickerId = message->document->fileId;
    }
    else if (message->photo.size()) {
        stickerId = message->photo.back()->fileId;

    }
    else { return ""; }

    string mimeType = "image/png";
    TgBot::File::Ptr stickerFile = bot.getApi().getFile(stickerId);
    string filePath = stickerFile->filePath;
    if (filePath.find(".webm") != string::npos || filePath.find(".tgs") != string::npos) {//todo добавить анимированые стикеры
        return "";
    }
    string file = bot.getApi().downloadFile(filePath);
    
    ofstream fileImage(filePath, std::ios::binary);
    fileImage << file;
    fileImage.close();
    Mat image = imread(filePath);
    int width = image.cols;
    int height = image.rows;
    int newWidth, newHeight;
 
    if (width >= height) {
        newWidth = 512;
        newHeight = newWidth * height / width;
    }
    else {
        newHeight = 512;
        newWidth = newHeight * width / height;
    }
    Mat newImage;
    resize(image, newImage, Size(newWidth, newHeight), INTER_LINEAR);
    string fileName = filePath.substr(0, filePath.find_last_of(".")) + ".png";
    imwrite(fileName, newImage);
    return fileName;
}

void BotRunner::fillDataFromDataBase()
{
    usersNewPackPosition = connector->getUsersPosition(1);
    usersAddPosition = connector->getUsersPosition(2);
    map<string, short>deleteIdMap = connector->getUsersPosition(3);
    for (auto const& imap : deleteIdMap) {
        idUsersForDeleteSticker.push_back(stoi(imap.first));
    }
    objectsStickerSets = connector->getObjectsStickerSets();
}

bool BotRunner::checkDuplicateKeyboard(TgBot::InlineKeyboardMarkup::Ptr keyboard1, TgBot::InlineKeyboardMarkup::Ptr keyboard2)
{
    vector<vector<TgBot::InlineKeyboardButton::Ptr>> kb1 = keyboard1->inlineKeyboard;
    vector<vector<TgBot::InlineKeyboardButton::Ptr>> kb2 = keyboard2->inlineKeyboard;
    if (kb1.size() != kb2.size()) {
        return false;
    }
    for (int i = 0; i < kb1.size(); i++)
    {
        if (kb1.at(i).size() != kb2.at(i).size()) {
            return false;
        }
    }
    for (int i = 0; i < kb1.size(); i++) {
        for (int j = 0; j < kb1.at(i).size(); j++) {
            if (kb1.at(i).at(j)->text != kb2.at(i).at(j)->text) {
                return false;
            }
        }
    }
    return true;
}

void BotRunner::startCommand(TgBot::Bot& bot, TgBot::Message::Ptr &message)
{
    if (!connector->checkUserById(message->from->id)) {
        connector->createNewUser(message->from->id);
    }
    string textMessage = connector->translator(message->from->id, "Hey!");

    bot.getApi().sendMessage(message->chat->id, textMessage);

}

void BotRunner::newPackCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    string textMessage = connector->translator(message->from->id, "Say me the short name of your Set for links");

    bot.getApi().sendMessage(message->chat->id, textMessage);
    usersNewPackPosition[to_string(message->from->id)] = 1;
    connector->changeStepPosition(message->from->id, 1, 1);
    objectsStickerSets[to_string(message->from->id)].push_back(new StickerSetUser());
}

void BotRunner::deleteCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    printf("Sticker was deleted\n");
    string textMessage = connector->translator(message->from->id, "Send me a sticker to delete");
    bot.getApi().sendMessage(message->chat->id, textMessage);
    idUsersForDeleteSticker.push_back(message->chat->id);
    connector->changeStepPosition(message->from->id, 3, 1);
}

void BotRunner::addCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    if (objectsStickerSets.find(to_string(message->from->id)) != objectsStickerSets.end()) {
        vector<StickerSetUser*> v = objectsStickerSets.find(to_string(message->from->id))->second;
        int idCurrentStickerPack = connector->getCurrentPackId(message->from->id);
        auto it = find_if(v.begin(), v.end(), [&idCurrentStickerPack](StickerSetUser* obj)
            {return obj->getIdStickerSet() == to_string(idCurrentStickerPack); });
        StickerSetUser* lastStickerSet;
        if (it == v.end()) {
            lastStickerSet = v.at(v.size() - 1);
        }
        else {
            lastStickerSet = *it;
        }
        string nameOfLastSet = lastStickerSet->getName();
        string textMessage = connector->translator(message->from->id, "You are on the latest created Set now - " + nameOfLastSet + ". Use /changeset to change your set");

        bot.getApi().sendMessage(message->chat->id, textMessage);
        //bot.getApi().sendMessage(message->chat->id, "You are on the latest created Set now - " + nameOfLastSet + ". Use /changeset to change your set");
        usersAddPosition[to_string(message->from->id)] = 1;
        textMessage = connector->translator(message->from->id, "Send me the photo for your sticker");

        bot.getApi().sendMessage(message->chat->id, textMessage);
        //bot.getApi().sendMessage(message->chat->id, "Send me the photo for your sticker");
        connector->changeStepPosition(message->from->id, 2, 1);
    }
    else {
        string textMessage = connector->translator(message->from->id, "You don't have any Sticker Sets. Use /newpack to create it");

        bot.getApi().sendMessage(message->chat->id, textMessage);
        //bot.getApi().sendMessage(message->chat->id, "You don't have any Sticker Sets. Use /newpack to create it");
    }
}

void BotRunner::langCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    map<int, string>langs = connector->selectAllLangs();
    if (langs.empty()) {
        string textMessage = connector->translator(message->from->id, "Sorry, some troubles with languages. Try later;)");

        bot.getApi().sendMessage(message->chat->id, textMessage);
        //bot.getApi().sendMessage(message->chat->id, "Sorry, some troubles with languages. Try later;)");
    }
    else {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        vector<TgBot::InlineKeyboardButton::Ptr> row;

        for (map<int, string>::iterator iter = langs.begin(); iter != langs.end(); ++iter) {
            row.clear();
            TgBot::InlineKeyboardButton::Ptr KeyboardButton(new TgBot::InlineKeyboardButton);
            KeyboardButton->text = iter->second;
            KeyboardButton->callbackData = ("Language: " + to_string(iter->first));
            row.push_back(KeyboardButton);
            keyboard->inlineKeyboard.push_back(row);
        }
        string textMessage = connector->translator(message->from->id, "Select your language");

        bot.getApi().sendMessage(message->chat->id, textMessage, false, 0, keyboard);
    }
}

void BotRunner::changeSetCommand(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    map<string, vector < StickerSetUser*>>::iterator findStickers = objectsStickerSets.find(to_string(message->from->id));
    if (findStickers != objectsStickerSets.end() && findStickers->second.size() != 0) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        vector<TgBot::InlineKeyboardButton::Ptr> row;
        int leftStickers = 4;
        if (4 > findStickers->second.size()) {
            leftStickers = findStickers->second.size() % 4;
        }

        for (vector < StickerSetUser*>::iterator iter = findStickers->second.begin(); iter < findStickers->second.begin() + leftStickers; ++iter) {
            row.clear();
            TgBot::InlineKeyboardButton::Ptr KeyboardButton(new TgBot::InlineKeyboardButton);
            KeyboardButton->text = (*iter)->getName();
            KeyboardButton->callbackData = "Current set: " + (*iter)->getIdStickerSet();
            row.push_back(KeyboardButton);
            keyboard->inlineKeyboard.push_back(row);
        }
        if (findStickers->second.size() > 4) {
            row.clear();
            TgBot::InlineKeyboardButton::Ptr KeyboardButton(new TgBot::InlineKeyboardButton);
            KeyboardButton->text = u8"▶️";
            KeyboardButton->callbackData = "Next sets: 4";
            row.push_back(KeyboardButton);
            keyboard->inlineKeyboard.push_back(row);
        }
        string textMessage = connector->translator(message->from->id, "Select your Set");
        bot.getApi().sendMessage(message->chat->id, textMessage, false, 0, keyboard);
    }
    else {
        string textMessage = connector->translator(message->from->id, "Sorry. You don't have any packs. Use /newpack to create it.");
        bot.getApi().sendMessage(message->chat->id, textMessage);
    }
}

void BotRunner::deleteCommandStep(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    if (find(idUsersForDeleteSticker.begin(), idUsersForDeleteSticker.end(), message->from->id) == idUsersForDeleteSticker.end()) {
        string textMessage = connector->translator(message->from->id, "You can use /help to see all commands");

        bot.getApi().sendMessage(message->chat->id, textMessage);
    }
    else if (message->sticker != NULL) {
        cout << "STICKER";
        bool resultDelete = false;
        try {
            resultDelete = bot.getApi().deleteStickerFromSet(message->sticker->fileId);
        }
        catch (exception e) {
            string textMessage = connector->translator(message->from->id, "Error. The sticker wasn't deleted");

            bot.getApi().sendMessage(message->chat->id, textMessage);
        }
        if (resultDelete) {
            string textMessage = connector->translator(message->from->id, "Your sticker was deleted");

            bot.getApi().sendMessage(message->chat->id, textMessage);
        }
        else {
            string textMessage = connector->translator(message->from->id, "Error. The sticker wasn't deleted");

            bot.getApi().sendMessage(message->chat->id, textMessage);
        }
        connector->changeStepPosition(message->from->id, 3, 0);
    }
}

void BotRunner::addCommandStep(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    if (usersAddPosition[to_string(message->from->id)] == 1) {
        string fileName = createImage(message, bot);
        if (fileName == "") {
            string textMessage = connector->translator(message->from->id, "Some troubles with your image. Try again");

            bot.getApi().sendMessage(message->chat->id, textMessage);
        }
        else {
            objectsStickerSets[to_string(message->from->id)].back()->setImagePath(fileName);
            usersAddPosition[to_string(message->from->id)] = 2;
            string textMessage = connector->translator(message->from->id, "Now send me the emoji for your sticker");
            connector->changeStepPosition(message->from->id, 2, 2);
            bot.getApi().sendMessage(message->chat->id, textMessage);
            //bot.getApi().sendMessage(message->chat->id, "Now send me the emoji for your sticker");
        }
    }
    else if (usersAddPosition[to_string(message->from->id)] == 2) {
        cout << "INSIDE";
        string emoji = message->text;
        vector<StickerSetUser*> v = objectsStickerSets.find(to_string(message->from->id))->second;
        int idCurrentStickerPack = connector->getCurrentPackId(message->from->id);
        auto it = find_if(v.begin(), v.end(), [&idCurrentStickerPack](StickerSetUser* obj)
            {return obj->getIdStickerSet() == to_string(idCurrentStickerPack); });
        StickerSetUser* lastStickerSet;
        if (it == v.end()) {
            lastStickerSet = v.at(v.size() - 1);
        }
        else {
            lastStickerSet = *it;
        }
        string name = lastStickerSet->getName();
        string filePath = objectsStickerSets[to_string(message->from->id)].back()->getImagePath();
        TgBot::InputFile::Ptr file = TgBot::InputFile::fromFile(filePath, "image/png");
        try {
            bot.getApi().addStickerToSet(message->from->id, name, file, emoji);
            string textMessage = connector->translator(message->from->id, "Your sticker was added to the set\nLink - http://t.me/addstickers/" + name);

            bot.getApi().sendMessage(message->chat->id, textMessage);
            usersAddPosition.erase(to_string(message->from->id));
            connector->changeStepPosition(message->from->id, 2, 0);
        }
        catch (TgBot::TgException& e) {
            bot.getApi().sendMessage(message->chat->id, e.what());
            string textMessage = connector->translator(message->from->id, "Some errors. Try again");

            bot.getApi().sendMessage(message->chat->id, textMessage);
        }
    }
}

void BotRunner::newPackCommandStep4(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    string textMessage;
    string emoji = message->text;
    string name = objectsStickerSets[to_string(message->from->id)].back()->getName();
    string title = objectsStickerSets[to_string(message->from->id)].back()->getTitle();
    string filePath = objectsStickerSets[to_string(message->from->id)].back()->getImagePath();
    if (filePath == "") {
        textMessage = connector->translator(message->from->id, "Some errors. Try again");
    }
    else {
        TgBot::InputFile::Ptr file = TgBot::InputFile::fromFile(filePath, "image/png");
        try {
            objectsStickerSets[to_string(message->from->id)].back()->setIdUser(to_string(message->from->id));
            objectsStickerSets[to_string(message->from->id)].back()->setIdStickerSetTg("");
            bot.getApi().createNewStickerSet(message->from->id, name, title, file, emoji);
            int lastPackId = connector->addNewStickerPack(objectsStickerSets[to_string(message->from->id)].back());
            if (lastPackId != 0) {
                objectsStickerSets[to_string(message->from->id)].back()->setIdStickerSet(to_string(lastPackId));
                textMessage = connector->translator(message->from->id, "Here is your new stickerset!");
                textMessage += u8"😁";
                textMessage += connector->translator(message->from->id, "Click the link!\nLink - http://t.me/addstickers/") + name;
            }
            else {
                textMessage = connector->translator(message->from->id, "Some errors. Try again");
            }


        }
        catch (TgBot::TgException& e) {
            bot.getApi().sendMessage(message->chat->id, e.what());
            textMessage = connector->translator(message->from->id, "Some errors. Try again");

        }
    }
    usersNewPackPosition.erase(to_string(message->from->id));
    connector->changeStepPosition(message->from->id, 1, 0);

    bot.getApi().sendMessage(message->chat->id, textMessage);
}

void BotRunner::newPackCommandStep3(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    string fileName = createImage(message, bot);
    if (fileName == "") {
        string textMessage = connector->translator(message->from->id, "Some troubles with your image. Try again");

        bot.getApi().sendMessage(message->chat->id, textMessage);

    }
    else {
        objectsStickerSets[to_string(message->from->id)].back()->setImagePath(fileName);
        usersNewPackPosition[to_string(message->from->id)] = 4;
        connector->changeStepPosition(message->from->id, 1, 4);
        string textMessage = connector->translator(message->from->id, "Now send me the emoji for your sticker");

        bot.getApi().sendMessage(message->chat->id, textMessage);

    }
}

void BotRunner::newPackCommandStep2(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    string titleText = message->text.c_str();
    string extraTitle = titleText + " :: @asterr_bot";
    if (extraTitle.size() >= 64) {
        string textMessage = connector->translator(message->from->id, "Your name must be shorter!");

        bot.getApi().sendMessage(message->chat->id, textMessage);

    }
    else if (titleText.size() <= 0) {
        string textMessage = connector->translator(message->from->id, "Please, input the title correctly!");

        bot.getApi().sendMessage(message->chat->id, textMessage);

    }
    else {
        usersNewPackPosition[to_string(message->from->id)] = 3;
        connector->changeStepPosition(message->from->id, 1, 3);
        objectsStickerSets[to_string(message->from->id)].back()->setTitle(extraTitle);
        string textMessage = connector->translator(message->from->id, "Now send me the photo for your sticker");

        bot.getApi().sendMessage(message->chat->id, textMessage);

    }
}

void BotRunner::newPackCommandStep1(TgBot::Bot& bot, TgBot::Message::Ptr& message)
{
    string addedName = "_by_asterr_bot";
    string messagestr = message->text.c_str();

    string name = messagestr + addedName;
    if (name.size() > 64) {
        string textMessage = connector->translator(message->from->id, "Your name must be shorter!");

        bot.getApi().sendMessage(message->chat->id, textMessage);

    }
    else if (messagestr.size() <= 0) {
        string textMessage = connector->translator(message->from->id, "Please, input the name correctly!");

        bot.getApi().sendMessage(message->chat->id, textMessage);
        //bot.getApi().sendMessage(message->chat->id, "Please, input the name correctly!");
    }
    else {

        bool flagUniqueSticker = false;
        try {
            TgBot::StickerSet::Ptr stickerSetPtr = bot.getApi().getStickerSet(name);
            string textMessage = connector->translator(message->from->id, "This name has already taken") + u8"😭";

            bot.getApi().sendMessage(message->chat->id, textMessage);
            //bot.getApi().sendMessage(message->chat->id, "This name has already taken" u8"😭");
        }
        catch (exception& e) {
            flagUniqueSticker = true;
        }
        if (flagUniqueSticker) {
            objectsStickerSets[to_string(message->from->id)].back()->setName(name);
            string textMessage = connector->translator(message->from->id, "Now say me the name of your sticker set (1-64 symbols)");
            connector->saveStickerPack(message->from->id, name);
            bot.getApi().sendMessage(message->chat->id, textMessage);
            usersNewPackPosition[to_string(message->from->id)] = 2;
            connector->changeStepPosition(message->from->id, 1, 2);

        }
    }
}

void BotRunner::languageCallBack(TgBot::Bot& bot, const TgBot::CallbackQuery::Ptr& query)
{
    cout << query->message->chat->id;
    int chatId = query->message->chat->id;
    string ans = query->data.substr(10);
    if (connector->updateLang(stoi(ans), chatId)) {
        string textMessage = connector->translator(chatId, "Lang was changed");
        bot.getApi().sendMessage(chatId, textMessage);
    }
    else {

        string textMessage = connector->translator(chatId, "Some errors. Try again");

        bot.getApi().sendMessage(chatId, textMessage);
    }
}

void BotRunner::currentSetCallBack(TgBot::Bot& bot, const TgBot::CallbackQuery::Ptr& query)
{
    int chatId = query->message->chat->id;
    string ans = query->data.substr(13);
    if (connector->updateCurrentPack(chatId, stoi(ans))) {
        string textMessage = connector->translator(chatId, "Your set was changed to ");
        int idCurrentStickerPack = stoi(ans);
        vector<StickerSetUser*> v = objectsStickerSets.find(to_string(chatId))->second;
        auto it = find_if(v.begin(), v.end(), [&idCurrentStickerPack](StickerSetUser* obj)
            {return obj->getIdStickerSet() == to_string(idCurrentStickerPack); });
        StickerSetUser* lastStickerSet;
        if (it == v.end()) {
            lastStickerSet = v.at(v.size() - 1);
        }
        else {
            lastStickerSet = *it;
        }
        string nameOfLastSet = lastStickerSet->getName();
        textMessage += u8" " + nameOfLastSet;
        bot.getApi().sendMessage(chatId, textMessage);
    }
    else {

        string textMessage = connector->translator(chatId, "Some errors. Try again");

        bot.getApi().sendMessage(chatId, textMessage);
    }
}

void BotRunner::nextSetCallBack(TgBot::Bot& bot, const TgBot::CallbackQuery::Ptr& query)
{
    int countNextSet = stoi(query->data.substr(11));
    cout << countNextSet << endl;
    map<string, vector < StickerSetUser*>>::iterator findStickers = objectsStickerSets.find(to_string(query->message->chat->id));
    if (findStickers != objectsStickerSets.end() && findStickers->second.size() != 0) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        vector<TgBot::InlineKeyboardButton::Ptr> row;
        int leftStickers = 4;
        if (countNextSet + 4 > findStickers->second.size()) {
            leftStickers = findStickers->second.size() % 4;
        }

        for (vector < StickerSetUser*>::iterator iter = findStickers->second.begin() + countNextSet; iter < findStickers->second.begin() + leftStickers + countNextSet; ++iter) {
            row.clear();
            TgBot::InlineKeyboardButton::Ptr KeyboardButton(new TgBot::InlineKeyboardButton);
            KeyboardButton->text = (*iter)->getName();
            KeyboardButton->callbackData = "Current set: " + (*iter)->getIdStickerSet();
            row.push_back(KeyboardButton);
            keyboard->inlineKeyboard.push_back(row);
        }
        row.clear();
        if (countNextSet - 4 >= 0) {
            TgBot::InlineKeyboardButton::Ptr KeyboardButton(new TgBot::InlineKeyboardButton);
            KeyboardButton->text = u8"◀️";
            KeyboardButton->callbackData = "Next sets: " + to_string(countNextSet - 4);
            row.push_back(KeyboardButton);
        }
        if (findStickers->second.size() > countNextSet + 4) {

            TgBot::InlineKeyboardButton::Ptr KeyboardButton(new TgBot::InlineKeyboardButton);
            KeyboardButton->text = u8"▶️";
            KeyboardButton->callbackData = "Next sets: " + to_string(countNextSet + 4);
            row.push_back(KeyboardButton);

        }
        keyboard->inlineKeyboard.push_back(row);

        string textMessage = connector->translator(query->message->from->id, "Select your Set");

        if (!checkDuplicateKeyboard(keyboard, query->message->replyMarkup)) {
            try {
                bot.getApi().editMessageText(textMessage, query->message->chat->id, query->message->messageId, "", "", false, keyboard);
            }
            catch (TgBot::TgException e) {
                cout << "E. Double click" << endl;
            }
        }
    }
}
