#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "StickerSetUser.h"
#include <iostream>
#include <tgbot/tgbot.h>
#include <map>
#include "Base64.h"

using namespace std;
using namespace cv;

map<string, short>usersInfo;
map<string, StickerSetUser*> objectsStickers;

bool checkNamesLenght(string name);

int main(int argc, char** argv) {
    TgBot::Bot bot("5860207024:AAEpaxdfaiBGFz91Ht6dtsKvE20Nz0irJ7s");
    /*Mat image = imread("file0.jpg");
    imwrite("file_0.png", image);
    int down_width = 512;
    int down_height = 512;
    Mat resized_down;
    int cols = image.cols;  int rows = image.rows;*/

    /*resize(image, resized_down, Size(down_width, down_height), INTER_LINEAR);
    imwrite("resizedFile.png", resized_down);*/
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
    });

    bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
        if (usersInfo.find(to_string(message->from->id)) == usersInfo.end()) {
            bot.getApi().sendMessage(message->chat->id, "You can use /help to see all commands");
        }
        else {
            if (usersInfo[to_string(message->from->id)] == 4) {
                string emoji = message->text;
                string name = objectsStickers[to_string(message->from->id)]->getName();
                string title = objectsStickers[to_string(message->from->id)]->getTitle();
                string filePath = objectsStickers[to_string(message->from->id)]->getImagePath();
                TgBot::InputFile::Ptr file = TgBot::InputFile::fromFile(filePath, "image/png");
                try {
                    bot.getApi().createNewStickerSet(message->from->id, name, title, file, emoji);
                    bot.getApi().sendMessage(message->chat->id, " Here is your new stickerset!" u8"😁" "Click the link!\nLink - http://t.me/addstickers/" + name);
                    usersInfo.erase(to_string(message->from->id));
                }
                catch (TgBot::TgException& e) {
                    bot.getApi().sendMessage(message->chat->id, e.what());
                }
            }
            else if (usersInfo[to_string(message->from->id)] == 3) {
                string stickerId;
                //todo проверка на размер файла
                if (message->document) {
                    stickerId = message->document->fileId;
                }
                else if (message->photo.size()) {
                    stickerId = message->photo.back()->fileId;
                    //cout << result.encode();

                }

                string mimeType = "image/png"; //todo проверить формат и отредактировть и проверить размер и отредактировать 
                //TgBot::InputFile::Ptr file = TgBot::InputFile::fromFile(stickerPath, mimeType);
                TgBot::File::Ptr stickerFile = bot.getApi().getFile(stickerId);
                string file = bot.getApi().downloadFile(stickerFile->filePath);
                //cout << file;


                ofstream fileImage(stickerFile->filePath, std::ios::binary);
                fileImage << file;
                fileImage.close();
                Mat image = imread(stickerFile->filePath);
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
                string fileName = stickerFile->filePath.substr(0, stickerFile->filePath.find_last_of(".")) + ".png";
                imwrite(fileName, newImage);
                objectsStickers[to_string(message->from->id)]->setImagePath(fileName);//stickerFile->filePath);
                usersInfo[to_string(message->from->id)] = 4;
                bot.getApi().sendMessage(message->chat->id, "Now send me the emoji for your sticker");
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
            else if (usersInfo[to_string(message->from->id)] == 1) {
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
                        bot.getApi().sendMessage(message->chat->id, "Now say me the name of your sticker set (1-64 symbols)");
                        usersInfo[to_string(message->from->id)] = 2;

                    }
                }
            }
        }
    });
    
    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "ASTER bot's commands:\n/newpack - creating a new sticker pack\n/add - adding the new sticker to a pack\n/delete - deleting the sticker from the pack ");
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