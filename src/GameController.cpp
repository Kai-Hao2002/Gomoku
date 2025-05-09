#include "GameController.hpp"

GameController::GameController(bool vsAI) : gameOver(false), winner('.') {
    p1 = new HumanPlayer('X');
    p2 = vsAI ? static_cast<Player*>(new AIPlayer('O')) : static_cast<Player*>(new HumanPlayer('O'));
    current = p1;
}

GameController::~GameController() {
    delete p1;
    delete p2;
}

const Board& GameController::getBoard() const {
    return board;
}

char GameController::getCurrentSymbol() const {
    return current->getSymbol();
}

bool GameController::makeMove(int row, int col) {
    if (gameOver || board.getCell(row, col) != '.') return false;

    if (!board.placePiece(row, col, current->getSymbol())) return false;

    if (board.isWin(row, col, current->getSymbol())) {
        gameOver = true;
        winner = current->getSymbol();
        return true;
    }

    if (board.isFull()) {
        gameOver = true;
        winner = '.';
        return true;
    }

    switchTurn();
    return true;
}

bool GameController::isGameOver() const {
    return gameOver;
}

char GameController::getWinnerSymbol() const {
    return winner;
}

void GameController::switchTurn() {
    current = (current == p1) ? p2 : p1;

    // AI 自動下棋
    if (!gameOver && dynamic_cast<AIPlayer*>(current)) {
        int row, col;
        current->makeMove(board, row, col);
        board.placePiece(row, col, current->getSymbol());

        if (board.isWin(row, col, current->getSymbol())) {
            gameOver = true;
            winner = current->getSymbol();
        } else if (board.isFull()) {
            gameOver = true;
            winner = '.';
        } else {
            current = (current == p1) ? p2 : p1;
        }
    }
}
