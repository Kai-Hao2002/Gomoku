#include "GameWindow.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include <iostream>
#include <cmath>  // 引入 <cmath> 库以使用 sin 函数

GameWindow::GameWindow(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2, bool isPvP)
    : p1(std::move(p1)), p2(std::move(p2)), isPvP(isPvP), gameOver(false) {
    currentPlayer = this->p1.get();
    lastPlayerSymbol = ' ';  // 初始化最後下棋的玩家符號
    lastMoveRow = -1;        // 初始化最後下棋的行
    lastMoveCol = -1;        // 初始化最後下棋的列
}

GameResult GameWindow::run(sf::RenderWindow& window, sf::Font& font) {
    wantToRestart = false;
    wantToExit = false;
    wantToModeSelection = false;
    gameOver = false;
    currentPlayer = p1.get();
    board = Board();  // 重置棋盤
    justRestarted = true;

    // 回合文字顯示
    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(18);
    turnText.setFillColor(sf::Color::Black);
    turnText.setPosition(10, 630);

    while (window.isOpen() && !wantToRestart && !wantToExit && !wantToModeSelection) {
        handleEvents(window);
        update();

        window.clear(sf::Color::White);
        draw(window, font);

        // 更新回合文字
        if (gameOver) {
            turnText.setString("");
        } else {
            if (currentPlayer->getSymbol() == 'X') {
                turnText.setString("Player 1's Turn (Black)");
            } else if (currentPlayer->getSymbol() == 'O') {
                turnText.setString("Player 2's Turn (White)");
            }
        }

        window.draw(turnText);

        if (gameOver) {
            displayResult(window, font);
        }

        window.display();
    }

    if (wantToModeSelection) return GameResult::ReturnToMenu;
    if (wantToRestart) return GameResult::Restart;
    return GameResult::Exit;
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
                    wantToRestart = true;
                    return;
                }

                // 檢查 Exit 按鈕是否被點擊
                if (exitButton.getGlobalBounds().contains(worldPos)) {
                    wantToExit = true;
                    window.close();
                    return;
                }
                // 檢查 GameMode 按鈕是否被點擊
                if (gameModeButton.getGlobalBounds().contains(worldPos)) {
                    wantToModeSelection = true;
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


void GameWindow::draw(sf::RenderWindow& window, sf::Font& font) {
    window.clear(sf::Color(255, 248, 220)); // Cornsilk 背景

    // ✅ 顯示玩家資訊欄（下方）
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::Black);
    infoText.setString("Player 1: Black (X)    Player 2: White (O)");
    infoText.setPosition(10, 600);  // 棋盤下方


    window.draw(infoText);


    // 🧱 繪製棋盤格與棋子（維持原本位置）
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            cell.setPosition(j * CELL_SIZE + 1, i * CELL_SIZE + 1);
            cell.setFillColor(sf::Color(245, 222, 179));
            cell.setOutlineThickness(1);
            cell.setOutlineColor(sf::Color(160, 82, 45));
            window.draw(cell);

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
    bool hoveringBackToMode = gameModeButton.getGlobalBounds().contains(mousePos);

    // 更新 hover alpha 值（漸變動畫）
    restartHoverAlpha = hoveringRestart ? std::min(255.f, restartHoverAlpha + 10.f) : std::max(0.f, restartHoverAlpha - 10.f);
    exitHoverAlpha = hoveringExit ? std::min(255.f, exitHoverAlpha + 10.f) : std::max(0.f, exitHoverAlpha - 10.f);
    gameModeHoverAlpha = hoveringBackToMode ? std::min(255.f, gameModeHoverAlpha + 10.f) : std::max(0.f, gameModeHoverAlpha - 10.f);
    // 結果文字
    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(48);  // 放大文字
    resultText.setFillColor(sf::Color::Yellow);  // 高亮顯示結果
    resultText.setOutlineColor(sf::Color::Black);  // 加黑邊讓文字清楚
    resultText.setOutlineThickness(2);
    float animScale = 1.f + 0.2f * sin(clock.getElapsedTime().asSeconds() * 2.f);  // 動畫效果：逐漸放大

    // 設定文字內容
    if (board.isFull() && !board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        resultText.setString("It's a draw!");
    } else if (board.isWin(lastMoveRow, lastMoveCol, lastPlayerSymbol)) {
        resultText.setString(lastPlayerSymbol == 'X' ? "Player 1 (X) wins!" : "Player 2 (O) wins!");
    }

    // 動畫縮放
    resultText.setScale(animScale, animScale);

    // 計算文字位置
    sf::FloatRect textBounds = resultText.getLocalBounds();
    float textX = 640.f / 2.f - textBounds.width / 2.f;
    float textY = 700.f / 2.f - textBounds.height / 2.f - 120.f;
    resultText.setPosition(textX, textY);

    // 計算背景框的大小，根據動畫縮放後的文字大小調整
    sf::RectangleShape resultBox;
    resultBox.setSize(sf::Vector2f(textBounds.width * animScale + 40.f, textBounds.height * animScale + 30.f));
    resultBox.setPosition(textX - 20.f * animScale, textY - 15.f * animScale);
    resultBox.setFillColor(sf::Color(50, 50, 50, 200));  // 半透明深灰背景
    resultBox.setOutlineColor(sf::Color::Yellow);        // 黃色邊框
    resultBox.setOutlineThickness(3.f);

    // 畫出背景框與文字
    window.draw(resultBox);
    window.draw(resultText);

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
    // 資訊欄背景
    sf::RectangleShape infoBar(sf::Vector2f(window.getSize().x, 40.f));
    infoBar.setFillColor(sf::Color(200, 200, 200));
    infoBar.setPosition(0, 0);
    window.draw(infoBar);

    // 顯示玩家資訊欄
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::Black);
    infoText.setString("Player 1: Black (X)    Player 2: White (O)");
    infoText.setPosition(10, 10); // 上方 10px 的位置
    window.draw(infoText);

    // 顯示玩家回合資訊
    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(18);
    turnText.setFillColor(currentPlayer->getSymbol() == 'X' ? sf::Color::Black : sf::Color::White);
    turnText.setString(currentPlayer->getSymbol() == 'X' ? "Player 1's Turn (Black)" : "Player 2's Turn (White)");
    turnText.setPosition(10, 630);  // 設置顯示位置
    window.draw(turnText);  // 顯示回合文字

     // ===== Game Mode 按鈕繪製 =====
    gameModeButton.setSize(sf::Vector2f(200, 50));
    gameModeButton.setPosition(220, 460);
    sf::Color gameModeColor(255, 165, 0, 255 - static_cast<int>(gameModeHoverAlpha));
    gameModeButton.setFillColor(gameModeColor);
    window.draw(gameModeButton);

    sf::RectangleShape gameModeOutline(gameModeButton);
    gameModeOutline.setFillColor(sf::Color::Transparent);
    gameModeOutline.setOutlineThickness(2);
    gameModeOutline.setOutlineColor(sf::Color::White);
    window.draw(gameModeOutline);

    for (int i = 0; i < 4; ++i) {
        sf::CircleShape corner(10);
        corner.setFillColor(gameModeColor);
        float x = (i % 2 == 0) ? gameModeButton.getPosition().x : gameModeButton.getPosition().x + gameModeButton.getSize().x - 20;
        float y = (i < 2) ? gameModeButton.getPosition().y : gameModeButton.getPosition().y + gameModeButton.getSize().y - 20;
        corner.setPosition(x, y);
        window.draw(corner);
    }

    gameModeText.setFont(font);
    gameModeText.setCharacterSize(24);
    gameModeText.setString("Back to Menu");
    gameModeText.setFillColor(sf::Color::White);
    gameModeText.setPosition(
        gameModeButton.getPosition().x + (200 - gameModeText.getLocalBounds().width) / 2,
        gameModeButton.getPosition().y + 10
    );
    window.draw(gameModeText);
    
}


