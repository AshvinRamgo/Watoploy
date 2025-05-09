#include "square.h"

Square::Square(int position, std::string name) : 
    position(position), name(name) {}

std::string Square::getName() const {
    return name;
}

int Square::getPosition() const {
    return position;
}


void Square::setTokenCoords(const std::vector<std::pair<int, int>> &coords) {
    tokenCoords = coords;
}

const std::vector<std::pair<int, int>> &Square::getTokenCoords() const {
    return tokenCoords;
}
