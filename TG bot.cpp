
#include "BotRunner.h"
#include "SFMLRunner.h"
#include <thread>

int main(int argc, char** argv) {
    
    
    std::thread botThread(BotRunner::run);
    std::thread sfmlThread(SFMLRunner::run);
    botThread.join();
    sfmlThread.join();
}

