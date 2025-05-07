#ifndef GAME_HPP
#define GAME_HPP

#include "Board.hpp"
#include "Player.hpp"
#include <memory>

class Game {
public:
    Game(Player* p1, Player* p2);
    void run();

private:
    Board board;
    std::unique_ptr<Player> players[2];
};

#endif
