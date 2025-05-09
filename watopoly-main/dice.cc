#include "dice.h"
#include <cstdlib>
#include <ctime>

using namespace std;

Dice::Dice() {
    srand(static_cast<unsigned int>(time(nullptr)));
}

pair<int, int> Dice::roll() {
    int d1 = (rand() % 6) + 1;
    int d2 = (rand() % 6) + 1;
    return {d1, d2};
}
