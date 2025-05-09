#ifndef SQUARE_H
#define SQUARE_H

#include <string>
#include <memory>
#include <vector>
#include <utility>

class Player;

class Square: public std::enable_shared_from_this<Square> {
protected:
    int position;
    std::string name;

private: 
    std::vector<std::pair<int, int>> tokenCoords;

public:
    Square(int position, std::string name);
    virtual ~Square() = default;

    virtual void landOn(std::shared_ptr<Player> player) = 0;

    std::string getName() const;
    int getPosition() const;

    void setTokenCoords(const std::vector<std::pair<int, int>> &coords);
    const std::vector<std::pair<int, int>> &getTokenCoords() const;
};

#endif
