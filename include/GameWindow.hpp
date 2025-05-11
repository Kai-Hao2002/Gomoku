#pragma once
#include <SFML/Graphics.hpp>
#include "Board.hpp"
#include "Player.hpp"
#include <memory>
#include <SFML/System.hpp>  // 引入 sf::Clock
#include <cmath>  // 引入 <cmath> 库以使用 sin 函数

class GameWindow {
public:
    GameWindow(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2, bool isPvP);
    bool run(sf::RenderWindow& window, sf::Font& font);

private:
    int boardSize = 15; // 15x15 棋盤

    int CELL_SIZE = 600 / boardSize;  

    char currentPlayerSymbol = 'X'; // 初始為 Player 1（黑子）
    sf::Clock clock;

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
    float restartHoverAlpha = 0.f;
    float exitHoverAlpha = 0.f;


    char winner = ' '; // 'X', 'O', or ' ' for draw

    sf::RectangleShape restartButton;
    sf::RectangleShape exitButton;
    sf::Text restartText;
    sf::Text exitText;

    void handleEvents(sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window,sf::Font& font);
    void displayResult(sf::RenderWindow& window, sf::Font& font);
};
