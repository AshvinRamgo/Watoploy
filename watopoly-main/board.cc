#include "board.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include "squarefactory.h"
#include "academicbuilding.h"
#include "ownablesquare.h"

using namespace std;
const std::vector<std::vector<std::pair<int, int>>> TOKEN_COORDS_BY_SQUARE = {

    // BOTTOM OF BOARD 0-10
    { {61,81}, {61,82}, {61,83}, {61,84}, {61,85}, {61,86}, {61,87} }, // COLLECT OSAP  
    { {61,73}, {61,74}, {61,75}, {61,76}, {61,77}, {61,78}, {61,79} }, // AL  
    { {61,65}, {61,66}, {61,67}, {61,68}, {61,69}, {61,70}, {61,71} }, // SLC  
    { {61,57}, {61,58}, {61,59}, {61,60}, {61,61}, {61,62}, {61,63} }, // ML  
    { {61,49}, {61,50}, {61,51}, {61,52}, {61,53}, {61,54}, {61,55} }, // TUTION  
    { {61,41}, {61,42}, {61,43}, {61,44}, {61,45}, {61,46}, {61,47} }, // MKV  
    { {61,33}, {61,34}, {61,35}, {61,36}, {61,37}, {61,38}, {61,39} }, // ECH  
    { {61,25}, {61,26}, {61,27}, {61,28}, {61,29}, {61,30}, {61,31} }, // NEEDLES HALL  
    { {61,17}, {61,18}, {61,19}, {61,20}, {61,21}, {61,22}, {61,23} }, // PAS  
    { {61,9},  {61,10}, {61,11}, {61,12}, {61,13}, {61,14}, {61,15} }, // HH  
    { {61,1},  {61,2},  {61,3},  {61,4},  {61,5},  {61,6},  {61,7}  }, // DC TIMS LINE      

    // LEFT SIDE 
    { {55,1}, {55,2}, {55,3}, {55,4}, {55,5}, {55,6}, {55,7} }, // RCH  
    { {48,1}, {48,2}, {48,3}, {48,4}, {48,5}, {48,6}, {48,7} }, // PAC  
    { {44,1}, {44,2}, {44,3}, {44,4}, {44,5}, {44,6}, {44,7} }, // DWE  
    { {38,1}, {38,2}, {38,3}, {38,4}, {38,5}, {38,6}, {38,7} }, // CPH  
    { {31,1}, {31,2}, {31,3}, {31,4}, {31,5}, {31,6}, {31,7} }, // UWP  
    { {27,1}, {27,2}, {27,3}, {27,4}, {27,5}, {27,6}, {27,7} }, // LHI  
    { {21,1}, {21,2}, {21,3}, {21,4}, {21,5}, {21,6}, {21,7} }, // SLC  
    { {16,1}, {16,2}, {16,3}, {16,4}, {16,5}, {16,6}, {16,7} }, // BMH  
    { {10,1}, {10,2}, {10,3}, {10,4}, {10,5}, {10,6}, {10,7} }, // OPT      
    
    // TOP OF BOARD 20-30
    { {4,1},  {4,2},  {4,3},  {4,4},  {4,5},  {4,6},  {4,7}  }, // GOOSE NESTING
    { {4,9},  {4,10}, {4,11}, {4,12}, {4,13}, {4,14}, {4,15} }, // EV1
    { {4,17}, {4,18}, {4,19}, {4,20}, {4,21}, {4,22}, {4,23} }, // NEEDLES HALL
    { {4,25}, {4,26}, {4,27}, {4,28}, {4,29}, {4,30}, {4,31} }, // EV2
    { {4,33}, {4,34}, {4,35}, {4,36}, {4,37}, {4,38}, {4,39} }, // EV3
    { {4,41}, {4,42}, {4,43}, {4,44}, {4,45}, {4,46}, {4,47} }, // V1
    { {4,49}, {4,50}, {4,51}, {4,52}, {4,53}, {4,54}, {4,55} }, // PHYS
    { {4,57}, {4,58}, {4,59}, {4,60}, {4,61}, {4,62}, {4,63} }, // B1
    { {4,65}, {4,66}, {4,67}, {4,68}, {4,69}, {4,70}, {4,71} }, // CIF
    { {4,73}, {4,74}, {4,75}, {4,76}, {4,77}, {4,78}, {4,79} }, // B2    
    { {4, 81},{4,82}, {4,83}, {4,84}, {4,85}, {4,86}, {4,87}}, // GO TO TIMS

    // RIGHT SIDE 
    { {10,81}, {10,82}, {10,83}, {10,84}, {10,85}, {10,86}, {10,87} }, // EIT
    { {16,81}, {16,82}, {16,83}, {16,84}, {16,85}, {16,86}, {16,87} }, // ESC  
    { {21,81}, {21,82}, {21,83}, {21,84}, {21,85}, {21,86}, {21,87} }, // SLC  
    { {27,81}, {27,82}, {27,83}, {27,84}, {27,85}, {27,86}, {27,87} }, // C2  
    { {31,81}, {31,82}, {31,83}, {31,84}, {31,85}, {31,86}, {31,87} }, // REV  
    { {38,81}, {38,82}, {38,83}, {38,84}, {38,85}, {38,86}, {38,87} }, // NEEDLES HALL  
    { {44,81}, {44,82}, {44,83}, {44,84}, {44,85}, {44,86}, {44,87} }, // MC  
    { {48,81}, {48,82}, {48,83}, {48,84}, {48,85}, {48,86}, {48,87} }, // COOP FEE  
    { {55,81}, {55,82}, {55,83}, {55,84}, {55,85}, {55,86}, {55,87} }, // DC      

};

