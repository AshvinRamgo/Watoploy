#include "academicbuilding.h"
#include "player.h"
#include <iostream>

AcademicBuilding::AcademicBuilding(int position, const std::string& name, int price, const std::string& block,
                                   int improvementCost, int t0, int t1, int t2, int t3, int t4, int t5)
    : OwnableSquare(position, name, price),
      monopolyBlock(block),
      improvementCost(improvementCost),
      numImprovements(0),
      tuition{t0, t1, t2, t3, t4, t5} {}

std::string AcademicBuilding::getMonopolyBlock() const {
    return monopolyBlock;
}

int AcademicBuilding::getImprovementCost() const {
    return improvementCost;
}

int AcademicBuilding::getImprovementLevel() const {
    return numImprovements;
}

void AcademicBuilding::upgrade() {

    if (owner && owner->hasMonopoly(monopolyBlock) && numImprovements < 5) {
        ++numImprovements;
        std::cout << owner->getName() << " upgraded " << name
                  << " (total improvements: " << numImprovements << ").\n";
    } else {
        std::cout << "Cannot upgrade " << name
                  << " — not a monopoly or already maxed.\n";
    }
}

void  AcademicBuilding::setImprovement(int amount) {
    // error check
    if (amount > 5) {
        std::cerr << "Invalid number of improvements" << std::endl;
        return;
    }

    numImprovements = amount;
}

void AcademicBuilding::downgrade() {
    if (numImprovements > 0) {
        --numImprovements;
        std::cout << owner->getName() << " downgraded " << name
                  << " (total improvements: " << numImprovements << ").\n";
    } else {
        std::cout << name << " has no improvements to downgrade.\n";
    }
}

int AcademicBuilding::getTuition() const {
    if (!owner) return 0;

    int amount = tuition[numImprovements];
    if (numImprovements == 0 && owner->hasMonopoly(monopolyBlock)) {
        amount *= 2;
    }
    return amount;
}

void AcademicBuilding::payTuition(std::shared_ptr<Player> player) {
    int amount = getTuition();

    std::cout << name << " has " << numImprovements
              << (numImprovements == 1 ? " improvement. " : " improvements. ");

    if (numImprovements == 0 && owner->hasMonopoly(monopolyBlock)) {
        std::cout << "(Monopoly bonus: tuition doubled!) ";
    }
    std::cout << std::endl;

    player->payRent(amount, owner);

    std::cout << player->getName() << " paid $" << amount
              << " in tuition to " << owner->getName()
              << " for " << name << "." << std::endl;
}

void AcademicBuilding::landOn(std::shared_ptr<Player> player) {
    std::cout << player->getName() << " landed on Academic Building: " << name << "." << std::endl;

    if (!owner) {
        std::cout << name << " is unowned. Purchase price: $" << price << std::endl;
        return;
    }

    if (owner == player) {
        std::cout << "You already own this building." << std::endl;
        return;
    }

    if (mortgaged) {
        std::cout << name << " is mortgaged. No tuition due." << std::endl;
        return;
    }

    payTuition(player);
}

void AcademicBuilding::resetImprovements() {
    numImprovements = 0;
    std::cout << name << "'s improvements have been reset." << std::endl;
}

