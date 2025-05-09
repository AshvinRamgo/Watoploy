#include "residence.h"
#include "player.h"
#include <iostream>

Residence::Residence(int position, const std::string& name)
    : OwnableSquare(position, name, 200) {}

void Residence::landOn(std::shared_ptr<Player> player) {
    std::cout << player->getName() << " landed on Residence: " << name << std::endl;

    if (!owner) {
        std::cout << name << " is unowned. Purchase price: $" << price << std::endl;
        return;
    }

    if (owner == player) {
        std::cout << "You already own this residence." << std::endl;
        return;
    }

    if (mortgaged) {
        std::cout << name << " is mortgaged. No rent due." << std::endl;
        return;
    }

    int numOwned = owner->getNumResidences();
    int rent = 0;
    switch (numOwned) {
        case 1: rent = 25; break;
        case 2: rent = 50; break;
        case 3: rent = 100; break;
        case 4: rent = 200; break;
    }

    std::cout << owner->getName() << " owns " << numOwned
              << (numOwned == 1 ? " residence. " : " residences. ")
              << "Rent: $" << rent << std::endl;

    player->payRent(rent, owner);
    std::cout << player->getName() << " paid $" << rent
              << " to " << owner->getName() << " for " << name << std::endl;
}
