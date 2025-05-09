#ifndef CELL_H
#define CELL_H

#include <vector>
#include <string>
#include <memory>
#include "player.h"

class Cell {
private:
    int row;
    int col;
    char displayChar;
    std::vector<std::shared_ptr<Player>> playersOnCell;
    std::string propertyName;
    bool hasOwner;
    char ownerChar;
    int improvements;
    bool mortgaged;

public:
    Cell();
    Cell(int row, int col);

    char getDisplayChar() const;
    void setDisplayChar(char c);

    void addPlayer(std::shared_ptr<Player> player);
    void removePlayer(std::shared_ptr<Player> player);
    const std::vector<std::shared_ptr<Player>>& getPlayers() const;

    void setPropertyName(const std::string& name);
    void setOwner(char ownerChar);
    void clearOwner();
    void setImprovements(int level);
    void setMortgaged(bool isMortgaged);

    int getRow() const;
    int getCol() const;
    std::string getPropertyName() const;
    bool isOwned() const;
    char getOwnerChar() const;
    int getImprovements() const;
    bool isMortgaged() const;
};

#endif
