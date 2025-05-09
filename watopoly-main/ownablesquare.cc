#include "ownablesquare.h"
#include "player.h"
#include <iostream>

OwnableSquare::OwnableSquare(int position, const std::string& name, int price)
    : Square(position, name), price(price), mortgaged(false) {}

// Allows a player to purchase if not already owned and has enough money
bool OwnableSquare::purchase(std::shared_ptr<Player> player) {
    if (owner || player->getMoney() < price) return false;
    player->updateMoney(-price);
    owner = player;
    player->addProperty(std::static_pointer_cast<OwnableSquare>(shared_from_this()));
    return true;
}

// Set the property as mortgaged and give half value to player
void OwnableSquare::mortgage() {
    if (!mortgaged && owner) {
        owner->updateMoney(price / 2);
        mortgaged = true;
    }
}

// Unmortgage the property by paying 60% of its value
void OwnableSquare::unmortgage() {
    if (mortgaged && owner) {
        int cost = price * 0.6;
        if (owner->getMoney() >= cost) {
            owner->updateMoney(-cost);
            mortgaged = false;
        } else {
            std::cout << owner->getName() << " doesn't have enough to unmortgage " << name << "\n";
        }
    }
}

// Return current owner
std::shared_ptr<Player> OwnableSquare::getOwner() const {
    return owner;
}

// Return if property has an owner
bool OwnableSquare::isOwned() const {
    return owner != nullptr;
}

// Return mortgage state
bool OwnableSquare::isMortgaged() const {
    return mortgaged;
}

// Set property owner manually (used for trades/bankruptcy)
void OwnableSquare::setOwner(std::shared_ptr<Player> player) {
    owner = player;
}

// Clear owner and mortgage status
void OwnableSquare::resetOwnership() {
    if (owner) {
        owner->removeProperty(std::static_pointer_cast<OwnableSquare>(shared_from_this()));
    }
    owner.reset();
    mortgaged = false;
}

// Return the purchase cost of the property
int OwnableSquare::getPurchasePrice() const {
    return price;
}

char OwnableSquare::getOwnerInitial() const {
    return owner ? owner->getNamePieces()[0] : ' ';
}

void OwnableSquare::setMortgage() {
    mortgaged = true;
}
