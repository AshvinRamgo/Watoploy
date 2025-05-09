#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "subject.h"
#include "observer.h"
#include "square.h"
#include "player.h"

class Board : public Subject, public std::enable_shared_from_this<Board> {
private:
    std::vector<std::shared_ptr<Square>> squares;
    std::vector<std::shared_ptr<Player>> players;
    int totalCups;
    static const int MAX_CUPS = 4;

public:
    Board();
    void initBoard();
    std::shared_ptr<Square> getSquare(int index);

    void addPlayer(std::shared_ptr<Player> player);
    void removePlayer(std::shared_ptr<Player> player);
    std::vector<std::shared_ptr<Player>> getPlayers() const;

    void attach(Observer* observer) override;
    void notifyObservers() const override;

    void addCup();
    void removeCup();
    int getTotalCups() const;
    bool canAddCup() const;

    const std::vector<std::shared_ptr<Square>>& getSquares() const;
    std::shared_ptr<Square> findSquareByName(const std::string& name);
    std::vector<std::shared_ptr<Square>> getMonopolySquares(const std::string& monopolyBlock);
    bool ownsMonopoly(std::shared_ptr<Player> player, const std::string& monopolyBlock);

    void saveBoardState(std::ostream& out) const;
    void loadBoardState(std::istream& in);

    void drawBoard() const;
};

#endif
