#include "Game.hpp"
#include <iostream>

Game::Game(Player* p1, Player* p2) {
    players[0] = std::unique_ptr<Player>(p1);
    players[1] = std::unique_ptr<Player>(p2);
}

void Game::run(sf::RenderWindow& window) {
    int current = 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 每回合處理玩家的移動
        int row, col;
        players[current]->makeMove(board, row, col);

        // 檢查移動是否合法
        if (!board.placePiece(row, col, players[current]->getSymbol())) {
            std::cout << "Invalid move. Try again.\n";
            continue;
        }

        // 檢查是否有玩家勝利
        if (board.isWin(row, col, players[current]->getSymbol())) {
            window.clear();
            draw(window);
            window.display();
            std::cout << "Player " << players[current]->getSymbol() << " wins!\n";
            break;
        }

        // 檢查是否平手
        if (board.isFull()) {
            window.clear();
            draw(window);
            window.display();
            std::cout << "The game is a draw!\n";
            break;
        }

        // 切換玩家
        current = 1 - current;

        // 清空視窗並繪製遊戲畫面
        window.clear(sf::Color::White);
        draw(window);
        window.display();
    }
}

void Game::draw(sf::RenderWindow& window) {
    // 定義每個格子的大小
    sf::RectangleShape cell(sf::Vector2f(40.f, 40.f));

    // 繪製棋盤格子
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            cell.setPosition(i * 40.f, j * 40.f);
            if ((i + j) % 2 == 0) {
                cell.setFillColor(sf::Color(240, 240, 240)); // 灰色格子
            } else {
                cell.setFillColor(sf::Color(200, 200, 200)); // 白色格子
            }
            window.draw(cell);
        }
    }

    // 根據棋盤狀態繪製棋子
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            char cell = board.getCell(i, j);
            if (cell == 'X' || cell == 'O') {
                sf::CircleShape piece(18.f);  // 圓形棋子，半徑18
                piece.setPosition(i * 40.f + 10.f, j * 40.f + 10.f);  // 設置位置
                piece.setFillColor(cell == 'X' ? sf::Color::Black : sf::Color::White);  // 根據棋子顏色選擇填充顏色
                window.draw(piece);  // 繪製棋子
            }
        }
    }
}
