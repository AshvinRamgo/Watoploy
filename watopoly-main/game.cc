#include "game.h"
#include "commandinterpreter.h"
#include "squarefactory.h"
#include "square.h"
#include "ownablesquare.h"
#include "academicbuilding.h"
#include "residence.h"
#include "gym.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

// private helper methods
void Game::handleSquareAction(shared_ptr<Player> player, shared_ptr<Square> square) {
    // execute the action for the square the player landed on
    square->landOn(player);
    
    // check if it's an ownable square that can be purchased
    auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(square);
    
    if (ownableSquare && !ownableSquare->isOwned()) {
        // Check if it's an academic building and if another player owns a property in the same block
        auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(ownableSquare);
        bool canBuy = true;
        
        if (academicBuilding) {
            string monopolyBlock = academicBuilding->getMonopolyBlock();
            auto monopolySquares = board->getMonopolySquares(monopolyBlock);
            
            // Check if any properties in this block are owned by another player
            for (auto& square : monopolySquares) {
                auto otherBuilding = dynamic_pointer_cast<OwnableSquare>(square);
                if (otherBuilding && otherBuilding != ownableSquare && otherBuilding->isOwned() && 
                    otherBuilding->getOwner() != player) {
                    canBuy = false;
                    cout << "You cannot buy " << ownableSquare->getName() 
                         << " because " << otherBuilding->getName() 
                         << " is owned by " << otherBuilding->getOwner()->getName() << "." << endl;
                    break;
                }
            }
        }
        
        if (canBuy) {
            cout << "Would you like to buy " << square->getName() 
                 << " for $" << ownableSquare->getPurchasePrice() << "? (y/n) ";
            
            string response;
            cin >> response;
            
            if (tolower(response[0]) == 'y') {
                // player wants to buy the property
                if (player->getMoney() >= ownableSquare->getPurchasePrice()) {
                    // player has enough money
                    player->updateMoney(-ownableSquare->getPurchasePrice());
                    ownableSquare->setOwner(player);
                    player->addProperty(ownableSquare);
                    
                    cout << player->getName() << " purchased " << square->getName() << endl;
                    
                    // check if this completes a monopoly
                    checkForMonopoly(player, ownableSquare);
                    
                } else {
                    // not enough money
                    cout << "Not enough money to purchase " << square->getName() << endl;
                    
                    // start auction!!
                    startAuction(ownableSquare);
                }
            } else {
                // player doesn't want to buy, start auction
                cout << player->getName() << " declined to purchase " << square->getName() << endl;
                startAuction(ownableSquare);
            }
        }
    }
    
    // update the board display
    board->notifyObservers();
}

void Game::checkForDoubles(int d1, int d2) {
    auto player = getCurrentPlayer();
    
    // check if player rolled doubles
    if (d1 == d2) {
        cout << "Doubles! " << player->getName() << " gets another turn." << endl;
        
        // increment doubles counter
        player->incrementDoubles();
        
        // check for three doubles in a row
        if (player->getConsecutiveDoubles() >= 3) {
            cout << player->getName() << " rolled doubles three times in a row!" << endl;
            cout << "Go directly to DC Tims Line!" << endl;
            
            // reset doubles counter
            player->resetDoubles();
            
            // send to DC Tims Line
            player->setPosition(10); // DC Tims Line is at position 10
            player->setInTimsLine(true);

            // update the board display
            board->notifyObservers();
            
            // end turn
            endTurn();
        } else {
            // player gets another turn, don't end turn yet
            cout << "Roll again!" << endl;
        }
    } else {
        // not doubles, reset counter and end turn
        player->resetDoubles();
        endTurn();
    }
}

void Game::startAuction(shared_ptr<OwnableSquare> property) {
    cout << "Starting auction for " << property->getName() << endl;
    
    // set auction flag
    inAuction = true;
    
    // use AuctionManager to handle the auction
    auctionManager->startAuction(property, players);
    
    // get the winning player and bid
    auto [winningPlayer, winningBid] = auctionManager->getAuctionResult();
    
    if (winningPlayer) {
        // someone won the auction
        cout << winningPlayer->getName() << " won the auction for " 
             << property->getName() << " with a bid of $" << winningBid << endl;
        
        // update player's money and property
        winningPlayer->updateMoney(-winningBid);
        property->setOwner(winningPlayer);
        winningPlayer->addProperty(property);
        
        // check if this completes a monopoly
        checkForMonopoly(winningPlayer, property);
    } else {
        // no one won the auction
        cout << "No one bid on " << property->getName() << endl;
    }
    
    // clear auction flag
    inAuction = false;
    
    // update the board display
    board->notifyObservers();
}

// todo - review this logic??
bool Game::attemptToPayDebt(shared_ptr<Player> player, int amount, shared_ptr<Player> creditor) {
    // check if player has enough money
    if (player->getMoney() >= amount) {
        // player has enough money
        player->updateMoney(-amount);
        
        // pay creditor if provided
        if (creditor) {
            creditor->updateMoney(amount);
        }
        
        return true;
    }
    
    // player doesn't have enough money
    cout << player->getName() << " does not have enough money to pay $" << amount << endl;
    cout << "You need to mortgage properties, sell improvements, or declare bankruptcy." << endl;
    
    // player must try to raise money
    int neededAmount = amount - player->getMoney();
    
    cout << "You need to raise $" << neededAmount << " more." << endl;
    
    // let player try to raise money through selling improvements and mortgaging
    // this would typically happen through the command interpreter in the main game loop
    // for now, just return false to indicate payment failed
    return false;
}

