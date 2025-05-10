#include "GameWindow.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include <iostream>
#include <SFML/System.hpp>  // 引入 sf::Clock
#include <cmath>  // 引入 <cmath> 库以使用 sin 函数

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
    // 滑鼠位置
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool hoveringRestart = restartButton.getGlobalBounds().contains(mousePos);
    bool hoveringExit = exitButton.getGlobalBounds().contains(mousePos);

    // 更新 hover alpha 值（漸變動畫）
    restartHoverAlpha = hoveringRestart ? std::min(255.f, restartHoverAlpha + 10.f) : std::max(0.f, restartHoverAlpha - 10.f);
    exitHoverAlpha = hoveringExit ? std::min(255.f, exitHoverAlpha + 10.f) : std::max(0.f, exitHoverAlpha - 10.f);

    // 結果文字
    sf::Text resultText;
    sf::Clock clock;
    resultText.setFont(font);
    resultText.setCharacterSize(48);  // 放大文字
    resultText.setFillColor(sf::Color::Yellow);  // 高亮顯示結果
    float animScale = 1.f + 0.2f * sin(clock.getElapsedTime().asSeconds() * 2.f);  // 動畫效果：逐漸放大

    if (board.isFull() && !board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        resultText.setString("It's a draw!");
    } else if (board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        if (lastPlayerSymbol == 'X')
            resultText.setString("Player 1 (X) wins!");
        else
            resultText.setString("Player 2 (O) wins!");
    }

    resultText.setPosition(
        640 / 2 - resultText.getLocalBounds().width / 2,
        640 / 2 - resultText.getLocalBounds().height / 2 - 100
    );
    window.draw(resultText);

    // 動畫效果：文字放大
    resultText.setScale(animScale, animScale);
    window.draw(resultText); // 畫出結果文字

    // ===== Restart 按鈕繪製 =====
    restartButton.setSize(sf::Vector2f(200, 50));
    restartButton.setPosition(220, 300);
    sf::Color restartColor(70, 130, 180, 255 - static_cast<int>(restartHoverAlpha));
    restartButton.setFillColor(restartColor);
    window.draw(restartButton);

    sf::RectangleShape restartOutline(restartButton);
    restartOutline.setFillColor(sf::Color::Transparent);
    restartOutline.setOutlineThickness(2);
    restartOutline.setOutlineColor(sf::Color::White);
    window.draw(restartOutline);

    for (int i = 0; i < 4; ++i) {
        sf::CircleShape corner(10);
        corner.setFillColor(restartColor);
        float x = (i % 2 == 0) ? restartButton.getPosition().x : restartButton.getPosition().x + restartButton.getSize().x - 20;
        float y = (i < 2) ? restartButton.getPosition().y : restartButton.getPosition().y + restartButton.getSize().y - 20;
        corner.setPosition(x, y);
        window.draw(corner);
    }

    restartText.setFont(font);
    restartText.setCharacterSize(24);
    restartText.setString("Restart");
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(
        restartButton.getPosition().x + (200 - restartText.getLocalBounds().width) / 2,
        restartButton.getPosition().y + 10
    );
    window.draw(restartText);

    // ===== Exit 按鈕繪製 =====
    exitButton.setSize(sf::Vector2f(200, 50));
    exitButton.setPosition(220, 380);
    sf::Color exitColor(220, 20, 60, 255 - static_cast<int>(exitHoverAlpha));
    exitButton.setFillColor(exitColor);
    window.draw(exitButton);

    sf::RectangleShape exitOutline(exitButton);
    exitOutline.setFillColor(sf::Color::Transparent);
    exitOutline.setOutlineThickness(2);
    exitOutline.setOutlineColor(sf::Color::White);
    window.draw(exitOutline);

    for (int i = 0; i < 4; ++i) {
        sf::CircleShape corner(10);
        corner.setFillColor(exitColor);
        float x = (i % 2 == 0) ? exitButton.getPosition().x : exitButton.getPosition().x + exitButton.getSize().x - 20;
        float y = (i < 2) ? exitButton.getPosition().y : exitButton.getPosition().y + exitButton.getSize().y - 20;
        corner.setPosition(x, y);
        window.draw(corner);
    }

    exitText.setFont(font);
    exitText.setCharacterSize(24);
    exitText.setString("Exit");
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(
        exitButton.getPosition().x + (200 - exitText.getLocalBounds().width) / 2,
        exitButton.getPosition().y + 10
    );
    window.draw(exitText);
}