std::string getSquareNameForIndex(int i) {
    static const std::vector<std::string> names = {
        "COLLECT OSAP", "AL", "SLC", "ML", "TUITION", "MKV", "ECH", "NEEDLES HALL", "PAS", "HH",
        "DC TIMS LINE", "RCH", "PAC", "DWE", "CPH", "UWP", "LHI", "SLC", "BMH", "OPT",
        "GOOSE NESTING", "EV1", "NEEDLES HALL", "EV2", "EV3", "V1", "PHYS", "B1", "CIF GYM", "B2",
        "GO TO TIMS", "EIT", "ESC", "SLC", "C2", "REV", "NEEDLES HALL", "MC", "COOP FEE", "DC"
    };

    if (i >= 0 && i < static_cast<int>(names.size())) {
        return names[i];
    } else {
        return "UNKNOWN";
    }
}

Board::Board() : totalCups(0) {
    squares.resize(40);
}

void Board::initBoard() {
    SquareFactory factory;

    for (int i = 0; i < 40; ++i) {
        std::string squareName = getSquareNameForIndex(i);

        if (squareName == "SLC" || squareName == "NEEDLES HALL") {
            squares[i] = factory.createSquareWithBoard(i, squareName, shared_from_this());
        } else {
            squares[i] = factory.createSquare(i, squareName);
        }

        if (!squares[i]) {
            exit(1);
        }

        squares[i]->setTokenCoords(TOKEN_COORDS_BY_SQUARE[i]);
    }
}


shared_ptr<Square> Board::getSquare(int index) {
    if (index < 0 || index >= static_cast<int>(squares.size())) {
        throw out_of_range("Square index out of range: " + to_string(index));
    }
    return squares[index];
}

void Board::addPlayer(shared_ptr<Player> player) {
    if (find_if(players.begin(), players.end(), 
                [&player](const shared_ptr<Player>& p) { 
                    return p->getName() == player->getName(); 
                    
                }) == players.end()) {
        players.push_back(player);
        notifyObservers();
    }
}

void Board::removePlayer(shared_ptr<Player> player) {
    auto it = find_if(players.begin(), players.end(), 
                      [&player](const shared_ptr<Player>& p) {
                          return p->getName() == player->getName();
                      });

    if (it != players.end()) {
        players.erase(it);
        notifyObservers();
    }
}

