#include "nonownable.h"
#include "player.h"
#include "board.h"
#include <vector>
#include <random>
#include <memory>
#include <iostream>

using namespace std;

// Utility function for random numbers
int randint(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

// Constructors
NonOwnable::NonOwnable(int position, const std::string& name) : Square(position, name) {}

GoToTims::GoToTims(int position) : NonOwnable(position, "GO TO TIMS") {}

DCTimsLine::DCTimsLine(int position) : NonOwnable(position, "DC TIMS LINE") {}

CollectOSAP::CollectOSAP(int position) : NonOwnable(position, "COLLECT OSAP") {}

GooseNesting::GooseNesting(int position) : NonOwnable(position, "GOOSE NESTING") {}
      
Tuition::Tuition(int position) : NonOwnable(position, "TUITION") {}

CoopFee::CoopFee(int position) : NonOwnable(position, "COOP FEE") {}

SLC::SLC(int position) : NonOwnable(position, "SLC") {
    effects.push_back(make_shared<MoveEffect>(-3));
    effects.push_back(make_shared<MoveEffect>(-2));
    effects.push_back(make_shared<MoveEffect>(-1));
    effects.push_back(make_shared<MoveEffect>(1));
    effects.push_back(make_shared<MoveEffect>(2));
    effects.push_back(make_shared<MoveEffect>(3));
    effects.push_back(make_shared<GoToTimsLineEffect>());
    effects.push_back(make_shared<CollectOSAPEffect>());
}

// Initialize with board, to be updated later when board is available
SLC::SLC(int position, shared_ptr<Board> board) : NonOwnable(position, "SLC") {
    effects.push_back(make_shared<MoveEffect>(-3));
    effects.push_back(make_shared<MoveEffect>(-2));
    effects.push_back(make_shared<MoveEffect>(-1));
    effects.push_back(make_shared<MoveEffect>(1));
    effects.push_back(make_shared<MoveEffect>(2));
    effects.push_back(make_shared<MoveEffect>(3));
    effects.push_back(make_shared<GoToTimsLineEffect>());
    effects.push_back(make_shared<CollectOSAPEffect>());
    effects.push_back(make_shared<RollupCupEffect>(board));
}

NeedlesHall::NeedlesHall(int position) : NonOwnable(position, "NEEDLES HALL") {
    effects.push_back(make_shared<LoseMoneyEffect>(200));
    effects.push_back(make_shared<LoseMoneyEffect>(100));
    effects.push_back(make_shared<LoseMoneyEffect>(50));
    effects.push_back(make_shared<GainMoneyEffect>(25));
    effects.push_back(make_shared<GainMoneyEffect>(50));
    effects.push_back(make_shared<GainMoneyEffect>(100));
    effects.push_back(make_shared<GainMoneyEffect>(200));
}

// Initialize with board, to be updated later when board is available
NeedlesHall::NeedlesHall(int position, shared_ptr<Board> board) : NonOwnable(position, "NEEDLES HALL") {
    effects.push_back(make_shared<LoseMoneyEffect>(200));
    effects.push_back(make_shared<LoseMoneyEffect>(100));
    effects.push_back(make_shared<LoseMoneyEffect>(50));
    effects.push_back(make_shared<GainMoneyEffect>(25));
    effects.push_back(make_shared<GainMoneyEffect>(50));
    effects.push_back(make_shared<GainMoneyEffect>(100));
    effects.push_back(make_shared<GainMoneyEffect>(200));
    effects.push_back(make_shared<RollupCupEffect>(board));
}

// Class implementations
void GoToTims::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on GO TO TIMS. Moving to DC TIMS LINE." << endl;
    // move to tims line (position 10) directly
    player->setPosition(10);
    player->setInTimsLine(true);
    cout << player->getName() << " is now in DC TIMS LINE." << endl;
}

void DCTimsLine::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on DC TIMS LINE." << endl;
    // Only handle special logic if player is already in tims line
    // Landing on the space doesn't put you in it, just visiting
    if (player->isInTimsLine()) {
        cout << "You are already in the DC TIMS LINE." << endl;
    }
}

void DCTimsLine::releaseOptions(shared_ptr<Player> player) {
    cout << "Options to get out of DC TIMS LINE:" << endl;
    cout << "1. Pay $50" << endl;
    cout << "2. Use a Roll Up the Rim cup (if available)" << endl;
    cout << "3. Try to roll doubles" << endl;
    
    int choice;
    cout << "Choose an option (1-3): ";
    cin >> choice;
    
    switch (choice) {
        case 1:
            if (player->getMoney() >= 50) {
                player->updateMoney(-50);
                player->setInTimsLine(false);
                cout << player->getName() << " paid $50 to get out of DC TIMS LINE." << endl;
            } else {
                cout << "Not enough money!" << endl;
            }
            break;
        case 2:
            if (player->useRollupCup()) {
                cout << player->getName() << " used a Roll Up the Rim cup to get out of DC TIMS LINE." << endl;
            } else {
                cout << "No Roll Up the Rim cup available!" << endl;
            }
            break;
        case 3:
            cout << "Rolling dice to try for doubles..." << endl;
            // Actual dice roll should be handled by the Game class
            break;
        default:
            cout << "Invalid choice!" << endl;
    }
}

void CollectOSAP::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on COLLECT OSAP. Collecting $200." << endl;
    player->updateMoney(200);
}

void GooseNesting::landOn(shared_ptr<Player> player) {
    // Attacked by a flock of geese, nothing happens
    cout << player->getName() << " was attacked by a flight of geese! Nothing else happens..." << endl;
}

