#ifndef SQUAREFACTORY_H
#define SQUAREFACTORY_H

#include <memory>
#include <string>
#include "square.h"
#include "board.h"

class SquareFactory {
public:
    static std::shared_ptr<Square> createSquare(int position, const std::string& name);
    static std::shared_ptr<Square> createSquareWithBoard(int position, const std::string& name, std::shared_ptr<Board> board);
};

#endif
