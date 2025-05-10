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
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            cell.setPosition(j * CELL_SIZE + 1, i * CELL_SIZE + 1);
            cell.setFillColor(sf::Color(230, 200, 150));
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
                window.draw(piece);
            }
        }
    }
}

void GameWindow::displayResult(sf::RenderWindow& window, sf::Font& font) {
    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(36);
    resultText.setFillColor(sf::Color::Black);

    // 若棋盤滿了且沒有人獲勝，顯示平局
    if (board.isFull() && !board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        resultText.setString("It's a draw!");
    } 
    // 如果有玩家獲勝
    else if (board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        if (lastPlayerSymbol == 'X') {
            resultText.setString("Player 1 (X) wins!");
        } else {
            resultText.setString("Player 2 (O) wins!");
        }
    }

    // 設置顯示位置
    resultText.setPosition(640 / 2 - resultText.getGlobalBounds().width / 2, 640 / 2 - resultText.getGlobalBounds().height / 2);

    // 使用 sf::RectangleShape 設置按鈕
    // Restart 按鈕
        restartButton.setSize(sf::Vector2f(200, 50));
        restartButton.setFillColor(sf::Color::Green);
        restartButton.setPosition(640 / 2 - 100, 640 / 2 + 60);

        restartText.setFont(font);
        restartText.setCharacterSize(24);
        restartText.setString("Restart");
        restartText.setFillColor(sf::Color::White);
        restartText.setPosition(restartButton.getPosition().x + 50, restartButton.getPosition().y + 10);

        // Exit 按鈕
        exitButton.setSize(sf::Vector2f(200, 50));
        exitButton.setFillColor(sf::Color::Red);
        exitButton.setPosition(640 / 2 - 100, 640 / 2 + 120);

        exitText.setFont(font);
        exitText.setCharacterSize(24);
        exitText.setString("Exit");
        exitText.setFillColor(sf::Color::White);
        exitText.setPosition(exitButton.getPosition().x + 80, exitButton.getPosition().y + 10);

        // 畫出結果與按鈕
        window.draw(resultText);
        window.draw(restartButton);
        window.draw(restartText);
        window.draw(exitButton);
        window.draw(exitText);

}
