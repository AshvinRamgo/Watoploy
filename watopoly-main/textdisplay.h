#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include "observer.h"
#include "subject.h"
#include "board.h"
#include "cell.h"
#include "player.h"
#include "square.h"

class TextDisplay : public Observer {
private:
    Board* board;
    std::vector<std::vector<char>> theDisplay;
    std::map<int, Cell> cells;
    // Track coordinates where player tokens were drawn.
    std::vector<std::pair<int, int>> previousTokenCoords;
    std::chrono::steady_clock::time_point lastNotifyTime; // For debouncing

    void initializeDisplay();
    // void populateCells();
    void drawPlayersOnBoard();
    void drawPropertyOwnership();
    void drawImprovements();

public:
    TextDisplay(Board* board);
    void notify(const Subject* whoNotified) override;
    void updateBoard(Cell* cell);

    friend std::ostream& operator<<(std::ostream& out, const TextDisplay& td);
};

#endif
