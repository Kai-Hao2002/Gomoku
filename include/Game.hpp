#ifndef GAME_HPP
#define GAME_HPP

#include "Board.hpp"
#include "Player.hpp"
#include <memory>
#include <SFML/Graphics.hpp>

class Game {
public:
    Game(Player* p1, Player* p2);
    void run(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

private:
    Board board;
    std::unique_ptr<Player> players[2];
};

#endif // GAME_HPP
