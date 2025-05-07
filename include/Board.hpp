#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <iostream>

class Board {
public:
    static const int SIZE = 15;
    Board();

    void print() const;
    bool placePiece(int row, int col, char symbol);
    bool isWin(int row, int col, char symbol);
    bool isFull() const;
    char getCell(int row, int col) const;

private:
    std::vector<std::vector<char>> grid;
};

#endif
