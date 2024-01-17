#include "Advertisment.h"

Advertisment::Advertisment(int id, int send, string message)
{
	this->id = id;
	this->message = message;
	this->send = send;
}

string Advertisment::getMessage()
{
	return this->message;
}

int Advertisment::getId()
{
	return id;
}
