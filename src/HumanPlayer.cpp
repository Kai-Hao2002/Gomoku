#include "HumanPlayer.hpp"

HumanPlayer::HumanPlayer(char symbol) : Player(symbol) {}

void HumanPlayer::makeMove(Board& board, int& row, int& col) {
    std::cout << "Player " << symbol << ", enter your move (row col): ";
    std::cin >> row >> col;
}
