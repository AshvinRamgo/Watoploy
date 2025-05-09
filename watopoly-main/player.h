#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <memory>
#include "ownablesquare.h"

class OwnableSquare;
class Player;

class Player : public std::enable_shared_from_this<Player> {
private:
    std::string name;
    char token;
    int money;
    int position;
    int timsCups;
    bool inTimsLine;
    int timsTurns;
    bool bankrupt;
    int consecutiveDoubles;
    std::shared_ptr<Player> creditor;

    std::vector<std::shared_ptr<OwnableSquare>> properties;

public:
    // Constructor
    Player(std::string name, char token, int startingMoney);

    // Basic info
    std::string getName() const;
    std::string getNamePieces() const;
    int getMoney() const;
    int getPlayerPosition() const;
    bool isBankrupt() const;
    void setMoney(int amount);
    std::shared_ptr<Player> getCreditor();


    // Properties
    std::vector<std::shared_ptr<OwnableSquare>> getOwnedProperties() const;
    bool hasMonopoly(const std::string& monopolyName) const;
    bool canTradeProperty(const std::string& propertyName) const;
    void addProperty(std::shared_ptr<OwnableSquare> property);
    void removeProperty(std::shared_ptr<OwnableSquare> property);
    int getNetWorth() const;

    // Movement & state
    void move(int steps);
    void setPosition(int newPosition);
    void updateMoney(int amount);
    void setBankrupt(bool status);
    void setTimsCups(int cups);
    void setInTimsLine(bool inLine);
    void setTurnsInTimsLine(int turns);
    int getTimsCups() const;
    bool isInTimsLine() const;
    int getTurnsInTimsLine() const;

    // Cups & rent
    bool useRollupCup();
    bool receiveRollupCup();
    void payRent(int amount, std::shared_ptr<Player> to);

    // Bankruptcy
    void clearProperties();

    // Improvements & Mortgage
    void mortgageProperty(const std::string& name);
    void unmortgageProperty(const std::string& name);
    void buyImprovement(const std::string& name);
    void sellImprovement(const std::string& name);

    // Doubles (for DC Tims Line logic)
    void incrementDoubles();
    void resetDoubles();
    int getConsecutiveDoubles() const;
    void incrementTurnsInTimsLine();

    // Ownables 
    int getNumGyms() const;
    int getNumResidences() const;

};

#endif // PLAYER_H

