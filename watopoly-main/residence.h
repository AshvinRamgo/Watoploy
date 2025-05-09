#ifndef RESIDENCE_H
#define RESIDENCE_H

#include "ownablesquare.h"

// Residences increase rent based on # owned (25, 50, 100, 200)
class Residence : public OwnableSquare {
public:
    Residence(int position, const std::string& name);
    void landOn(std::shared_ptr<Player> player) override;
};

#endif