void Game::checkForMonopoly(shared_ptr<Player> player, shared_ptr<OwnableSquare> property) {
    // check if this is an academic building
    auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
    
    if (academicBuilding) {
        string monopolyBlock = academicBuilding->getMonopolyBlock();
        
        // check if player now owns all properties in this monopoly
        if (board->ownsMonopoly(player, monopolyBlock)) {
            cout << "Congratulations! " << player->getName() 
                 << " now owns all properties in the " << monopolyBlock << " monopoly!" << endl;
        }
    }
}

// ctor
Game::Game(bool testing) : 
    currentPlayer(0), 
    gameOver(false), 
    inAuction(false),
    testingMode(testing) {
    
    // init game components
    board = make_shared<Board>();
    dice = make_shared<Dice>();
    auctionManager = make_shared<AuctionManager>();
    
    // init the board
    board->initBoard();
    
    // create the text display and attach it to the board (observer pattern)
    textDisplay = make_shared<TextDisplay>(board.get());
    board->attach(textDisplay.get());
}


void Game::initPlayers() {
    cout << "Welcome to Watopoly!" << endl;
    
    // get number of players and initialize them
    int numPlayers;

    cout << "Enter number of players (2-6): ";
    while (!(cin >> numPlayers)) {
        // terminate  on eof
        if (cin.eof())  {
            return;
        }

        cerr << "Error: Invalid non-integer input" << endl;

        //clear stream
        cin.clear();
        cin.ignore();

        cout << "Enter number of players (2-6): ";
    }

    // validate input
    if (numPlayers < 2 || numPlayers > 6) {
        cout << "Invalid number of players. Using default of 2." << endl;
        numPlayers = 2;
    }
    
    cin.ignore(); // clear newline
    
    // init players
    for (int i = 0; i < numPlayers; ++i) {
        string name;
        char token;
        bool validToken = false;
        
        do {
            cout << "Enter player " << (i+1) << " name: ";
            getline(cin, name);

            // terminate  on eof
            if (cin.eof())  {
                return;
            }

            if (name == "BANK") {
                cout << "Player cannot be named BANK" << endl;
            }
        } while (name == "BANK");
        
        // Keep track of tokens already in use
        while (!validToken) {
            cout << "Choose a token (G, B, D, P, S, $, L, T): ";
            cin >> token;
            token = toupper(token); // Convert to uppercase for consistency
            cin.ignore(); // clear newline
            
            // Check if token is valid
            if (token != 'G' && token != 'B' && token != 'D' && token != 'P' && 
                token != 'S' && token != '$' && token != 'L' && token != 'T') {
                cout << "Invalid token! Please choose from: G, B, D, P, S, $, L, T" << endl;
                continue;
            }
            
            // Check if token is already in use
            bool tokenInUse = false;
            for (int j = 0; j < i; ++j) {
                if (players[j]->getNamePieces()[0] == token) {
                    tokenInUse = true;
                    break;
                }
            }
            
            if (tokenInUse) {
                cout << "Token already in use! Please choose a different token." << endl;
            } else {
                validToken = true;
            }
        }
        
        // create player with starting money of $1500
        auto player = make_shared<Player>(name, token, 1500);
        players.push_back(player);
        
        // add player to the board
        board->addPlayer(player);     
    }
    board->notifyObservers();  // <- force an update after all players are added
    
}

// game control methods
// todo - see if you can break this up??
void Game::runGame() {
    
    // main game loop
    string command;
    CommandInterpreter interpreter(shared_from_this());
    
    while (!gameOver) {
        // display the current state
        displayGameState();
        
        // get command
        cout << "> ";
        getline(cin, command);

        // terminate  on eof
        if (cin.eof())  {
            return;
        }
        
        // process command via interpreter
        interpreter.interpret(command);
        
        // check if game is over
        gameOver = checkEndCondition();
    }
    
    // game over - announce winner
    cout << "Game Over! The winner is " << players[0]->getName() << "!" << endl;
}

void Game::processCommand(string command) {
    // this should now handled by the CommandInterpreter
    // todo - remove?? keep this for now??
}

void Game::saveGame(string filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file for saving." << endl;
        return;
    }
    
    // write number of players
    file << players.size() << endl;
    
    // write player information
    for (const auto& player : players) {
        file << player->getName() << " "
             << player->getNamePieces() << " "
             << player->getTimsCups() << " "
             << player->getMoney() << " "
             << player->getPlayerPosition();
        
        // add tims line status if needed
        if (player->getPlayerPosition() == 10) {
            if (player->isInTimsLine()) {
                file << " 1 " << player->getTurnsInTimsLine();
            } else {
                file << " 0";
            }
        }
        
        file << endl;
    }
    
    // write current player index
    file << currentPlayer << endl;
    
    // write board state
    board->saveBoardState(file);
    
    file.close();
    cout << "Game saved to " << filename << endl;
}

