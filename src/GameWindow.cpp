#include "GameWindow.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include <iostream>

GameWindow::GameWindow(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2, bool isPvP)
    : p1(std::move(p1)), p2(std::move(p2)), isPvP(isPvP), gameOver(false) {
    currentPlayer = this->p1.get();
    lastPlayerSymbol = ' ';  // 初始化最後下棋的玩家符號
    lastMoveRow = -1;        // 初始化最後下棋的行
    lastMoveCol = -1;        // 初始化最後下棋的列
}

bool GameWindow::run(sf::RenderWindow& window, sf::Font& font) {
    wantToReturnToMenu = false;
    wantToExit = false;
    gameOver = false;
    currentPlayer = p1.get();
    board = Board();  // 重置棋盤
    justRestarted = true;

    while (window.isOpen() && !wantToReturnToMenu && !wantToExit) {
        handleEvents(window);
        update();

        window.clear(sf::Color::White);
        draw(window);
        if (gameOver) {
            displayResult(window, font);  // 顯示遊戲結束畫面
        }
        window.display();
    }

    return wantToReturnToMenu; // true 表示回主選單，false 表示退出
}

void GameWindow::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            wantToExit = true;
            window.close();
            return;
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (gameOver) {
                // 檢查 Restart 按鈕是否被點擊
                if (restartButton.getGlobalBounds().contains(worldPos)) {
                    wantToReturnToMenu = true;
                    return;
                }

                // 檢查 Exit 按鈕是否被點擊
                if (exitButton.getGlobalBounds().contains(worldPos)) {
                    wantToExit = true;
                    window.close();
                    return;
                }

            } else if (currentPlayer == p1.get() || currentPlayer == p2.get()) {
                int col = static_cast<int>(worldPos.x) / CELL_SIZE;
                int row = static_cast<int>(worldPos.y) / CELL_SIZE;

                if (row >= 0 && row < Board::SIZE && col >= 0 && col < Board::SIZE) {
                    if (board.placePiece(row, col, currentPlayer->getSymbol())) {
                        lastMoveRow = row;
                        lastMoveCol = col;
                        lastPlayerSymbol = currentPlayer->getSymbol();

                        if (board.isWin(row, col, currentPlayer->getSymbol()) || board.isFull()) {
                            gameOver = true;
                        } else {
                            currentPlayer = (currentPlayer == p1.get()) ? p2.get() : p1.get();
                        }
                    }
                }
            }
        }
    }
}

void GameWindow::update() {
    if (gameOver || isPvP) return;

    if (currentPlayer == p2.get()) {
        int row, col;
        currentPlayer->makeMove(board, row, col);
        if (board.placePiece(row, col, currentPlayer->getSymbol())) {
            lastMoveRow = row;
            lastMoveCol = col;
            lastPlayerSymbol = currentPlayer->getSymbol();

            if (board.isWin(row, col, currentPlayer->getSymbol()) || board.isFull()) {
                gameOver = true;
            } else {
                currentPlayer = p1.get();
            }
        }
    }
}

void GameWindow::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(255, 248, 220)); // 背景色：Cornsilk

    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            cell.setPosition(j * CELL_SIZE + 1, i * CELL_SIZE + 1);
            cell.setFillColor(sf::Color(245, 222, 179)); // 淺木色
            cell.setOutlineThickness(1);
            cell.setOutlineColor(sf::Color(160, 82, 45)); // 深棕線
            window.draw(cell);
        }
    }

    // 繪製棋子
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            char val = board.getCell(i, j);
            if (val == 'X' || val == 'O') {
                sf::CircleShape piece(CELL_SIZE / 2 - 6);
                piece.setPosition(j * CELL_SIZE + 6, i * CELL_SIZE + 6);
                piece.setFillColor(val == 'X' ? sf::Color::Black : sf::Color::White);
                piece.setOutlineThickness(2);
                piece.setOutlineColor(sf::Color::Black);
                window.draw(piece);
            }
        }
    }
}
void GameWindow::displayResult(sf::RenderWindow& window, sf::Font& font) {
    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(40);
    resultText.setFillColor(sf::Color(50, 50, 50)); // 深灰色
    resultText.setStyle(sf::Text::Bold);

    if (board.isFull() && !board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        resultText.setString("It's a draw!");
    } else if (board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        resultText.setString(
            lastPlayerSymbol == 'X' ? "Player 1 (X) wins!" : "Player 2 (O) wins!"
        );
    }

    // 結果置中
    resultText.setPosition(640 / 2 - resultText.getLocalBounds().width / 2, 180);
    window.draw(resultText);

    // Restart Button
    restartButton.setSize(sf::Vector2f(200, 50));
    restartButton.setFillColor(sf::Color(70, 130, 180)); // Steel Blue
    restartButton.setPosition(220, 300);
    window.draw(restartButton);

    restartText.setFont(font);
    restartText.setCharacterSize(24);
    restartText.setString("Restart");
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(
        restartButton.getPosition().x + (200 - restartText.getLocalBounds().width) / 2,
        restartButton.getPosition().y + 8
    );
    window.draw(restartText);

    // Exit Button
    exitButton.setSize(sf::Vector2f(200, 50));
    exitButton.setFillColor(sf::Color(220, 20, 60)); // Crimson
    exitButton.setPosition(220, 380);
    window.draw(exitButton);

    exitText.setFont(font);
    exitText.setCharacterSize(24);
    exitText.setString("Exit");
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(
        exitButton.getPosition().x + (200 - exitText.getLocalBounds().width) / 2,
        exitButton.getPosition().y + 8
    );
    window.draw(exitText);
}
