#include "StickerSetUser.h"


StickerSetUser::StickerSetUser()
{
	this->name = "";
	this->title = "";
	this->idUser = "";
}

StickerSetUser::StickerSetUser(string name, string title, string idUser)
{
	this->name = name;
	this->title = title;
	this->idUser = idUser;
}


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

string StickerSetUser::getIdStickerSetTg()
{
	return idStickerSetTg;
}

string StickerSetUser::getIdUser()
{
	return idUser;
}

string StickerSetUser::getIdStickerSet()
{
	return idStickerSet;
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

void StickerSetUser::setIdStickerSetTg(string idStickerSetTg)
{
	this->idStickerSetTg = idStickerSetTg;
}

void StickerSetUser::setIdUser(string idUser)
{
	this->idUser = idUser;
}

void StickerSetUser::setIdStickerSet(string idStickerSet)
{
	this->idStickerSet = idStickerSet;
}
