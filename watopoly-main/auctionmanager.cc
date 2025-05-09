#include "auctionmanager.h"
#include <iostream>
#include <limits>
#include <algorithm>

AuctionManager::AuctionManager() : winningPlayer(nullptr), winningBid(0) {}

void AuctionManager::startAuction(std::shared_ptr<OwnableSquare> property,
                                  const std::vector<std::shared_ptr<Player>>& players) {
    std::cout << "Auction for " << property->getName() << " begins!" << std::endl;
    
    winningBid = 0;
    winningPlayer = nullptr;
    
    // Run multiple rounds of bidding
    bool continueBidding = true;
    int round = 1;
    std::vector<std::shared_ptr<Player>> activeBidders;
    
    // Initialize active bidders with non-bankrupt players
    for (auto& player : players) {
        if (!player->isBankrupt()) {
            activeBidders.push_back(player);
        }
    }
    
    while (continueBidding && !activeBidders.empty()) {
        std::cout << "\nBidding round " << round << std::endl;
        std::cout << "Current high bid: $" << winningBid << std::endl;
        
        if (winningPlayer) {
            std::cout << "Current high bidder: " << winningPlayer->getName() << std::endl;
        }
        
        std::vector<std::shared_ptr<Player>> stillActive;
        
        for (auto& player : activeBidders) {
            std::cout << player->getName() << ", enter your bid (higher than $" 
                     << winningBid << ") or 0 to pass: ";
            int bid;
            std::cin >> bid;
            
            // Clear any invalid input
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                bid = 0;
            }

            if (bid <= 0) {
                std::cout << player->getName() << " passes." << std::endl;
                continue;
            }
            
            if (bid <= winningBid) {
                std::cout << "Bid must be higher than $" << winningBid << ". " 
                         << player->getName() << " passes." << std::endl;
                continue;
            }
            
            if (bid > player->getMoney()) {
                std::cout << "You don't have enough money for that bid! " 
                         << player->getName() << " has $" << player->getMoney() << std::endl;
                continue;
            }

            // Valid bid
            winningBid = bid;
            winningPlayer = player;
            stillActive.push_back(player);
            std::cout << player->getName() << " bids $" << bid << std::endl;
        }
        
        // Update active bidders for next round
        activeBidders = stillActive;
        
        // End auction if no one placed a bid this round
        if (stillActive.empty()) {
            continueBidding = false;
        }
        
        round++;
    }
    
    // Announce the result
    if (winningPlayer) {
        std::cout << "\nAuction ended! " << winningPlayer->getName() 
                 << " won with a bid of $" << winningBid << std::endl;
    } else {
        std::cout << "\nNo one bid on " << property->getName() << ". It remains with the bank." << std::endl;
    }
}

std::pair<std::shared_ptr<Player>, int> AuctionManager::getAuctionResult() const {
    return {winningPlayer, winningBid};
}

