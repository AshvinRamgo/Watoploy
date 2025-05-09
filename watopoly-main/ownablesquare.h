#ifndef OWNABLESQUARE_H
#define OWNABLESQUARE_H

#include <memory>
#include <string>
#include "square.h"

// Forward declaration
class Player;

class OwnableSquare : public Square {
protected:
    int price;
    std::shared_ptr<Player> owner;
    bool mortgaged;

public:
    OwnableSquare(int position, const std::string& name, int price);
    virtual ~OwnableSquare() = default;

    bool purchase(std::shared_ptr<Player> player);
    void mortgage(); // Gives half value to player 
    void setMortgage(); //no  money to player is given
    void unmortgage(); // Costs 60% to unmortgage 
 
    std::shared_ptr<Player> getOwner() const;
    bool isOwned() const;
    bool isMortgaged() const;
    void setOwner(std::shared_ptr<Player> player);
    virtual char getOwnerInitial() const;
    void resetOwnership();

    int getPurchasePrice() const;

    virtual void landOn(std::shared_ptr<Player> player) override = 0;
};

#endif

