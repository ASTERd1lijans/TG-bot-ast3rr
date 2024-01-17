#pragma once
#include <iostream>

using namespace std;

class Advertisment
{
	int id, send; // send 0 - это неотправленный, а 1 - это отправленный
	string message;
public:
	Advertisment(int id, int send, string message);
	string getMessage();
	int getId();
};

