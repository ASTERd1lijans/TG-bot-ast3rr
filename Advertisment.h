#pragma once
#include <iostream>

using namespace std;

class Advertisment
{
	int id, send; // send 0 - ��� ��������������, � 1 - ��� ������������
	string message;
public:
	Advertisment(int id, int send, string message);
	string getMessage();
	int getId();
};

