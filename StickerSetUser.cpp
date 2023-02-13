#include "StickerSetUser.h"

string StickerSetUser::getName()
{
	return name;
}

string StickerSetUser::getTitle()
{
	return title;
}

string StickerSetUser::getEmoji()
{
	return emoji;
}

string StickerSetUser::getImagePath()
{
	return imagePath;
}

void StickerSetUser::setName(string name)
{
	this->name = name;
}

void StickerSetUser::setTitle(string title)
{
	this->title = title;
}

void StickerSetUser::setEmoji(string emoji)
{
	this->emoji = emoji;
}

void StickerSetUser::setImagePath(string imagePath)
{
	this->imagePath = imagePath;
}