bool GameWindow::showModeSelection(sf::RenderWindow& window, sf::Font& font, bool& isPvP) {
    sf::Text title("Select Game Mode", font, 36);
    title.setFillColor(sf::Color::Black);
    title.setPosition(145, 60);

    struct Button {
        sf::RectangleShape shape;
        sf::Text text;
        float hoverAlpha = 0.0f;
    };

    std::vector<Button> buttons(3);

    const std::vector<std::string> labels = {
        "Player vs Player", "Player vs Computer", "Exit Game"
    };
    const float startY = 180;
    const float spacing = 80;

    for (int i = 0; i < 3; ++i) {
        auto& btn = buttons[i];

        btn.shape.setSize(sf::Vector2f(300, 50));
        btn.shape.setPosition(150, startY + i * spacing);
        btn.shape.setFillColor(sf::Color(255, 165, 0));  // 預設色

        btn.text.setFont(font);
        btn.text.setCharacterSize(24);
        btn.text.setString(labels[i]);
        btn.text.setFillColor(sf::Color::White);
        btn.text.setPosition(
            btn.shape.getPosition().x + (300 - btn.text.getLocalBounds().width) / 2,
            btn.shape.getPosition().y + 10
        );
    }

    while (window.isOpen()) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                for (int i = 0; i < 3; ++i) {
                    if (buttons[i].shape.getGlobalBounds().contains(mousePos)) {
                        if (i == 0) { isPvP = true; return true; }
                        if (i == 1) { isPvP = false; return true; }
                        if (i == 2) { window.close(); return false; }
                    }
                }
            }
        }

        // 更新 hover 效果
        for (auto& btn : buttons) {
            if (btn.shape.getGlobalBounds().contains(mousePos)) {
                btn.hoverAlpha = std::min(100.0f, btn.hoverAlpha + 10.f);  // 控制變暗程度
            } else {
                btn.hoverAlpha = std::max(0.0f, btn.hoverAlpha - 10.f);
            }
        }

        window.clear(sf::Color(255, 255, 240));
        window.draw(title);

        for (auto& btn : buttons) {
            // 填色與 hover效果（變暗處理）
            sf::Color btnColor(255 - static_cast<int>(btn.hoverAlpha), 165 - static_cast<int>(btn.hoverAlpha), 0);  // Darken the color
            btn.shape.setFillColor(btnColor);
            window.draw(btn.shape);

            // 邊框
            sf::RectangleShape outline(btn.shape);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineThickness(2);
            outline.setOutlineColor(sf::Color::White);
            window.draw(outline);

            // 四個圓角圓點
            for (int i = 0; i < 4; ++i) {
                sf::CircleShape corner(10);
                corner.setFillColor(btnColor);
                float x = (i % 2 == 0) ? btn.shape.getPosition().x : btn.shape.getPosition().x + btn.shape.getSize().x - 20;
                float y = (i < 2) ? btn.shape.getPosition().y : btn.shape.getPosition().y + btn.shape.getSize().y - 20;
                corner.setPosition(x, y);
                window.draw(corner);
            }

            window.draw(btn.text);
        }

        window.display();
    }

    return false;
}
