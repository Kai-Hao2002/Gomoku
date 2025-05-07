#include "Game.hpp"
#include <iostream>

Game::Game(Player* p1, Player* p2) {
    players[0] = std::unique_ptr<Player>(p1);
    players[1] = std::unique_ptr<Player>(p2);
}

void Game::run() {
    int current = 0;
    while (true) {
        board.print();
        int row, col;
        players[current]->makeMove(board, row, col);
        if (!board.placePiece(row, col, players[current]->getSymbol())) {
            std::cout << "Invalid move. Try again.\n";
            continue;
        }
        if (board.isWin(row, col, players[current]->getSymbol())) {
            board.print();
            std::cout << "Player " << players[current]->getSymbol() << " wins!\n";
            break;
        }
        if (board.isFull()) {
            board.print();
            std::cout << "The game is a draw!\n";
            break;
        }
        current = 1 - current;
    }
}
