#pragma once
#include <SFML/Graphics.hpp>
#include "Board.hpp"
#include "Player.hpp"
#include <memory>

class GameWindow {
public:
    GameWindow(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2, bool isPvP);
    void run(sf::RenderWindow& window);

private:
    static constexpr int CELL_SIZE = 40;

    std::unique_ptr<Player> p1, p2;
    Player* currentPlayer;
    Board board;
    bool isPvP;
    bool gameOver;
    bool waitingForExitClick;
    bool justRestarted = false;


    sf::Text resultText;
    sf::Text restartButton;
    sf::Text exitButton;

    void handleEvents(sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    void displayResult(sf::RenderWindow& window);
};