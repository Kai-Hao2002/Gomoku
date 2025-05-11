// #include "Game.hpp"
// #include "HumanPlayer.hpp"
// #include "AIPlayer.hpp"
// #include <iostream>
// #include "Board.hpp"
// #include "GameWindow.hpp"


// int main() {
//     std::cout << "Welcome to Gomoku!\n";
//     std::cout << "Choose mode:\n1. Player vs Player\n2. Player vs AI\n> ";
//     int choice;
//     std::cin >> choice;

//     Player* p1 = new HumanPlayer('X');
//     Player* p2 = (choice == 2) ? static_cast<Player*>(new AIPlayer('O'))
//                                : static_cast<Player*>(new HumanPlayer('O'));

//     Game game(p1, p2);
//     game.run();

//     return 0;
// }
