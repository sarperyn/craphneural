#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>


#define UNASSIGNED 0

class Sudoku {
private:
  int grid[9][9];
  int solnGrid[9][9];
  int guessNum[9];
  int gridPos[81];
  int difficultyLevel;
  bool grid_status;
public:
  Sudoku ();
  Sudoku (std::string, bool row_major=true);
  void fillEmptyDiagonalBox(int);
  void createSeed();
  void printGrid();
  bool solveGrid();
  std::string getGrid();
  void countSoln(int &number);
  void genPuzzle();
  bool verifyGridStatus();
  void printSVG(std::string);
  void calculateDifficulty();
  int  branchDifficultyScore();
  void saveGrid(const std::string);
};

std::string construct_filename(const std::string& kind);
std::string format_filename(const std::string& prefix, int index);