void Game::loadGame(string filename) {

    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open save file - " << filename << endl;
        return;
    }

    // clear current game state
    players.clear();
    
    // read number of players
    int numPlayers;
    file >> numPlayers;
    
    // read player information
    for (int i = 0; i < numPlayers; ++i) {
        string name;
        char token;
        int timsCups, money, position, inTimsLine;
        
        file >> name >> token >> timsCups >> money >> position;

        if (position == 10)  { //if in tims line
            file >> inTimsLine;
        } else {
            inTimsLine = 0;
        } 
        
        // create player with loaded state
        auto player = make_shared<Player>(name, token, money);
        player->setPosition(position);
        player->setTimsCups(timsCups);
        
        // set tims line status if needed
        if (inTimsLine == 1) {
            int turns;
            file >> turns;
            player->setInTimsLine(true);
            player->setTurnsInTimsLine(turns);
        }

        //gototims square
        if (position == 30) {
            player->setInTimsLine(true);
            player->setPosition(10);
            player->setTurnsInTimsLine(0);
        }
        
        players.push_back(player);
        board->addPlayer(player);
    }
    
    // read current player index
    file >> currentPlayer;

    // read board state
    board->loadBoardState(file);
    
    file.close();
    cout << "Game loaded from " << filename << endl;
    
    // update display
    board->notifyObservers();

    // run game
    runGame();
}

bool Game::checkEndCondition() {
    // count non-bankrupt players
    int activePlayers = 0;
    for (const auto& player : players) {
        if (!player->isBankrupt()) {
            activePlayers++;
        }
    }
    
    // Game ends when only one player remains
    return activePlayers <= 1;
}

void Game::startTurn() {
    // roll dice to start turn
    rollDice();
}

void Game::endTurn() {
    // move to the next player (skipping bankrupt players)
    do {
        currentPlayer = (currentPlayer + 1) % players.size();
    } while (players[currentPlayer]->isBankrupt() && !gameOver);
    
    cout << "Turn ended. Next player: " << players[currentPlayer]->getName() << endl;
}

// dice methods
void Game::rollDice() {
    auto player = getCurrentPlayer();
    
    // check if player is in DC Tims Line
    if (player->isInTimsLine()) {
        // handle DC Tims Line logic
        cout << player->getName() << " is in the DC Tims Line." << endl;
        
        // show options
        cout << "Options:" << endl;
        cout << "1. Pay $50 to leave" << endl;
        cout << "2. Use a Roll Up the Rim cup (if you have one)" << endl;
        cout << "3. Try to roll doubles" << endl;
        
        int choice;
        cout << "Enter choice (1-3): ";
        cin >> choice;
        cin.ignore(); // clear newline
        
        switch (choice) {
            case 1:
                // pay to leave
                if (player->getMoney() >= 50) {
                    player->updateMoney(-50);
                    player->setInTimsLine(false);
                    cout << player->getName() << " paid $50 to leave the DC Tims Line." << endl;
                    
                    // now roll and move
                    auto [d1, d2] = dice->roll();
                    int steps = d1 + d2;
                    
                    cout << player->getName() << " rolled " << d1 << " and " << d2 
                          << " (total: " << steps << ")" << endl;
                    
                    // move the player
                    int newPosition = (player->getPlayerPosition() + steps) % 40;
                    player->setPosition(newPosition);
                    
                    // execute square action
                    auto square = board->getSquare(newPosition);
                    handleSquareAction(player, square);
                    
                    // check for doubles
                    checkForDoubles(d1, d2);
                } else {
                    cout << "Not enough money to pay $50. Try another option." << endl;
                    return;
                }
                break;
                
            case 2:
                // use Roll Up the Rim cup
                if (player->useRollupCup()) {
                    cout << player->getName() << " used a Roll Up the Rim cup to leave the DC Tims Line." << endl;
                    player->setInTimsLine(false);
                    
                    // decrease cup count
                    board->removeCup();
                    
                    // now roll and move
                    auto [d1, d2] = dice->roll();
                    int steps = d1 + d2;
                    
                    cout << player->getName() << " rolled " << d1 << " and " << d2 
                          << " (total: " << steps << ")" << endl;
                    
                    // move the player
                    int newPosition = (player->getPlayerPosition() + steps) % 40;
                    player->setPosition(newPosition);
                    
                    // execute square action
                    auto square = board->getSquare(newPosition);
                    handleSquareAction(player, square);
                    
                    // check for doubles
                    checkForDoubles(d1, d2);
                } else {
                    cout << "No Roll Up the Rim cups available. Try another option." << endl;
                    return;
                }
                break;
                
            case 3:
                // try to roll doubles
                auto [d1, d2] = dice->roll();
                cout << player->getName() << " rolled " << d1 << " and " << d2 << endl;
                
                if (d1 == d2) {
                    cout << "Rolled doubles! " << player->getName() << " is free to leave the DC Tims Line." << endl;
                    player->setInTimsLine(false);
                    
                    // move the player
                    int newPosition = (player->getPlayerPosition() + d1 + d2) % 40;
                    player->setPosition(newPosition);
                    
                    // execute square action
                    auto square = board->getSquare(newPosition);
                    handleSquareAction(player, square);
                    
                    // reset doubles counter since this doesn't count for triple doubles
                    player->resetDoubles();
                    endTurn();
                } else {
                    cout << "Failed to roll doubles." << endl;
                    
                    // increment turns in Tims Line
                    player->incrementTurnsInTimsLine();
                    
                    // check if this is the third turn
                    if (player->getTurnsInTimsLine() >= 3) {
                        cout << "This is your third turn in the DC Tims Line. You must pay $50." << endl;
                        
                        if (player->getMoney() >= 50) {
                            player->updateMoney(-50);
                            player->setInTimsLine(false);
                            
                            // move according to the roll
                            int newPosition = (player->getPlayerPosition() + d1 + d2) % 40;
                            player->setPosition(newPosition);
                            
                            // execute square action
                            auto square = board->getSquare(newPosition);
                            handleSquareAction(player, square);
                        } else {
                            // not enough money, handle mortgage options or bankruptcy
                            cout << "Not enough money to pay $50. You need to mortgage properties or declare bankruptcy." << endl;
                            
                            // give player options
                            bool resolved = false;
                            string command;
                            
                            while (!resolved) {
                                cout << "Options: 'mortgage <property>', 'bankrupt', or 'list' to see your properties: ";
                                getline(cin, command);
                                
                                if (command.substr(0, 8) == "mortgage" && command.length() > 9) {
                                    // Extract property name
                                    string propertyName = command.substr(9);
                                    mortgageProperty(propertyName);
                                    
                                    // Check if player now has enough money
                                    if (player->getMoney() >= 50) {
                                        cout << "You now have enough money to pay the fee." << endl;
                                        player->updateMoney(-50);
                                        player->setInTimsLine(false);
                                        resolved = true;
                                    }
                                } 
                                else if (command == "bankrupt") {
                                    declareCurrentPlayerBankrupt();
                                    resolved = true;
                                }
                                else if (command == "list") {
                                    displayCurrentPlayerAssets();
                                }
                                else {
                                    cout << "Invalid command." << endl;
                                }
                            }
                            
                            // if player paid the fee, move them
                            if (!player->isInTimsLine() && !player->isBankrupt()) {
                                // move according to the roll
                                int newPosition = (player->getPlayerPosition() + d1 + d2) % 40;
                                player->setPosition(newPosition);
                                
                                // execute square action
                                auto square = board->getSquare(newPosition);
                                handleSquareAction(player, square);
                            }
                        }
                    }
                    
                    endTurn();
                }
                break;
        }
    } else {
        // normal turn - roll and move
        auto [d1, d2] = dice->roll();
        int steps = d1 + d2;
        
        cout << player->getName() << " rolled " << d1 << " and " << d2 
              << " (total: " << steps << ")" << endl;
        
        // Get current position before moving
        int oldPosition = player->getPlayerPosition();
        
        // move the player
        int newPosition = (oldPosition + steps) % 40;
        player->setPosition(newPosition);
        
        // check if passed GO
        if (newPosition < oldPosition) {
            cout << player->getName() << " passed GO and collected $200." << endl;
            player->updateMoney(200);
        }
        
        // get the square the player landed on
        auto square = board->getSquare(newPosition);
        
        // execute the square's action
        handleSquareAction(player, square);
        
        // check for doubles
        checkForDoubles(d1, d2);
    }
}

