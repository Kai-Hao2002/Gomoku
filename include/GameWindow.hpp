#pragma once
#include <SFML/Graphics.hpp>
#include "Board.hpp"
#include "Player.hpp"
#include <memory>

class GameWindow {
public:
    GameWindow(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2, bool isPvP);
    bool run(sf::RenderWindow& window, sf::Font& font);

private:
    static constexpr int CELL_SIZE = 40;

    std::unique_ptr<Player> p1, p2;
    Player* currentPlayer;
    Board board;
    bool isPvP;
    bool gameOver;
    bool justRestarted = false;
    bool wantToReturnToMenu = false;
    bool wantToExit = false;
    int lastMoveRow = -1;
    int lastMoveCol = -1;
    char lastPlayerSymbol = ' ';

    char winner = ' '; // 'X', 'O', or ' ' for draw

    sf::RectangleShape restartButton;
    sf::RectangleShape exitButton;
    sf::Text restartText;
    sf::Text exitText;

    void handleEvents(sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    void displayResult(sf::RenderWindow& window, sf::Font& font);
};
