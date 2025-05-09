#ifndef ACADEMICBUILDING_H
#define ACADEMICBUILDING_H

#include "ownablesquare.h"
#include <string>
#include <array>

class AcademicBuilding : public OwnableSquare {
    std::string monopolyBlock;
    int improvementCost;
    int numImprovements;
    std::array<int, 6> tuition;

public:
    AcademicBuilding(int position, const std::string& name, int price, const std::string& block,
                     int improvementCost, int t0, int t1, int t2, int t3, int t4, int t5);

    std::string getMonopolyBlock() const;
    int getImprovementCost() const;
    int getImprovementLevel() const;

    void upgrade();   // Buy improvement
    void setImprovement(int amount);
    void downgrade(); // Sell improvement

    int getTuition() const;
    void payTuition(std::shared_ptr<Player> player);

    void landOn(std::shared_ptr<Player> player) override;

    void resetImprovements();   // Clear improvements only
};

#endif
