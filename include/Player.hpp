#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Board.hpp"

class Player {
public:
    Player(char symbol);
    virtual ~Player() {}
    virtual void makeMove(Board& board, int& row, int& col) = 0;
    char getSymbol() const;

protected:
    char symbol;
};

#endif