// player action methods
// todo - simplify this??
void Game::executeTrade(string playerName, string give, string receive) {
    // find the player to trade with
    auto targetIt = find_if(players.begin(), players.end(), 
                          [&playerName](const shared_ptr<Player>& p) {
                              return p->getName() == playerName;
                          });
    
    if (targetIt == players.end()) {
        cout << "Player " << playerName << " not found." << endl;
        return;
    }
    
    auto currentPlayer = getCurrentPlayer();
    auto targetPlayer = *targetIt;
    
    // determine what's being offered and requested
    bool giveIsMoney = all_of(give.begin(), give.end(), [](char c) {
        return isdigit(c);
    });
    
    bool receiveIsMoney = all_of(receive.begin(), receive.end(), [](char c) {
        return isdigit(c);
    });
    
    if (giveIsMoney && receiveIsMoney) {
        cout << "Cannot trade money for money." << endl;
        return;
    }
    
    // execute the trade based on what's being given and received
    if (giveIsMoney) {
        // current player gives money, gets property
        int amount = stoi(give);
        
        // check if current player has enough money
        if (currentPlayer->getMoney() < amount) {
            cout << "Not enough money for the trade. You have $" << currentPlayer->getMoney() 
                 << " but the trade requires $" << amount << "." << endl;
            return;
        }
        
        // find the property
        auto property = dynamic_pointer_cast<OwnableSquare>(board->findSquareByName(receive));
        
        if (!property) {
            cout << "Property " << receive << " not found." << endl;
            return;
        }
        
        // check if target player owns the property
        if (property->getOwner() != targetPlayer) {
            cout << targetPlayer->getName() << " does not own " << receive << "." << endl;
            return;
        }
        
        // Check if it's an academic building and if any property in the block has improvements
        auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
        if (academicBuilding) {
            string monopolyBlock = academicBuilding->getMonopolyBlock();
            auto monopolySquares = board->getMonopolySquares(monopolyBlock);
            
            // Check if any properties in this block have improvements
            for (auto& square : monopolySquares) {
                auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
                if (otherBuilding && otherBuilding->getImprovementLevel() > 0) {
                    cout << "Cannot trade " << receive << " because property " 
                         << otherBuilding->getName() << " in the " << monopolyBlock 
                         << " block has improvements." << endl;
                    return;
                }
            }
        } else {
            // For non-academic buildings, still use the player's check
            if (!targetPlayer->canTradeProperty(receive)) {
                cout << "Cannot trade " << receive << " because it or other properties in its monopoly have improvements." << endl;
                return;
            }
        }
        
        // propose trade to target player
        cout << currentPlayer->getName() << " offers $" << amount << " for " << receive << "." << endl;
        cout << "Does " << targetPlayer->getName() << " accept? (y/n): ";
        
        string response;
        cin >> response;
        
        if (tolower(response[0]) == 'y') {
            // execute trade
            currentPlayer->updateMoney(-amount);
            targetPlayer->updateMoney(amount);
            
            targetPlayer->removeProperty(property);
            property->setOwner(currentPlayer);
            currentPlayer->addProperty(property);
            
            cout << "Trade completed!" << endl;
            
            // check if this creates a monopoly
            checkForMonopoly(currentPlayer, property);
        } else {
            cout << "Trade declined." << endl;
        }
    } else if (receiveIsMoney) {
        // current player gives property, gets money
        int amount = stoi(receive);
        
        // find the property
        auto property = dynamic_pointer_cast<OwnableSquare>(board->findSquareByName(give));
        
        if (!property) {
            cout << "Property " << give << " not found." << endl;
            return;
        }
        
        // check if current player owns the property
        if (property->getOwner() != currentPlayer) {
            cout << "You do not own " << give << "." << endl;
            return;
        }
        
        // Check if it's an academic building and if any property in the block has improvements
        auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
        if (academicBuilding) {
            string monopolyBlock = academicBuilding->getMonopolyBlock();
            auto monopolySquares = board->getMonopolySquares(monopolyBlock);
            
            // Check if any properties in this block have improvements
            for (auto& square : monopolySquares) {
                auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
                if (otherBuilding && otherBuilding->getImprovementLevel() > 0) {
                    cout << "Cannot trade " << give << " because property " 
                         << otherBuilding->getName() << " in the " << monopolyBlock 
                         << " block has improvements." << endl;
                    return;
                }
            }
        } else {
            // For non-academic buildings, still use the player's check
            if (!currentPlayer->canTradeProperty(give)) {
                cout << "Cannot trade " << give << " because it or other properties in its monopoly have improvements." << endl;
                return;
            }
        }
        
        // check if target player has enough money
        if (targetPlayer->getMoney() < amount) {
            cout << targetPlayer->getName() << " does not have enough money for the trade. They have $" 
                 << targetPlayer->getMoney() << " but the trade requires $" << amount << "." << endl;
            return;
        }
        
        // propose trade to target player
        cout << currentPlayer->getName() << " offers " << give << " for $" << amount << "." << endl;
        cout << "Does " << targetPlayer->getName() << " accept? (y/n): ";
        
        string response;
        cin >> response;
        
        if (tolower(response[0]) == 'y') {
            // execute trade
            currentPlayer->removeProperty(property);
            property->setOwner(targetPlayer);
            targetPlayer->addProperty(property);
            
            targetPlayer->updateMoney(-amount);
            currentPlayer->updateMoney(amount);
            
            cout << "Trade completed!" << endl;
            
            // check if this creates a monopoly
            checkForMonopoly(targetPlayer, property);
        } else {
            cout << "Trade declined." << endl;
        }
    } else {
        // current player gives property, gets property
        // find the properties
        auto propertyGive = dynamic_pointer_cast<OwnableSquare>(board->findSquareByName(give));
        auto propertyReceive = dynamic_pointer_cast<OwnableSquare>(board->findSquareByName(receive));
        
        if (!propertyGive) {
            cout << "Property " << give << " not found." << endl;
            return;
        }
        
        if (!propertyReceive) {
            cout << "Property " << receive << " not found." << endl;
            return;
        }
        
        // check ownership
        if (propertyGive->getOwner() != currentPlayer) {
            cout << "You do not own " << give << "." << endl;
            return;
        }
        
        if (propertyReceive->getOwner() != targetPlayer) {
            cout << targetPlayer->getName() << " does not own " << receive << "." << endl;
            return;
        }
        
        // Check if propertyGive is an academic building and if any property in the block has improvements
        auto academicBuildingGive = dynamic_pointer_cast<AcademicBuilding>(propertyGive);
        if (academicBuildingGive) {
            string monopolyBlock = academicBuildingGive->getMonopolyBlock();
            auto monopolySquares = board->getMonopolySquares(monopolyBlock);
            
            // Check if any properties in this block have improvements
            for (auto& square : monopolySquares) {
                auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
                if (otherBuilding && otherBuilding->getImprovementLevel() > 0) {
                    cout << "Cannot trade " << give << " because property " 
                         << otherBuilding->getName() << " in the " << monopolyBlock 
                         << " block has improvements." << endl;
                    return;
                }
            }
        } else {
            // For non-academic buildings, still use the player's check
            if (!currentPlayer->canTradeProperty(give)) {
                cout << "Cannot trade " << give << " because it or other properties in its monopoly have improvements." << endl;
                return;
            }
        }
        
        // Check if propertyReceive is an academic building and if any property in the block has improvements
        auto academicBuildingReceive = dynamic_pointer_cast<AcademicBuilding>(propertyReceive);
        if (academicBuildingReceive) {
            string monopolyBlock = academicBuildingReceive->getMonopolyBlock();
            auto monopolySquares = board->getMonopolySquares(monopolyBlock);
            
            // Check if any properties in this block have improvements
            for (auto& square : monopolySquares) {
                auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
                if (otherBuilding && otherBuilding->getImprovementLevel() > 0) {
                    cout << "Cannot trade " << receive << " because property " 
                         << otherBuilding->getName() << " in the " << monopolyBlock 
                         << " block has improvements." << endl;
                    return;
                }
            }
        } else {
            // For non-academic buildings, still use the player's check
            if (!targetPlayer->canTradeProperty(receive)) {
                cout << "Cannot trade " << receive << " because it or other properties in its monopoly have improvements." << endl;
                return;
            }
        }
        
        // propose trade to target player
        cout << currentPlayer->getName() << " offers " << give << " for " << receive << "." << endl;
        cout << "Does " << targetPlayer->getName() << " accept? (y/n): ";
        
        string response;
        cin >> response;
        
        if (tolower(response[0]) == 'y') {
            // execute trade
            currentPlayer->removeProperty(propertyGive);
            propertyGive->setOwner(targetPlayer);
            targetPlayer->addProperty(propertyGive);
            
            targetPlayer->removeProperty(propertyReceive);
            propertyReceive->setOwner(currentPlayer);
            currentPlayer->addProperty(propertyReceive);
            
            cout << "Trade completed!" << endl;
            
            // check if this creates monopolies
            checkForMonopoly(currentPlayer, propertyReceive);
            checkForMonopoly(targetPlayer, propertyGive);
        } else {
            cout << "Trade declined." << endl;
        }
    }
    
    // update the board display
    board->notifyObservers();
}

