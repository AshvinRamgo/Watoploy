#include "cell.h"
#include <algorithm>

using namespace std;

Cell::Cell() : row(0), col(0) {}

Cell::Cell(int row, int col)
    : row(row), col(col), displayChar(' '), hasOwner(false), ownerChar(' '),
      improvements(0), mortgaged(false) {}

char Cell::getDisplayChar() const {
    return displayChar;
}

void Cell::setDisplayChar(char c) {
    displayChar = c;
}

void Cell::addPlayer(shared_ptr<Player> player) {
    playersOnCell.push_back(player);
}

void Cell::removePlayer(shared_ptr<Player> player) {
    auto it = find_if(playersOnCell.begin(), playersOnCell.end(), 
                      [&player](const shared_ptr<Player>& p) {
                          return p->getName() == player->getName();
                      });
    if (it != playersOnCell.end()) {
        playersOnCell.erase(it);
    }
}

const vector<shared_ptr<Player>>& Cell::getPlayers() const {
    return playersOnCell;
}

void Cell::setPropertyName(const string& name) {
    propertyName = name;
}

void Cell::setOwner(char owner) {
    ownerChar = owner;
    hasOwner = true;
}

void Cell::clearOwner() {
    ownerChar = ' ';
    hasOwner = false;
}

void Cell::setImprovements(int level) {
    improvements = level;
}

void Cell::setMortgaged(bool isMortgaged) {
    mortgaged = isMortgaged;
}

int Cell::getRow() const {
    return row;
}

int Cell::getCol() const {
    return col;
}

string Cell::getPropertyName() const {
    return propertyName;
}

bool Cell::isOwned() const {
    return hasOwner;
}

char Cell::getOwnerChar() const {
    return ownerChar;
}

int Cell::getImprovements() const {
    return improvements;
}

bool Cell::isMortgaged() const {
    return mortgaged;
}
