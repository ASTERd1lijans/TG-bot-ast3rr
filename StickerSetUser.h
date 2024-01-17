#pragma once
#include <iostream>

using namespace std;

class StickerSetUser
{
	string name, title, emoji, imagePath, idStickerSetTg, idUser, idStickerSet;
public:
	StickerSetUser();
	StickerSetUser(string name, string title, string idUser);
	string getName();
	string getTitle();
	string getEmoji();
	string getImagePath();
	string getIdStickerSetTg();
	string getIdUser();
	string getIdStickerSet();
	void setName(string name);
	void setTitle(string title);
	void setEmoji(string emoji);
	void setImagePath(string imagePath);
	void setIdStickerSetTg(string idStickerSetTg);
	void setIdUser(string idUser);
	void setIdStickerSet(string idStickerSet);
};