void Game::buyImprovement(string propertyName) {
    auto player = getCurrentPlayer();
    
    // find the property
    auto square = board->findSquareByName(propertyName);
    auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
    
    if (!academicBuilding) {
        cout << propertyName << " is not an academic building." << endl;
        return;
    }
    
    // check if player owns the property
    if (academicBuilding->getOwner() != player) {
        cout << "You do not own " << propertyName << "." << endl;
        return;
    }
    
    // check if player owns the monopoly
    string monopolyBlock = academicBuilding->getMonopolyBlock();
    if (!board->ownsMonopoly(player, monopolyBlock)) {
        cout << "You do not own all properties in the " << monopolyBlock << " monopoly." << endl;
        return;
    }
    
    // check if already at maximum improvements
    if (academicBuilding->getImprovementLevel() >= 5) {
        cout << propertyName << " already has maximum improvements." << endl;
        return;
    }
    
    // check if player has enough money
    int improvementCost = academicBuilding->getImprovementCost();
    if (player->getMoney() < improvementCost) {
        cout << "Not enough money. Improvement cost: $" << improvementCost 
             << " but you only have $" << player->getMoney() << "." << endl;
        return;
    }
    
    // check for even improvement rule
    auto monopolySquares = board->getMonopolySquares(monopolyBlock);
    int currentLevel = academicBuilding->getImprovementLevel();
    
    // ensure all properties in monopoly have at least the current level of improvements
    for (auto& square : monopolySquares) {
        auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
        
        if (otherBuilding && otherBuilding != academicBuilding && 
            otherBuilding->getImprovementLevel() < currentLevel) {
            cout << "Cannot improve " << propertyName << " until " 
                 << otherBuilding->getName() << " has at least " 
                 << currentLevel << " improvements." << endl;
            return;
        }
    }
    
    // buy the improvement
    academicBuilding->upgrade();
    player->updateMoney(-improvementCost);
    
    cout << "Improvement purchased for " << propertyName 
         << ". New level: " << academicBuilding->getImprovementLevel() << endl;
    
    // update the board display
    board->notifyObservers();
}

