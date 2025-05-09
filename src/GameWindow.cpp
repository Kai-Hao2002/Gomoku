// GameWindow.cpp
#include "GameWindow.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include <iostream>

GameWindow::GameWindow(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2, bool isPvP)
    : p1(std::move(p1)), p2(std::move(p2)), isPvP(isPvP), gameOver(false), waitingForExitClick(false) {
    currentPlayer = this->p1.get();
}

void GameWindow::run(sf::RenderWindow& window) {
    while (window.isOpen()) {
        handleEvents(window);
        update();
        window.clear(sf::Color::White);
        draw(window);
        window.display();
    }
}

void GameWindow::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
            int col = static_cast<int>(worldPos.x) / CELL_SIZE;
            int row = static_cast<int>(worldPos.y) / CELL_SIZE;

            if (gameOver) {
                std::cout << "Clicked at: " << worldPos.x << ", " << worldPos.y << "\n";
                std::cout << "Restart bounds: " << restartButton.getGlobalBounds().left << ","
                          << restartButton.getGlobalBounds().top << " - size: "
                          << restartButton.getGlobalBounds().width << "x"
                          << restartButton.getGlobalBounds().height << "\n";

                if (restartButton.getGlobalBounds().contains(worldPos)) {
                    std::cout << "Restart clicked!\n";
                    board.reset();
                    justRestarted = true;  // ← 加在這裡
                    gameOver = false;
                    currentPlayer = p1.get();
                    return;
                }
                if (exitButton.getGlobalBounds().contains(worldPos)) {
                    std::cout << "Exit clicked!\n";
                    window.close();
                    return;
                }
            } else if (!gameOver && currentPlayer == p1.get()) {
                if (row >= 0 && row < Board::SIZE && col >= 0 && col < Board::SIZE) {
                    if (board.placePiece(row, col, currentPlayer->getSymbol())) {
                        if (board.isWin(row, col, currentPlayer->getSymbol()) || board.isFull()) {
                            gameOver = true;
                        } else {
                            currentPlayer = p2.get();
                        }
                    }
                }
            }
        }
    }
}

void GameWindow::update() {
    if (gameOver || isPvP || justRestarted) {
        justRestarted = false;  // 清除 flag
        return;
    }

    if (currentPlayer == p2.get()) {
        int row, col;
        currentPlayer->makeMove(board, row, col);
        if (board.placePiece(row, col, currentPlayer->getSymbol())) {
            if (board.isWin(row, col, currentPlayer->getSymbol()) || board.isFull()) {
                gameOver = true;
            } else {
                currentPlayer = p1.get();
            }
        }
    }
}


void GameWindow::draw(sf::RenderWindow& window) {
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            cell.setPosition(j * CELL_SIZE + 1, i * CELL_SIZE + 1);
            cell.setFillColor(sf::Color(230, 200, 150));
            window.draw(cell);
        }
    }

    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            char val = board.getCell(i, j);
            if (val == 'X' || val == 'O') {
                sf::CircleShape piece(CELL_SIZE / 2 - 6);
                piece.setPosition(j * CELL_SIZE + 6, i * CELL_SIZE + 6);
                piece.setFillColor(val == 'X' ? sf::Color::Black : sf::Color::White);
                window.draw(piece);
            }
        }
    }

    if (gameOver) displayResult(window);
}

void GameWindow::displayResult(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return;
    }

    resultText.setFont(font);
    resultText.setCharacterSize(30);
    resultText.setFillColor(sf::Color::White);
    resultText.setString(
        board.isFull()
            ? "It's a draw!"
            : "Player " + std::string(1, currentPlayer->getSymbol()) + " wins!"
    );
    resultText.setPosition(160, 200);

    restartButton.setFont(font);
    restartButton.setString("Restart");
    restartButton.setCharacterSize(24);
    restartButton.setFillColor(sf::Color::Green);
    restartButton.setPosition(180, 280);

    exitButton.setFont(font);
    exitButton.setString("Exit");
    exitButton.setCharacterSize(24);
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setPosition(350, 280);

    window.draw(resultText);
    window.draw(restartButton);
    window.draw(exitButton);
}