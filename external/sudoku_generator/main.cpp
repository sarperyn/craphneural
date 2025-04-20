#include <iostream>
#include <fstream>
#include <string>

#include "sudokuGen.hpp"

using namespace std;

// START: The main function
int main(int argc, char const *argv[])
{
  // Initialising seed for random number generation
  srand(time(NULL));

  int num_puzzles = 1000000;
  if (argc > 1) {
      num_puzzles = std::atoi(argv[1]);
  }

  for (int i = 0; i < num_puzzles; ++i) {
    Sudoku* puzzle = new Sudoku();

    // Creating a seed for puzzle generation
    puzzle->createSeed();
    puzzle->saveGrid("solution");

    // Generating the puzzle
    puzzle->genPuzzle();
    puzzle->saveGrid("puzzle");

    delete puzzle;

    // Progress log
    if ((i + 1) % 1000 == 0) {
    std::cout << "Generated " << i + 1 << " puzzles..." << std::endl;
    }
  }
  std::cout << "Generated " << num_puzzles << " puzzles." << std::endl;
  // Final log
  std::cout << "Done generating " << num_puzzles << " puzzles." << std::endl;
  return 0;
}
// END: The main function