void Game::sellImprovement(string propertyName) {
    auto player = getCurrentPlayer();
    
    // find the property
    auto square = board->findSquareByName(propertyName);
    auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
    
    if (!academicBuilding) {
        cout << propertyName << " is not an academic building." << endl;
        return;
    }
    
    // check if player owns the property
    if (academicBuilding->getOwner() != player) {
        cout << "You do not own " << propertyName << "." << endl;
        return;
    }
    
    // check if there are improvements to sell
    if (academicBuilding->getImprovementLevel() <= 0) {
        cout << propertyName << " has no improvements to sell." << endl;
        return;
    }
    
    // check for even improvement rule
    string monopolyBlock = academicBuilding->getMonopolyBlock();
    auto monopolySquares = board->getMonopolySquares(monopolyBlock);
    int currentLevel = academicBuilding->getImprovementLevel();
    
    // ensure no property in monopoly has more improvements than current - 1
    for (auto& square : monopolySquares) {
        auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
        
        if (otherBuilding && otherBuilding != academicBuilding && 
            otherBuilding->getImprovementLevel() > currentLevel) {
            cout << "Cannot sell improvement from " << propertyName << " until " 
                 << otherBuilding->getName() << " improvements are sold. " 
                 << otherBuilding->getName() << " has " << otherBuilding->getImprovementLevel() 
                 << " improvements, while " << propertyName << " has " << currentLevel << "." << endl;
            return;
        }
    }
    
    // sell the improvement
    academicBuilding->downgrade();
    
    // player receives half the improvement cost
    int refund = academicBuilding->getImprovementCost() / 2;
    player->updateMoney(refund);
    
    cout << "Improvement sold from " << propertyName 
         << ". New level: " << academicBuilding->getImprovementLevel() 
         << ". Received $" << refund << endl;
    
    // update the board display
    board->notifyObservers();
}

