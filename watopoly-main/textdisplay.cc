#include "textdisplay.h"
#include "board.h"
#include "academicbuilding.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>
#include <cstdlib> // For getenv
#include <filesystem>

using namespace std;

// might have to change offset depending -> will know when have all implementaion 

bool isPlayerToken(char ch) {
    return (ch == 'G' || ch == 'B' || ch == 'D' || ch == 'P' ||
            ch == 'S' || ch == '$' || ch == 'L' || ch == 'T');
}

TextDisplay::TextDisplay(Board* board) : board(board) {
    initializeDisplay();
    // populateCells();
    drawPlayersOnBoard();
}

void TextDisplay::initializeDisplay() {
    ifstream infile{"board.txt"};
    
    string line;
    theDisplay.clear();
    while(getline(infile, line)) {
        vector<char> row(line.begin(), line.end());
        theDisplay.push_back(row);
    }
    infile.close();
}

void TextDisplay::drawPlayersOnBoard() {
    for (const auto &coord : previousTokenCoords) {
        int r = coord.first;
        int c = coord.second;
        if (r >= 0 && r < static_cast<int>(theDisplay.size()) &&
            c >= 0 && c < static_cast<int>(theDisplay[r].size()))
        {
            if (isPlayerToken(theDisplay[r][c])) {
                theDisplay[r][c] = ' ';
            }
        }
    }
    previousTokenCoords.clear();

    auto players = board->getPlayers();
    auto squares = board->getSquares();

    for (auto &player : players) {
        int pos = player->getPlayerPosition();
        char token = player->getNamePieces()[0];

        if (pos < 0 || pos >= static_cast<int>(squares.size())) {
            continue;
        }

        auto square = squares[pos];
        const auto &coords = square->getTokenCoords();

        int index = std::distance(players.begin(), std::find(players.begin(), players.end(), player));
        if (index < static_cast<int>(coords.size())) {
            auto [r, c] = coords[index];
            if (r >= 0 && r < static_cast<int>(theDisplay.size()) &&
                c >= 0 && c < static_cast<int>(theDisplay[r].size()))
            {
                theDisplay[r][c] = token;
                previousTokenCoords.push_back({r, c});
            }
        }
    }
}

void TextDisplay::drawPropertyOwnership() {
    auto squares = board->getSquares();

    for (int i = 0; i < static_cast<int>(squares.size()); i++) {
        auto ownableSquare = dynamic_pointer_cast<OwnableSquare>(squares[i]);
        if (!ownableSquare) continue;

        std::string ownerString = "own:  ";  // Default to empty ownership

        if (ownableSquare->isOwned()) {
            auto owner = ownableSquare->getOwner();
            char token = owner->getNamePieces()[0];
            ownerString[4] = token;  // Replace blank space with token if owned

            if (ownableSquare->isMortgaged()) {
                ownerString += " (M)";
            }
        }

        // Get token coordinates and shift row down by 1
        const auto& coords = squares[i]->getTokenCoords();
        if (coords.empty()) continue;

        // Use the leftmost token position as anchor
        int baseRow = coords[0].first;
        int baseCol = coords[0].second;

        int targetRow = baseRow + 1;  // row below token display

        // Safety bounds check
        if (targetRow >= static_cast<int>(theDisplay.size())) continue;

        // Write the string on the board
        for (size_t j = 0; j < ownerString.size(); ++j) {
            int col = baseCol + static_cast<int>(j);
            if (col >= 0 && col < static_cast<int>(theDisplay[targetRow].size())) {
                theDisplay[targetRow][col] = ownerString[j];
            }
        }
    }
}
 
void TextDisplay::drawImprovements() {
    auto squares = board->getSquares();

    for (int i = 0; i < static_cast<int>(squares.size()); i++) {
        auto academicBuilding = dynamic_pointer_cast<AcademicBuilding>(squares[i]);
        if (!academicBuilding) continue;

        int level = academicBuilding->getImprovementLevel();
        if (level == 0) continue;

        const auto& coords = squares[i]->getTokenCoords();
        if (coords.empty()) continue;

        int baseRow = coords[0].first;
        int baseCol = coords[0].second;
        int targetRow = baseRow - 3;

        if (targetRow < 0 || targetRow >= static_cast<int>(theDisplay.size())) continue;

        // Pick letter: 'B' for bathroom (1–4), 'C' for cafeteria (5)
        char ch = (level == 5) ? 'C' : 'B';

        // Write the character at baseCol, and maybe repeat a few to make it more visible
        for (int j = 0; j < std::min(level, 5); ++j) {
            int col = baseCol + j;
            if (col >= 0 && col < static_cast<int>(theDisplay[targetRow].size())) {
                theDisplay[targetRow][col] = ch;
            }
        }
    }
}



void TextDisplay::notify(const Subject* whoNotified) {
    initializeDisplay();
    drawPlayersOnBoard();
    drawPropertyOwnership();
    drawImprovements();
}

void TextDisplay::updateBoard(Cell* cell) {
    // For simplicity, redraw the entire board.
    notify(board);
}

ostream& operator<<(ostream& out, const TextDisplay& td) {
    for (const auto &row : td.theDisplay) {
        for (char ch : row) {
            out << ch;
        }
        out << "\n";
    }
    return out;
}

