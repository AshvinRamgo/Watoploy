#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "game.h"

class CommandInterpreter {
private:
    std::shared_ptr<Game> game;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandHandlers;

    void setupDefaultCommands();
    std::vector<std::string> tokenizeCommand(const std::string& commandStr) const;

public:
    CommandInterpreter(std::shared_ptr<Game> game);

    void interpret(std::string command);
    void registerCommand(std::string name, std::function<void(const std::vector<std::string>&)> handler);

    void handleRollCommand(const std::vector<std::string>& args);
    void handleTradeCommand(const std::vector<std::string>& args);
    void handleImproveCommand(const std::vector<std::string>& args);
    void handleMortgageCommand(const std::vector<std::string>& args);
    void handleUnmortgageCommand(const std::vector<std::string>& args);
    void handleAssetsCommand(const std::vector<std::string>& args);
    void handleAllCommand(const std::vector<std::string>& args);
    void handleSaveCommand(const std::vector<std::string>& args);
    void handleBankruptCommand(const std::vector<std::string>& args);
    void handleNextCommand(const std::vector<std::string>& args);
    
    // Testing mode commands
    void handleMoveCommand(const std::vector<std::string>& args);
    void handleAddMoneyCommand(const std::vector<std::string>& args);
};

#endif