vector<shared_ptr<Player>> Board::getPlayers() const {
    return players;
}

void Board::attach(Observer* observer) {
    Subject::attach(observer);
}

void Board::notifyObservers() const {
    for (auto obs : observers) {
        obs->notify(this);
    }
}


void Board::addCup() {
    if (totalCups >= MAX_CUPS) {
        throw runtime_error("Maximum number of Roll Up the Rim cups already in play");
    }
    totalCups++;
}

void Board::removeCup() {
    if (totalCups <= 0) {
        throw runtime_error("No Roll Up the Rim cups in play");
    }
    totalCups--;
}

int Board::getTotalCups() const {
    return totalCups;
}

bool Board::canAddCup() const {
    return totalCups < MAX_CUPS;
}

const vector<shared_ptr<Square>>& Board::getSquares() const {
    return squares;
}

shared_ptr<Square> Board::findSquareByName(const string& name) {
    auto it = find_if(squares.begin(), squares.end(),
                      [&name](const shared_ptr<Square>& square) {
                          return square->getName() == name;
                      });
    return (it != squares.end()) ? *it : nullptr;
}

vector<shared_ptr<Square>> Board::getMonopolySquares(const string& monopolyBlock) {
    vector<shared_ptr<Square>> result;
    for (auto& square : squares) {
        auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(square);
        if (academicBuilding && academicBuilding->getMonopolyBlock() == monopolyBlock) {
            result.push_back(square);
        }
    }
    return result;
}

bool Board::ownsMonopoly(shared_ptr<Player> player, const string& monopolyBlock) {
    auto monopolySquares = getMonopolySquares(monopolyBlock);
    for (auto& square : monopolySquares) {
        auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(square);
        if (!ownableSquare || !ownableSquare->isOwned() || 
            ownableSquare->getOwner() != player) {
            return false;
        }
    }
    return !monopolySquares.empty();
}

void Board::saveBoardState(ostream& out) const {
    for (size_t i = 0; i < squares.size(); i++) {
        auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(squares[i]);
        if (ownableSquare) {
            out << squares[i]->getName() << " ";
            out << (ownableSquare->isOwned() ? ownableSquare->getOwner()->getName() : "BANK") << " ";
            if (ownableSquare->isMortgaged()) {
                out << "-1";
            } else {
                auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(squares[i]);
                out << (academicBuilding ? academicBuilding->getImprovementLevel() : 0);
            }
            out << endl;
        }
    }
    out << totalCups << endl;
}

void Board::loadBoardState(istream& in) {
    if (squares.empty()) {
        initBoard();
    }

    unordered_map<string, int> propertyIndices;
    for (size_t i = 0; i < squares.size(); i++) {
        propertyIndices[squares[i]->getName()] = static_cast<int>(i);
    }

    for (size_t i = 0; i < squares.size(); i++) {
        string propertyName, ownerName;
        int improvements;
        in >> propertyName >> ownerName >> improvements;

        if (propertyIndices.find(propertyName) != propertyIndices.end()) {
            int index = propertyIndices[propertyName];
            auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(squares[index]);

            if (ownableSquare) {
                if (ownerName != "BANK") {
                    auto playerIt = find_if(players.begin(), players.end(),
                                            [&ownerName](const shared_ptr<Player>& p) {
                                                return p->getName() == ownerName;
                                            });

                    if (playerIt != players.end()) {
                        ownableSquare->setOwner(*playerIt);
                        (*playerIt)->addProperty(ownableSquare);
                    }
                }

                if (improvements == -1) {
                    ownableSquare->setMortgage();
                } else {
                    auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(squares[index]);
                    if (academicBuilding && improvements > 0) {
                        academicBuilding->setImprovement(improvements);
                    }
                }
            }
        }
    }

    in >> totalCups;
    notifyObservers();
}

void Board::drawBoard() const {
    notifyObservers();
}
