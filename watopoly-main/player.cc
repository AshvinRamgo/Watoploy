#include <iostream>
#include <algorithm>
#include <map>
#include <memory>
#include "academicbuilding.h"
#include "gym.h" 
#include "residence.h"
#include "player.h"

using namespace std;

Player::Player(string name, char token, int startingMoney)
    : name(name), token(token), money(startingMoney), position(0),
      timsCups(0), inTimsLine(false), timsTurns(0), bankrupt(false),
      consecutiveDoubles(0) {}

string Player::getName() const {
    return name;
}

string Player::getNamePieces() const {
    return string(1, token);
}

void Player::setMoney(int amount) {
    money = amount;
}

int Player::getMoney() const {
    return money;
}

int Player::getPlayerPosition() const {
    return position;
}

bool Player::isBankrupt() const {
    return bankrupt;
}

void Player::setBankrupt(bool status) {
    bankrupt = status;
}

vector<shared_ptr<OwnableSquare>> Player::getOwnedProperties() const {
    return properties;
}

bool Player::hasMonopoly(const string& monopolyName) const {
    map<string, int> monopolies = {{"Arts1", 2}, {"Arts2", 3}, {"Eng", 3}, {"Health", 3},
                                  {"Env", 3}, {"Sci1", 3}, {"Sci2", 3}, {"Math", 2}};
    int monopoly_block_counter = 0;
    
    if (monopolies.find(monopolyName) == monopolies.end()) {
        return false;
    }

    for (const auto& property : properties) {
        auto academic_building = dynamic_pointer_cast<AcademicBuilding>(property);
        
        // check if the property is an academicBuilding
        if (!academic_building) {
            continue;
        }

        // increment counter if the block is of type monopolyName
        if (academic_building->getMonopolyBlock() == monopolyName) {
            monopoly_block_counter++;
        }
    }

    return monopolies[monopolyName] == monopoly_block_counter;
}

bool Player::canTradeProperty(const string& propertyName) const {
    for (const auto& property : properties) {
        if (property->getName() == propertyName) {
            auto academic = dynamic_pointer_cast<AcademicBuilding>(property);
            if (academic && academic->getImprovementLevel() > 0) return false;

            string block = academic ? academic->getMonopolyBlock() : "";
            for (const auto& other : properties) {
                auto ab = dynamic_pointer_cast<AcademicBuilding>(other);
                if (ab && ab->getMonopolyBlock() == block && ab->getImprovementLevel() > 0) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

void Player::addProperty(shared_ptr<OwnableSquare> property) {
    properties.push_back(property);
    property->setOwner(shared_from_this());
}

void Player::removeProperty(shared_ptr<OwnableSquare> property) {
    properties.erase(remove(properties.begin(), properties.end(), property), properties.end());
}

void Player::clearProperties() {
    properties.clear();
}

int Player::getNetWorth() const {
    int total = money;
    for (const auto& prop : properties) {
        total += prop->getPurchasePrice();
        auto ab = dynamic_pointer_cast<AcademicBuilding>(prop);
        if (ab) total += ab->getImprovementLevel() * ab->getImprovementCost();
    }
    return total;
}

void Player::move(int steps) {
    position = (position + steps + 40) % 40;
}

void Player::setPosition(int newPosition) {
    position = newPosition;
}

void Player::updateMoney(int amount) {
    money += amount;
    if (money >= 0) {
        creditor = nullptr;
    }
}

void Player::payRent(int amount, shared_ptr<Player> to) {
    if (money >= amount) {
        money -= amount;
        to->updateMoney(amount);
    } else {
        // Not enough money - this should be handled in Game class
        // For now, we'll just transfer what we can
        int availableAmount = money;
        money -= amount;
        to->updateMoney(availableAmount);
        creditor = to;
        cout << "Player doesn't have enough money! Should declare bankruptcy." << endl;
    }
}

bool Player::useRollupCup() {
    if (timsCups > 0 && inTimsLine) {
        timsCups--;
        inTimsLine = false;
        return true;
    }
    return false;
}

bool Player::receiveRollupCup() {
    if (timsCups < 4) { // max 4 cups per player
        timsCups++;
        return true;
    }
    return false;
}

void Player::setTimsCups(int cups) {
    timsCups = cups;
}

void Player::setInTimsLine(bool inLine) {
    inTimsLine = inLine;
}

bool Player::isInTimsLine() const {
    return inTimsLine;
}

void Player::setTurnsInTimsLine(int turns) {
    timsTurns = turns;
}

int Player::getTurnsInTimsLine() const {
    return timsTurns;
}

void Player::mortgageProperty(const string& name) {
    for (auto& prop : properties) {
        if (prop->getName() == name && !prop->isMortgaged()) {
            prop->mortgage();
            updateMoney(prop->getPurchasePrice() / 2);
            break;
        }
    }
}

void Player::unmortgageProperty(const string& name) {
    for (auto& prop : properties) {
        if (prop->getName() == name && prop->isMortgaged()) {
            // Calculate unmortgage cost with 10% interest
            int mortgageValue = prop->getPurchasePrice() / 2;
            int interest = mortgageValue * 0.1;  // 10% interest
            int totalCost = mortgageValue + interest;
            
            // Check if player has enough money
            if (money < totalCost) {
                cout << "Not enough money to unmortgage " << name 
                     << ". Need $" << totalCost << " but only have $" << money << "." << endl;
                return;
            }
            
            // Player has enough money, unmortgage the property
            updateMoney(-totalCost);
            prop->unmortgage();
            cout << name << " unmortgaged for $" << totalCost 
                 << " ($" << mortgageValue << " + $" << interest << " interest)" << endl;
            break;
        }
    }
}

void Player::buyImprovement(const string& name) {
    for (auto& prop : properties) {
        if (prop->getName() == name) {
            auto academic_building = dynamic_pointer_cast<AcademicBuilding>(prop);
            if (academic_building && money >= academic_building->getImprovementCost()) {
                academic_building->upgrade();
                updateMoney(-academic_building->getImprovementCost());
            }
            break;
        }
    }
}

void Player::sellImprovement(const string& name) {
    for (auto& prop : properties) {
        if (prop->getName() == name) {
            auto academic_building = dynamic_pointer_cast<AcademicBuilding>(prop);
            if (academic_building) {
                int refund = academic_building->getImprovementCost() / 2;
                academic_building->downgrade();
                updateMoney(refund);
            }
            break;
        }
    }
}

// Doubles logic
void Player::incrementDoubles() {
    consecutiveDoubles++;
}

void Player::resetDoubles() {
    consecutiveDoubles = 0;
}

int Player::getConsecutiveDoubles() const {
    return consecutiveDoubles;
}

void Player::incrementTurnsInTimsLine() {
    timsTurns++;
}

int Player::getTimsCups() const {
    return timsCups;
}

int Player::getNumGyms() const {
    int count = 0;
    for (const auto& prop : properties) {
        if (std::dynamic_pointer_cast<Gym>(prop)) {
            ++count;
        }
    }
    return count;
}

int Player::getNumResidences() const {
    int count = 0;
    for (const auto& prop : properties) {
        if (std::dynamic_pointer_cast<Residence>(prop)) {
            ++count;
        }
    }
    return count;
}

shared_ptr<Player> Player::getCreditor() {
    return creditor;
}
