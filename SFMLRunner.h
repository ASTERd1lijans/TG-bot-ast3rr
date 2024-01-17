#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "MySQLConnection.h"
#include <SFML/System/Clock.hpp>

class SFMLRunner
{
	static void saveMessage(std::wstring message);
public: 
	static void run();
	
};