void Tuition::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on TUITION." << endl;
    int in;

    // Choose between paying $300 or 10% of net worth
    cout << "Pay Tuition - $300 (1) or 10% of net worth <" << player->getNetWorth() << "> (2)" << endl;

    if (cin >> in) {
        if (in == 1) {
            cout << "Option 1 selected - paying $300" << endl;
            player->updateMoney(-300);
            return;
        } 
    }

    cout << "Option 2 selected - paying 10% of net worth" << endl;
    int payment = player->getNetWorth() / 10;
    cout << "Paying $" << payment << endl;
    player->updateMoney(-payment);
}

void CoopFee::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on COOP FEE. Paying $150." << endl;
    player->updateMoney(-150);
}

void SLC::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on SLC. Drawing a card..." << endl;
    int RNG = randint(1, 27);  // Changed from 24 to 27 to add cup chance

    // Calculate prob
    if (RNG <= 3) { //1/9
        cout << "Card: Move back 3 spaces." << endl;
        effects[0]->apply(player);
    } else if (RNG <= 7) { //1/6.75
        cout << "Card: Move back 2 spaces." << endl;
        effects[1]->apply(player);
    } else if (RNG <= 11) { //1/6.75
        cout << "Card: Move back 1 space." << endl;
        effects[2]->apply(player);
    } else if (RNG <= 14) { //1/9
        cout << "Card: Move forward 1 space." << endl;
        effects[3]->apply(player);
    } else if (RNG <= 18) { //1/6.75
        cout << "Card: Move forward 2 spaces." << endl;
        effects[4]->apply(player);
    } else if (RNG <= 22) { //1/6.75
        cout << "Card: Move forward 3 spaces." << endl;
        effects[5]->apply(player);
    } else if (RNG <= 24) { //1/13.5
        cout << "Card: Go to DC TIMS LINE." << endl;
        effects[6]->apply(player);
    } else if (RNG <= 26) { //1/13.5
        cout << "Card: Advance to COLLECT OSAP and collect $200." << endl;
        effects[7]->apply(player);
    } else { //1/27
        cout << "Card: Win a Roll Up the Rim cup!" << endl;
        effects[8]->apply(player);
    }
}

void NeedlesHall::landOn(shared_ptr<Player> player) {
    cout << player->getName() << " landed on NEEDLES HALL. Drawing a card..." << endl;
    int RNG = randint(1, 20);  // Changed from 18 to 20 to add cup chance

    // Calculate prob
    if (RNG == 1) { //1/20
        cout << "Card: Pay $200." << endl;
        effects[0]->apply(player);
    } else if (RNG <= 3) { //1/10
        cout << "Card: Pay $100." << endl;
        effects[1]->apply(player);
    } else if (RNG <= 6) { //1/6.67
        cout << "Card: Pay $50." << endl;
        effects[2]->apply(player);
    } else if (RNG <= 12) { //1/3.33
        cout << "Card: Receive $25." << endl;
        effects[3]->apply(player);
    } else if (RNG <= 15) { //1/6.67
        cout << "Card: Receive $50." << endl;
        effects[4]->apply(player);
    } else if (RNG <= 17) { //1/10
        cout << "Card: Receive $100." << endl;
        effects[5]->apply(player);
    } else if (RNG <= 18) { //1/20
        cout << "Card: Receive $200." << endl;
        effects[6]->apply(player);
    } else { // 1/10 (19-20)
        cout << "Card: Win a Roll Up the Rim cup!" << endl;
        effects[7]->apply(player);
    }
}

// CardEffects
MoveEffect::MoveEffect(int spaces_moved) : spaces_moved{spaces_moved} {}

void MoveEffect::apply(shared_ptr<Player> player) {
    //check if passed go
    int old_pos = player->getPlayerPosition();
    if (old_pos + spaces_moved >= 40) {
        player->updateMoney(200);
        cout << player->getName() << " collected $200 for passing GO." << endl;
    }

    player->move(spaces_moved);

}

GainMoneyEffect::GainMoneyEffect(int money_gained) : money_gained{money_gained} {}

void GainMoneyEffect::apply(shared_ptr<Player> player)  {
    player->updateMoney(money_gained);
}

LoseMoneyEffect::LoseMoneyEffect(int money_lost) : money_lost{money_lost} {}

void LoseMoneyEffect::apply(shared_ptr<Player> player) {
    if (player->getMoney() < money_lost) {
        cout << player->getName() << " doesn't have enough money to pay $" << money_lost << "!" << endl;
        cout << "You need to mortgage properties or declare bankruptcy." << endl;
        return;
    }
    player->updateMoney(-money_lost);
}

void GoToTimsLineEffect::apply(shared_ptr<Player> player)  {
    // Move to timsline space
    player->setPosition(10);
    // Set inTimsLine to true
    player->setInTimsLine(true);
}

void CollectOSAPEffect::apply(shared_ptr<Player> player) {
    // Check if player is moving backward (past Go)
    int oldPosition = player->getPlayerPosition();
    player->setPosition(0);
    
    // Only award money if not moving backward
    if (oldPosition > 0) {
        player->updateMoney(200);
        cout << player->getName() << " collected $200 for passing GO." << endl;
    }
}

RollupCupEffect::RollupCupEffect(shared_ptr<Board> board) : board(board) {}

void RollupCupEffect::apply(shared_ptr<Player> player) {
    if (board->canAddCup()) {
        if (player->receiveRollupCup()) {
            board->addCup();
            cout << player->getName() << " won a Roll Up the Rim cup!" << endl;
        } else {
            cout << player->getName() << " already has the maximum number of Roll Up the Rim cups." << endl;
        }
    } else {
        cout << "No more Roll Up the Rim cups available in the game." << endl;
    }
}
