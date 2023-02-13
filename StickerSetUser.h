#pragma once
#include <iostream>

using namespace std;

class StickerSetUser
{
	string name, title, emoji, imagePath;
public:
	string getName();
	string getTitle();
	string getEmoji();
	string getImagePath();
	void setName(string name);
	void setTitle(string title);
	void setEmoji(string emoji);
	void setImagePath(string imagePath);
};