void Game::mortgageProperty(string propertyName) {
    auto player = getCurrentPlayer();
    
    // find the property
    auto square = board->findSquareByName(propertyName);
    auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(square);
    
    if (!ownableSquare) {
        cout << propertyName << " is not an ownable property." << endl;
        return;
    }
    
    // check if player owns the property
    if (ownableSquare->getOwner() != player) {
        cout << "You do not own " << propertyName << "." << endl;
        return;
    }
    
    // check if already mortgaged
    if (ownableSquare->isMortgaged()) {
        cout << propertyName << " is already mortgaged." << endl;
        return;
    }
    
    // check for improvements on academic buildings
    auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(ownableSquare);
    if (academicBuilding && academicBuilding->getImprovementLevel() > 0) {
        cout << "Cannot mortgage " << propertyName << " until all improvements are sold." << endl;
        return;
    }
    
    // check for improvements on other properties in the same monopoly
    string monopolyBlock = "";
    if (academicBuilding) {
        monopolyBlock = academicBuilding->getMonopolyBlock();
        auto monopolySquares = board->getMonopolySquares(monopolyBlock);
        
        for (auto& square : monopolySquares) {
            auto otherBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
            
            if (otherBuilding && otherBuilding != academicBuilding && 
                otherBuilding->getImprovementLevel() > 0) {
                cout << "Cannot mortgage " << propertyName << " until all improvements on " 
                     << otherBuilding->getName() << " are sold." << endl;
                return;
            }
        }
    }
    
    // mortgage the property
    ownableSquare->mortgage();
    
    // player receives half the purchase price
    int mortgageValue = ownableSquare->getPurchasePrice() / 2;
    player->updateMoney(mortgageValue);
    
    cout << propertyName << " mortgaged. Received $" << mortgageValue << endl;
    
    // update the board display
    board->notifyObservers();
}

void Game::unmortgageProperty(string propertyName) {
    auto player = getCurrentPlayer();
    
    // find the property
    auto square = board->findSquareByName(propertyName);
    auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(square);
    
    if (!ownableSquare) {
        cout << propertyName << " is not an ownable property." << endl;
        return;
    }
    
    // check if player owns the property
    if (ownableSquare->getOwner() != player) {
        cout << "You do not own " << propertyName << "." << endl;
        return;
    }
    
    // check if mortgaged
    if (!ownableSquare->isMortgaged()) {
        cout << propertyName << " is not mortgaged." << endl;
        return;
    }
    
    // calculate unmortgage cost (mortgage value + 10% interest)
    int mortgageValue = ownableSquare->getPurchasePrice() / 2;
    int unmortgageCost = mortgageValue + (mortgageValue / 10);
    
    // check if player has enough money
    if (player->getMoney() < unmortgageCost) {
        cout << "Not enough money. Unmortgage cost: $" << unmortgageCost << endl;
        return;
    }
    
    // unmortgage the property
    ownableSquare->unmortgage();
    player->updateMoney(-unmortgageCost);
    
    cout << propertyName << " unmortgaged for $" << unmortgageCost << endl;
    
    // update the board display
    board->notifyObservers();
}

// todo - fix this logic??
void Game::declareCurrentPlayerBankrupt() {
    auto player = getCurrentPlayer();
    
    // get the creditor (whoever player owes money to)
    // if currentCreditor is null, bankruptcy is to the bank
    std::shared_ptr<Player> creditor = player->getCreditor();
    
    // process bankruptcy
    processBankruptcy(player, creditor);
    
}

void Game::processBankruptcy(shared_ptr<Player> debtor, shared_ptr<Player> creditor) {
    cout << debtor->getName() << " has declared bankruptcy";
    
    if (creditor) {
        cout << " to " << creditor->getName();
    } else {
        cout << " to the bank";
    }
    cout << "." << endl;
    
    // mark player as bankrupt
    debtor->setBankrupt(true);
    
    if (creditor) {
        // transfer all assets to creditor
        for (auto& property : debtor->getOwnedProperties()) {
            property->setOwner(creditor);
            creditor->addProperty(property);
            
            // unmortgage properties that were mortgaged
            if (property->isMortgaged()) {
                // creditor pays 10% interest immediately
                int mortgageValue = property->getPurchasePrice() / 2;
                int interest = mortgageValue / 10;
                
                creditor->updateMoney(-interest);
                
                cout << creditor->getName() << " paid $" << interest 
                     << " interest on mortgaged property " << property->getName() << endl;
            }
        }
        
        // transfer money
        creditor->updateMoney(debtor->getMoney());
        
        // transfer Roll Up the Rim cups
        for (int i = 0; i < debtor->getTimsCups(); i++) {
            creditor->receiveRollupCup();
        }
        
        cout << "All assets transferred to " << creditor->getName() << endl;
    } else {
        // return properties to the bank
        for (auto& property : debtor->getOwnedProperties()) {
            property->resetOwnership();
            
            // unmortgage the property
            if (property->isMortgaged()) {
                property->unmortgage();
            }
            
            // reset improvements for academic buildings
            auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
            if (academicBuilding) {
                academicBuilding->resetImprovements();
            }
            
            cout << "Property " << property->getName() << " returned to the bank." << endl;
            
            // auction the property
            startAuction(property);
        }
        
        // return Roll Up the Rim cups
        for (int i = 0; i < debtor->getTimsCups(); i++) {
            board->removeCup();
        }
    }
    
    // clear player's properties and money
    debtor->clearProperties();
    debtor->setMoney(0);
    debtor->setTimsCups(0);
    
    // check if game is over
    gameOver = checkEndCondition();
    
    if (!gameOver) {
        // move to next player if current player went bankrupt
        if (debtor == getCurrentPlayer()) {
            endTurn();
        }
    }
    
    // update the board display
    board->notifyObservers();
}

