#ifndef HUMANPLAYER_HPP
#define HUMANPLAYER_HPP

#include "Player.hpp"
#include <iostream>

class HumanPlayer : public Player {
public:
    HumanPlayer(char symbol);
    void makeMove(Board& board, int& row, int& col) override;
};

#endif
