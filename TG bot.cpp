#include <stdio.h>
#include <iostream>
#include <tgbot/tgbot.h>

using namespace std;

int main() {
    TgBot::Bot bot("5860207024:AAEpaxdfaiBGFz91Ht6dtsKvE20Nz0irJ7s");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hey!");
        });
    bot.getEvents().onCommand("send", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(449972946, "/start");
        //bot.getApi().sendMessage(message->chat->id, answer->text);
        });
    bot.getEvents().onCommand("newpack", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Say me the name of your Set");
        bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
            string addedName = "_by_asterr_bot";
            string messagestr = message->text.c_str();
            //string name = messagestr.substr(9, messagestr.size()-9) + addedName;
            string name = messagestr + addedName;
            // bot.getApi().sendMessage(message->chat->id, "And now say me the short name of your Set for links");
            string title = message->text.c_str();
            title = title + " :: @asterr_bot";
            //title.replace(title.find("/newpack "), 9, "");
            // bot.getApi().sendMessage(message->chat->id, "So, send me your photo for sticker");
            string stickerPath = "lion.png";
            string mimeType = "image/png";
            TgBot::InputFile::Ptr file = TgBot::InputFile::fromFile(stickerPath, mimeType);
            bot.getApi().sendMessage(message->chat->id, "Now send me the emoji for your sticker");
            string emoji = u8"❤️‍🔥";
            bot.getApi().createNewStickerSet(message->from->id, name, title, file, emoji);
            bot.getApi().sendMessage(message->chat->id, " Here is your new stickerset!" u8"😁" "Click the link!\n Link - http://t.me/addstickers/" + name);
            });
        //bot.getApi().addStickerToSet(message->from->id, name, file, emoji);
        });


    /*bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        if (message->sticker) {
            bot.getApi().sendMessage(message->chat->id, message->sticker->emoji);
        }
        bot.getApi().sendMessage(message->chat->id, "sorry, I don't understand this phrase:)");
        });*/
        

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
