#include "gym.h"
#include "player.h"
#include "dice.h"
#include <iostream>

Gym::Gym(int position, const std::string &name)
    : OwnableSquare(position, name, 150) {}

void Gym::landOn(std::shared_ptr<Player> player) {
    std::cout << player->getName() << " landed on Gym: " << name << std::endl;
    if (!owner) {
        std::cout << name << " is unowned. Purchase price: $" << price << std::endl;
        return;
    }
    if (owner == player) {
        std::cout << "You already own this gym." << std::endl;
        return;
    }
    if (mortgaged) {
        std::cout << name << " is mortgaged. No fee due." << std::endl;
        return;
    }

    Dice dice;
    auto roll = dice.roll();
    int diceSum = roll.first + roll.second;
    int gymsOwned = owner->getNumGyms();
    int multiplier = (gymsOwned == 1 ? 4 : 10);
    int fee = multiplier * diceSum;

    std::cout << "Rolled " << roll.first << " and " << roll.second
              << " (sum = " << diceSum << ")" << std::endl;

    std::cout << owner->getName() << " owns " << gymsOwned
              << (gymsOwned == 1 ? " gym. " : " gyms. ")
              << "Multiplier: " << multiplier << "x" << std::endl;

    player->payRent(fee, owner);

    std::cout << player->getName() << " paid $" << fee
              << " to " << owner->getName() << " for gym usage at " << name << std::endl;
}
