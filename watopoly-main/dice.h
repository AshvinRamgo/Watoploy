#ifndef DICE_H
#define DICE_H

#include <utility>

class Dice {
public:
    Dice();
    std::pair<int, int> roll();
};

#endif