// display methods
void Game::displayCurrentPlayerAssets() {
    auto player = getCurrentPlayer();
    
    cout << "\n=== " << player->getName() << "'s Assets ===" << endl;
    cout << "Money: $" << player->getMoney() << endl;
    cout << "Properties:" << endl;
    
    auto properties = player->getOwnedProperties();
    
    if (properties.empty()) {
        cout << "  None" << endl;
    } else {
        // group properties by monopoly
        unordered_map<string, vector<shared_ptr<OwnableSquare>>> propertyGroups;
        
        for (auto& property : properties) {
            string group = "Other";
            
            auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
            if (academicBuilding) {
                group = academicBuilding->getMonopolyBlock();
            } else if (dynamic_pointer_cast<Residence>(property)) {
                group = "Residences";
            } else if (dynamic_pointer_cast<Gym>(property)) {
                group = "Gyms";
            }
            
            propertyGroups[group].push_back(property);
        }
        
        // display properties by group
        for (const auto& [group, props] : propertyGroups) {
            cout << "  " << group << ":" << endl;
            
            for (const auto& property : props) {
                cout << "    " << property->getName();
                
                if (property->isMortgaged()) {
                    cout << " (mortgaged)";
                } else {
                    auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
                    if (academicBuilding && academicBuilding->getImprovementLevel() > 0) {
                        cout << " (" << academicBuilding->getImprovementLevel() << " improvements)";
                    }
                }
                
                cout << endl;
            }
        }
    }
    
    cout << "Roll Up the Rim cups: " << player->getTimsCups() << endl;
}

void Game::displayAllPlayersAssets() {
    cout << "\n========== All Players' Assets ==========\n" << endl;
    
    for (const auto& player : players) {
        if (!player->isBankrupt()) {
            cout << player->getName() << ":" << endl;
            cout << "  Money: $" << player->getMoney() << endl;
            cout << "  Position: " << player->getPlayerPosition() 
                 << " (" << board->getSquare(player->getPlayerPosition())->getName() << ")" << endl;
            
            auto properties = player->getOwnedProperties();
            cout << "  Properties: ";
            
            if (properties.empty()) {
                cout << "None" << endl;
            } else {
                cout << endl;
                
                for (const auto& property : properties) {
                    cout << "    " << property->getName();
                    
                    if (property->isMortgaged()) {
                        cout << " (mortgaged)";
                    } else {
                        auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(property);
                        if (academicBuilding && academicBuilding->getImprovementLevel() > 0) {
                            cout << " (" << academicBuilding->getImprovementLevel() << " improvements)";
                        }
                    }
                    
                    cout << endl;
                }
            }
            
            cout << "  Roll Up the Rim cups: " << player->getTimsCups() << endl;
            cout << endl;
        }
    }
}

void Game::displayGameState() {
    // display the board
    cout << *textDisplay << endl;
    
    // display current player info
    auto currentPlayerPtr = players[currentPlayer];
    cout << "Current player: " << currentPlayerPtr->getName() 
          << " (" << currentPlayerPtr->getNamePieces() << ")" << endl;
    cout << "Money: $" << currentPlayerPtr->getMoney() << endl;
    cout << "Position: " << board->getSquare(currentPlayerPtr->getPlayerPosition())->getName() << endl;
    
    // display DC Tims Line status if applicable
    if (currentPlayerPtr->isInTimsLine()) {
        cout << "In DC Tims Line (turn " << currentPlayerPtr->getTurnsInTimsLine() + 1 << " of 3)" << endl;
    }
    
    // display Roll Up the Rim cups
    cout << "Roll Up the Rim cups: " << currentPlayerPtr->getTimsCups() << endl;
}

// getters:
shared_ptr<Board> Game::getBoard() const {
    return board;
}

shared_ptr<Player> Game::getCurrentPlayer() const {
    return players[currentPlayer];
}

bool Game::isGameOver() const {
    return gameOver;
}

// Testing mode methods
void Game::rollDice(int d1, int d2) {
    if (!testingMode) {
        cout << "Error: This command is only available in testing mode." << endl;
        return;
    }

    auto player = getCurrentPlayer();
    
    cout << player->getName() << " rolled " << d1 << " and " << d2 
          << " (total: " << (d1 + d2) << ")" << endl;
    
    // Get current position before moving
    int oldPosition = player->getPlayerPosition();
    
    // move the player
    int newPosition = (oldPosition + d1 + d2) % 40;
    player->setPosition(newPosition);
    
    // check if passed GO
    if (newPosition < oldPosition) {
        cout << player->getName() << " passed GO and collected $200." << endl;
        player->updateMoney(200);
    }
    
    // get the square the player landed on
    auto square = board->getSquare(newPosition);
    
    // execute the square's action
    handleSquareAction(player, square);
    
    // check for doubles
    checkForDoubles(d1, d2);
}

void Game::movePlayer(int squareIndex) {
    if (!testingMode) {
        cout << "Error: This command is only available in testing mode." << endl;
        return;
    }
    
    if (squareIndex < 0 || squareIndex > 39) {
        cout << "Error: Square index must be between 0 and 39." << endl;
        return;
    }
    
    auto player = getCurrentPlayer();
    int oldPosition = player->getPlayerPosition();
    
    // Special case: Check if player is passing GO
    if (squareIndex < oldPosition) {
        cout << player->getName() << " passed GO and collected $200." << endl;
        player->updateMoney(200);
    }
    
    player->setPosition(squareIndex);
    auto square = board->getSquare(squareIndex);
    
    // Execute square action
    handleSquareAction(player, square);
    
    // End the player's turn after moving in testing mode
    endTurn();
}

void Game::addMoney(int amount) {
    if (!testingMode) {
        cout << "Error: This command is only available in testing mode." << endl;
        return;
    }
    
    auto player = getCurrentPlayer();
    player->updateMoney(amount);
    
    cout << "$" << amount << " added to " << player->getName() 
          << ". New balance: $" << player->getMoney() << endl;
}
