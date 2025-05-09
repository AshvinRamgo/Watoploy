#include "commandinterpreter.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include "ownablesquare.h"

using namespace std;

CommandInterpreter::CommandInterpreter(shared_ptr<Game> game) : game(game) {
    setupDefaultCommands();
}

void CommandInterpreter::setupDefaultCommands() {
    registerCommand("roll", [this](const vector<string>& args) { handleRollCommand(args); });
    registerCommand("next", [this](const vector<string>& args) { handleNextCommand(args); });
    registerCommand("trade", [this](const vector<string>& args) { handleTradeCommand(args); });
    registerCommand("improve", [this](const vector<string>& args) { handleImproveCommand(args); });
    registerCommand("mortgage", [this](const vector<string>& args) { handleMortgageCommand(args); });
    registerCommand("unmortgage", [this](const vector<string>& args) { handleUnmortgageCommand(args); });
    registerCommand("bankrupt", [this](const vector<string>& args) { handleBankruptCommand(args); });
    registerCommand("assets", [this](const vector<string>& args) { handleAssetsCommand(args); });
    registerCommand("all", [this](const vector<string>& args) { handleAllCommand(args); });
    registerCommand("save", [this](const vector<string>& args) { handleSaveCommand(args); });

    // Testing mode commands
    if (game->isTestingMode()) {
        registerCommand("move", [this](const vector<string>& args) { handleMoveCommand(args); });
        registerCommand("addmoney", [this](const vector<string>& args) { handleAddMoneyCommand(args); });
    }

    registerCommand("help", [this, isTestingMode = game->isTestingMode()](const vector<string>& args) {
        cout << "Available commands:\n"
             << "  roll - Roll dice and move\n";
        
        if (isTestingMode) {
            cout << "  roll <d1> <d2> - Roll specific dice values (testing mode)\n";
        }
        
        cout << "  next - End turn\n"
             << "  trade <name> <give> <receive> - Offer a trade\n"
             << "  improve <property> buy/sell - Buy or sell improvements\n"
             << "  mortgage <property> - Mortgage a property\n"
             << "  unmortgage <property> - Unmortgage a property\n"
             << "  bankrupt - Declare bankruptcy\n"
             << "  assets - Display your assets\n"
             << "  all - Display all players' assets\n"
             << "  save <filename> - Save the game\n";
             
        if (isTestingMode) {
            cout << "  move <square_index> - Move to a specific square (testing mode)\n"
                 << "  addmoney <amount> - Add money to current player (testing mode)\n";
        }
        
        cout << "  help - Show this help message" << endl;
    });

    registerCommand("default", [](const vector<string>& args) {
        cout << "Unknown command. Type 'help' for a list of commands." << endl;
    });
}

vector<string> CommandInterpreter::tokenizeCommand(const string& commandStr) const {
    vector<string> tokens;
    istringstream iss(commandStr);
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void CommandInterpreter::interpret(string commandStr) {
    vector<string> tokens = tokenizeCommand(commandStr);
    if (tokens.empty()) return;

    string command = tokens[0];
    vector<string> args(tokens.begin() + 1, tokens.end());

    auto it = commandHandlers.find(command);
    if (it != commandHandlers.end()) {
        it->second(args);
    } else {
        commandHandlers["default"](args);
    }
}

void CommandInterpreter::registerCommand(string name, function<void(const vector<string>&)> handler) {
    commandHandlers[name] = handler;
}

void CommandInterpreter::handleRollCommand(const vector<string>& args) {
    // check if current player has negative money
    shared_ptr<Player> current_player = game->getCurrentPlayer();
    if (current_player->getMoney() < 0) {
        cout << "Cannot Roll, get out of the negatives or bankrupt to ";
        string creditor = current_player->getCreditor() ? current_player->getCreditor()->getName() : "BANK";
        cout << creditor << endl;
        return;
    }

    // Check if specific roll values were provided (testing mode)
    if (game->isTestingMode() && args.size() == 2) {
        try {
            int d1 = stoi(args[0]);
            int d2 = stoi(args[1]);
            
            // Validate dice range
            if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
                cout << "Dice values must be between 1 and 6." << endl;
                return;
            }
            
            // Roll with specific values in testing mode
            game->rollDice(d1, d2);
        } catch (const invalid_argument&) {
            cout << "Invalid dice values. Must be numbers between 1 and 6." << endl;
        }
    } else {
        // Normal roll
        game->rollDice();
    }
}

void CommandInterpreter::handleTradeCommand(const vector<string>& args) {
    if (args.size() != 3) {
        cout << "Usage: trade <name> <give> <receive>" << endl;
        return;
    }

    string playerName = args[0], give = args[1], receive = args[2];

    bool giveIsMoney = (give.find_first_not_of("0123456789") == string::npos);
    bool receiveIsMoney = (receive.find_first_not_of("0123456789") == string::npos);

    if (giveIsMoney && receiveIsMoney) {
        cout << "Error: Cannot trade money for money." << endl;
        return;
    }

    game->executeTrade(playerName, give, receive);
}

void CommandInterpreter::handleImproveCommand(const vector<string>& args) {
    if (args.size() != 2) {
        cout << "Usage: improve <property> buy/sell" << endl;
        return;
    }

    string propertyName = args[0], action = args[1];

    if (action == "buy") {
        game->buyImprovement(propertyName);
    } else if (action == "sell") {
        game->sellImprovement(propertyName);
    } else {
        cout << "Error: Action must be 'buy' or 'sell'." << endl;
    }
}

void CommandInterpreter::handleMortgageCommand(const vector<string>& args) {
    if (args.size() != 1) {
        cout << "Usage: mortgage <property>" << endl;
        return;
    }

    game->mortgageProperty(args[0]);
}

void CommandInterpreter::handleUnmortgageCommand(const vector<string>& args) {
    if (args.size() != 1) {
        cout << "Usage: unmortgage <property>" << endl;
        return;
    }

    game->unmortgageProperty(args[0]);
}

void CommandInterpreter::handleAssetsCommand(const vector<string>& args) {
    game->displayCurrentPlayerAssets();
}

void CommandInterpreter::handleAllCommand(const vector<string>& args) {
    game->displayAllPlayersAssets();
}

void CommandInterpreter::handleSaveCommand(const vector<string>& args) {
    if (args.size() != 1) {
        cout << "Usage: save <filename>" << endl;
        return;
    }

    game->saveGame(args[0]);
}

void CommandInterpreter::handleBankruptCommand(const vector<string>& args) {
    game->declareCurrentPlayerBankrupt();
}

void CommandInterpreter::handleNextCommand(const vector<string>& args) {
    game->endTurn();
}

void CommandInterpreter::handleMoveCommand(const vector<string>& args) {
    if (!game->isTestingMode()) {
        cout << "Command only available in testing mode." << endl;
        return;
    }
    
    if (args.size() != 1) {
        cout << "Usage: move <square_index>" << endl;
        return;
    }
    
    try {
        int squareIndex = stoi(args[0]);
        game->movePlayer(squareIndex);
    } catch (const invalid_argument&) {
        cout << "Invalid square index. Must be a number between 0 and 39." << endl;
    }
}

void CommandInterpreter::handleAddMoneyCommand(const vector<string>& args) {
    if (!game->isTestingMode()) {
        cout << "Command only available in testing mode." << endl;
        return;
    }
    
    if (args.size() != 1) {
        cout << "Usage: addmoney <amount>" << endl;
        return;
    }
    
    try {
        int amount = stoi(args[0]);
        game->addMoney(amount);
    } catch (const invalid_argument&) {
        cout << "Invalid amount. Must be a number." << endl;
    }
}
