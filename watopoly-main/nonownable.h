#ifndef NONOWNABLE_H
#define NONOWNABLE_H

#include <vector>
#include <memory>
#include "square.h"

// Forward declarations
class Player;
class CardEffect;
class Board;

class NonOwnable : public Square {
public:
    NonOwnable(int position, const std::string& name);
    virtual void landOn(std::shared_ptr<Player> player) = 0;
};

class GoToTims : public NonOwnable {
public:
    GoToTims(int position);
    void landOn(std::shared_ptr<Player> player) override;
};

class DCTimsLine : public NonOwnable {
public:
    DCTimsLine(int position);
    void landOn(std::shared_ptr<Player> player) override;
    void releaseOptions(std::shared_ptr<Player> player);
};

class CollectOSAP : public NonOwnable {
public:
    CollectOSAP(int position);
    void landOn(std::shared_ptr<Player> player) override;
};

class GooseNesting : public NonOwnable {
public:
    GooseNesting(int position);
    void landOn(std::shared_ptr<Player> player) override;
};

class Tuition : public NonOwnable {
public:
    Tuition(int position);
    void landOn(std::shared_ptr<Player> player) override;
};

class CoopFee : public NonOwnable {
public:
    CoopFee(int position);
    void landOn(std::shared_ptr<Player> player) override;
};

class SLC : public NonOwnable {
private:
    std::vector<std::shared_ptr<CardEffect>> effects;

public:
    SLC(int position);
    SLC(int position, std::shared_ptr<Board> board);
    void landOn(std::shared_ptr<Player> player) override;
};

class NeedlesHall : public NonOwnable {
private:
    std::vector<std::shared_ptr<CardEffect>> effects;

public:
    NeedlesHall(int position);
    NeedlesHall(int position, std::shared_ptr<Board> board);
    void landOn(std::shared_ptr<Player> player) override;
};

class CardEffect {
public:
    virtual void apply(std::shared_ptr<Player> player) = 0;
    virtual ~CardEffect() = default;
};

class MoveEffect : public CardEffect {
private:
    int spaces_moved;

public:
    MoveEffect(int spaces_moved);
    void apply(std::shared_ptr<Player> player) override;
};

class GainMoneyEffect : public CardEffect {
private:
    int money_gained;

public:
    GainMoneyEffect(int money_gained);
    void apply(std::shared_ptr<Player> player) override;
};

class LoseMoneyEffect : public CardEffect {
private:
    int money_lost;

public:
    LoseMoneyEffect(int money_lost);
    void apply(std::shared_ptr<Player> player) override;
};

class GoToTimsLineEffect : public CardEffect {
public:
    void apply(std::shared_ptr<Player> player) override;
};

class CollectOSAPEffect : public CardEffect {
public:
    void apply(std::shared_ptr<Player> player) override;
};

class RollupCupEffect : public CardEffect {
private:
    std::shared_ptr<Board> board;
    
public:
    RollupCupEffect(std::shared_ptr<Board> board);
    void apply(std::shared_ptr<Player> player) override;
};

#endif // NONOWNABLE_H 
