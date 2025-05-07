#include "Player.hpp"

Player::Player(char s) : symbol(s) {}

char Player::getSymbol() const {
    return symbol;
}
