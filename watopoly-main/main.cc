#include <iostream>
#include <memory>
#include <string>
#include "game.h"

int main(int argc, char* argv[]) {
    bool testingMode = false;
    bool loadGame = false;
    std::string fname;
    
    // Check command line arguments for testing mode
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-testing") {
            testingMode = true;
            std::cout << "Starting game in testing mode." << std::endl;
        } else if (arg == "-load" && i < argc - 1) {
            loadGame = true;
            fname = argv[i + 1];
        }
    }
    
    // create and run the game with appropriate mode
    auto game = std::make_shared<Game>(testingMode);
    if (loadGame) {
        game->loadGame(fname);
    } else {
        game->initPlayers();
        game->runGame();
    }
    
    return 0;
}
