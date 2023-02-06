#include <stdio.h>
#include <fstream>
#include "StickerSetUser.h"
#include <iostream>
#include <tgbot/tgbot.h>
#include <map>
#include "Base64.h"
#include <Magick++.h>

using namespace std;
using namespace Magick;
map<string, short>usersInfo;
map<string, StickerSetUser*> objectsStickers;

bool checkNamesLenght(string name);

int main(int argc, char** argv) {
    InitializeMagick(*argv);
    TgBot::Bot bot("5860207024:AAEpaxdfaiBGFz91Ht6dtsKvE20Nz0irJ7s");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hey!");
        });
    bot.getEvents().onCommand("send", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(449972946, "/start");
        //bot.getApi().sendMessage(message->chat->id, answer->text);
        });
    bot.getEvents().onCommand("newpack", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Say me the short name of your Set for links");
        usersInfo[to_string(message->from->id)] = 1;
        objectsStickers[to_string(message->from->id)] = new StickerSetUser();
        bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
            if (usersInfo[to_string(message->from->id)] == 1) {
                string addedName = "_by_asterr_bot";
                string messagestr = message->text.c_str();
                //string name = messagestr.substr(9, messagestr.size()-9) + addedName;
                string name = messagestr + addedName;
                if (name.size() > 64) {
                    bot.getApi().sendMessage(message->chat->id, "Your name must be shorter!");
                }
                else if (messagestr.size() <= 0) {
                    bot.getApi().sendMessage(message->chat->id, "Please, input the name correctly!");
                }
                else {
                    /*if (stickerSetPtr != NULL) {
                        bot.getApi().sendMessage(message->chat->id, "This name has already taken" u8"😭");
                    }*/
                    bool flagUniqueSticker = false;
                    try {
                        TgBot::StickerSet::Ptr stickerSetPtr = bot.getApi().getStickerSet(name);
                        bot.getApi().sendMessage(message->chat->id, "This name has already taken" u8"😭");
                    }
                    catch (exception& e) {
                        flagUniqueSticker = true;
                    }
                    if (flagUniqueSticker) {
                        objectsStickers[to_string(message->from->id)]->setName(name);
                        usersInfo[to_string(message->from->id)] = 2;
                        bot.getApi().sendMessage(message->chat->id, "Now say me the name of your sticker set (1-64 symbols)");
                    }
                }
            }
            else if (usersInfo[to_string(message->from->id)] == 2) {
                string titleText = message->text.c_str();
                string extraTitle = titleText + " :: @asterr_bot";
                if (extraTitle.size() >= 64) {
                    bot.getApi().sendMessage(message->chat->id, "Your name must be shorter!");
                }
                else if (titleText.size() <= 0) {
                    bot.getApi().sendMessage(message->chat->id, "Please, input the title correctly!");
                }
                else {
                    usersInfo[to_string(message->from->id)] = 3;
                    objectsStickers[to_string(message->from->id)]->setTitle(extraTitle);
                    bot.getApi().sendMessage(message->chat->id, "Now send me the photo for your sticker");
                }
            }
            else if (usersInfo[to_string(message->from->id)] == 3) {
                string stickerId;
                //todo проверка на размер файла
                if (message->document) {
                    stickerId = message->document->fileId;
                }
                else if(message->photo.size()){
                    stickerId = message->photo[3]->fileId;
                    //cout << result.encode();
                   
                }
                
                string mimeType = "image/png"; //todo проверить формат и отредактировть и проверить размер и отредактировать 
                //TgBot::InputFile::Ptr file = TgBot::InputFile::fromFile(stickerPath, mimeType);
                TgBot::File::Ptr stickerFile = bot.getApi().getFile(stickerId);
                string file = bot.getApi().downloadFile(stickerFile->filePath);
                //cout << file;
                objectsStickers[to_string(message->from->id)]->setImagePath(stickerFile->filePath);
                ofstream fileImage(stickerFile->filePath, std::ios::binary);
                fileImage << file;
                fileImage.close();
                /*Image imageForSticker(stickerFile->filePath);
                Blob blob;
                imageForSticker.magick("PNG");
                imageForSticker.write(&blob);
                Image resultImage(blob);
                string result = stickerFile->filePath.substr(0, stickerFile->filePath.find(".")) + ".png";
                resultImage.write(result);*/
                usersInfo[to_string(message->from->id)] = 4;
                bot.getApi().sendMessage(message->chat->id, "Now send me the emoji for your sticker");
            }
            else if (usersInfo[to_string(message->from->id)] == 4) {
                string emoji = message->text;
                string name = objectsStickers[to_string(message->from->id)]->getName();
                string title = objectsStickers[to_string(message->from->id)]->getTitle();
                string filePath = objectsStickers[to_string(message->from->id)]->getImagePath();
                try {
                    bot.getApi().createNewStickerSet(message->from->id, name, title, filePath, emoji);
                    bot.getApi().sendMessage(message->chat->id, " Here is your new stickerset!" u8"😁" "Click the link!\nLink - http://t.me/addstickers/" + name);
                    usersInfo[to_string(message->from->id)] = 0;
                }
                catch (TgBot::TgException& e) {
                    bot.getApi().sendMessage(message->chat->id, e.what());
                }
            }
            
        });

    });

 
    
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) { 
        printf("error: %s\n", e.what());
    }
    return 0;
}

bool checkNamesLenght(string name) {
    return name.size() <= 64;
}