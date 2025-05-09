#ifndef GYM_H
#define GYM_H

#include "ownablesquare.h"

class Gym : public OwnableSquare {
public:
    Gym(int position, const std::string& name);
    void landOn(std::shared_ptr<Player> player) override;
};

#endif
