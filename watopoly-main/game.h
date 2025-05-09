#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "board.h"
#include "player.h"
#include "dice.h"
#include "textdisplay.h"
#include "auctionmanager.h"
#include "ownablesquare.h"

class Game : public std::enable_shared_from_this<Game> {
private:
    std::shared_ptr<Board> board;
    std::vector<std::shared_ptr<Player>> players;
    int currentPlayer;
    std::shared_ptr<Dice> dice;
    std::shared_ptr<TextDisplay> textDisplay;
    std::shared_ptr<AuctionManager> auctionManager;
    bool gameOver;
    bool inAuction;
    bool testingMode;  // Flag for testing mode

    // private helper methods
    void handleSquareAction(std::shared_ptr<Player> player, std::shared_ptr<Square> square);
    void checkForDoubles(int d1, int d2);
    void startAuction(std::shared_ptr<OwnableSquare> property);
    bool attemptToPayDebt(std::shared_ptr<Player> player, int amount, std::shared_ptr<Player> creditor);
    void checkForMonopoly(std::shared_ptr<Player> player, std::shared_ptr<OwnableSquare> property);

public:
    Game(bool testing = false);
    ~Game() = default;

    // game control
    void initPlayers();
    void runGame();
    void processCommand(std::string command); // optional
    void saveGame(std::string filename);
    void loadGame(std::string filename);
    bool checkEndCondition();
    void startTurn();
    void endTurn();

    // dice
    void rollDice();

    // player actions
    void executeTrade(std::string playerName, std::string give, std::string receive);
    void buyImprovement(std::string propertyName);
    void sellImprovement(std::string propertyName);
    void mortgageProperty(std::string propertyName);
    void unmortgageProperty(std::string propertyName);
    void declareCurrentPlayerBankrupt();
    void processBankruptcy(std::shared_ptr<Player> debtor, std::shared_ptr<Player> creditor);

    // display
    void displayCurrentPlayerAssets();
    void displayAllPlayersAssets();
    void displayGameState();

    // getters
    std::shared_ptr<Board> getBoard() const;
    std::shared_ptr<Player> getCurrentPlayer() const;
    bool isGameOver() const;

    // Testing mode methods
    void rollDice(int d1, int d2);  // Roll specific dice values
    void movePlayer(int squareIndex);  // Move to specific square
    void addMoney(int amount);  // Add money to current player
    bool isTestingMode() const { return testingMode; }
};

#endif
