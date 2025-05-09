#ifndef AUCTIONMANAGER_H
#define AUCTIONMANAGER_H

#include <memory>
#include <vector>
#include "ownablesquare.h"
#include "player.h"

class AuctionManager {
private:
    std::shared_ptr<Player> winningPlayer;
    int winningBid;

public:
    AuctionManager();

    void startAuction(std::shared_ptr<OwnableSquare> property,
                      const std::vector<std::shared_ptr<Player>>& players);

    std::pair<std::shared_ptr<Player>, int> getAuctionResult() const;
};

#endif // AUCTIONMANAGER_H

