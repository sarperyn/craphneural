#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include "sudokuGen.hpp"
using namespace std;

std::random_device rd;
std::mt19937 g(rd());

// START: Get grid as string in row major order
string Sudoku::getGrid()
{
  string s = "";
  for(int row_num=0; row_num<9; ++row_num)
  {
    for(int col_num=0; col_num<9; ++col_num)
    {
      s = s + to_string(grid[row_num][col_num]);
    }
  }

  return s;
}
// END: Get grid as string in row major order


// START: Generate random number
int genRandNum(int maxLimit)
{
  return rand()%maxLimit;
}
// END: Generate random number


// START: Helper functions for solving grid
bool FindUnassignedLocation(int grid[9][9], int &row, int &col)
{
    for (row = 0; row < 9; row++)
    {
        for (col = 0; col < 9; col++)
        {
            if (grid[row][col] == UNASSIGNED)
                return true;
        }
    }

    return false;
}

bool UsedInRow(int grid[9][9], int row, int num)
{
    for (int col = 0; col < 9; col++)
    {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool UsedInCol(int grid[9][9], int col, int num)
{
    for (int row = 0; row < 9; row++)
    {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool UsedInBox(int grid[9][9], int boxStartRow, int boxStartCol, int num)
{
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if (grid[row+boxStartRow][col+boxStartCol] == num)
                return true;
        }
    }

    return false;
}

bool isSafe(int grid[9][9], int row, int col, int num)
{
    return !UsedInRow(grid, row, num) && !UsedInCol(grid, col, num) && !UsedInBox(grid, row - row%3 , col - col%3, num);
}
// END: Helper functions for solving grid


// START: Create seed grid
void Sudoku::fillEmptyDiagonalBox(int idx)
{
  int start = idx*3;
  std::shuffle(this->guessNum, this->guessNum + 9, g);

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      this->grid[start+i][start+j] = guessNum[i*3+j];
    }
  }
}

void Sudoku::createSeed()
{
  /* Fill diagonal boxes to form:
      x | . | .
      . | x | .
      . | . | x
  */
  this->fillEmptyDiagonalBox(0);
  this->fillEmptyDiagonalBox(1);
  this->fillEmptyDiagonalBox(2);

  /* Fill the remaining blocks:
      x | x | x
      x | x | x
      x | x | x
  */
  this->solveGrid(); // TODO: not truly random, but still good enough because we generate random diagonals.

  // Saving the solution grid
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      this->solnGrid[i][j] = this->grid[i][j];
    }
  }
  // cout<<"The solution grid is:\n";
  // this->printGrid();
  // cout<<"The seed grid is:\n";
  // this->printGrid();
  // cout<<"The seed grid is:\n";
  // Saving the grid as a string
  // string grid_str = "";
  // for(int i=0;i<9;i++)
  // {
  //   for(int j=0;j<9;j++)
  //   {
  //     grid_str = grid_str + to_string(this->grid[i][j]);
  //   }
  // }
  // // cout<<grid_str<<endl;
  // // Save the grid as a string
  // ofstream outFile("seed.txt");
  // outFile << grid_str;
  // outFile.close();
}
// END: Create seed grid


// START: Intialising
Sudoku::Sudoku()
{

  // initialize difficulty level
  this->difficultyLevel = 0;

  // Randomly shuffling the array of removing grid positions
  for(int i=0;i<81;i++)
  {
    this->gridPos[i] = i;
  }

  std::shuffle(this->gridPos, this->gridPos + 81, g);

  // Randomly shuffling the guessing number array
  for(int i=0;i<9;i++)
  {
    this->guessNum[i]=i+1;
  }

  std::shuffle(this->guessNum, this->guessNum + 9, g);


  // Initialising the grid
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      this->grid[i][j]=0;
    }
  }

  grid_status = true;
}
// END: Initialising


// START: Custom Initialising with grid passed as argument
Sudoku::Sudoku(string grid_str, bool row_major)
{
  if(grid_str.length() != 81)
  {
    grid_status=false;
    return;
  }

  // First pass: Check if all cells are valid
  for(int i=0; i<81; ++i)
  {
    int curr_num = grid_str[i]-'0';
    if(!((curr_num == UNASSIGNED) || (curr_num > 0 && curr_num < 10)))
    {
      grid_status=false;
      return;
    }

    if(row_major) grid[i/9][i%9] = curr_num;
    else          grid[i%9][i/9] = curr_num;
  }

  // Second pass: Check if all columns are valid
  for (int col_num=0; col_num<9; ++col_num)
  {
    bool nums[10]={false};
    for (int row_num=0; row_num<9; ++row_num)
    {
      int curr_num = grid[row_num][col_num];
      if(curr_num!=UNASSIGNED && nums[curr_num]==true)
      {
        grid_status=false;
        return;
      }
      nums[curr_num] = true;
    }
  }

  // Third pass: Check if all rows are valid
  for (int row_num=0; row_num<9; ++row_num)
  {
    bool nums[10]={false};
    for (int col_num=0; col_num<9; ++col_num)
    {
      int curr_num = grid[row_num][col_num];
      if(curr_num!=UNASSIGNED && nums[curr_num]==true)
      {
        grid_status=false;
        return;
      }
      nums[curr_num] = true;
    }
  }

  // Fourth pass: Check if all blocks are valid
  for (int block_num=0; block_num<9; ++block_num)
  {
    bool nums[10]={false};
    for (int cell_num=0; cell_num<9; ++cell_num)
    {
      int curr_num = grid[((int)(block_num/3))*3 + (cell_num/3)][((int)(block_num%3))*3 + (cell_num%3)];
      if(curr_num!=UNASSIGNED && nums[curr_num]==true)
      {
        grid_status=false;
        return;
      }
      nums[curr_num] = true;
    }
  }

  // Randomly shuffling the guessing number array
  for(int i=0;i<9;i++)
  {
    this->guessNum[i]=i+1;
  }

  std::shuffle(this->guessNum, this->guessNum + 9, g);


  grid_status = true;
}
// END: Custom Initialising


// START: Verification status of the custom grid passed
bool Sudoku::verifyGridStatus()
{
  return grid_status;
}
// END: Verification of the custom grid passed


// START: Printing the grid
void Sudoku::printGrid()
{
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      if(grid[i][j] == 0)
	cout<<".";
      else
	cout<<grid[i][j];
      cout<<"|";
    }
    cout<<endl;
  }

  cout<<"\nDifficulty of current sudoku(0 being easiest): "<<this->difficultyLevel;
  cout<<endl;
}
// END: Printing the grid


// START: Modified Sudoku solver
bool Sudoku::solveGrid()
{
    int row, col;

    // If there is no unassigned location, we are done
    if (!FindUnassignedLocation(this->grid, row, col))
       return true; // success!

    // Consider digits 1 to 9
    for (int num = 0; num < 9; num++)
    {
        // if looks promising
        if (isSafe(this->grid, row, col, this->guessNum[num]))
        {
            // make tentative assignment
            this->grid[row][col] = this->guessNum[num];

            // return, if success, yay!
            if (solveGrid())
                return true;

            // failure, unmake & try again
            this->grid[row][col] = UNASSIGNED;
        }
    }

    return false; // this triggers backtracking

}
// END: Modified Sudoku Solver


// START: Check if the grid is uniquely solvable
void Sudoku::countSoln(int &number)
{
  int row, col;

  if(!FindUnassignedLocation(this->grid, row, col))
  {
    number++;
    return ;
  }


  for(int i=0;i<9 && number<2;i++)
  {
      if( isSafe(this->grid, row, col, this->guessNum[i]) )
      {
        this->grid[row][col] = this->guessNum[i];
        countSoln(number);
      }

      this->grid[row][col] = UNASSIGNED;
  }

}
// END: Check if the grid is uniquely solvable


// START: Gneerate puzzle
void Sudoku::genPuzzle()
{
  for(int i=0;i<81;i++)
  {
    int x = (this->gridPos[i])/9;
    int y = (this->gridPos[i])%9;
    int temp = this->grid[x][y];
    this->grid[x][y] = UNASSIGNED;

    // If now more than 1 solution , replace the removed cell back.
    int check=0;
    countSoln(check);
    if(check!=1)
    {
      this->grid[x][y] = temp;
    }
  }

  // Save the grid as a string
  string grid_str = "";
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      grid_str = grid_str + to_string(this->grid[i][j]);
    }
  }
  // cout<<grid_str<<endl;
  // Save the grid as a string
  ofstream outFile("puzzle.txt");
  outFile << grid_str;
  outFile.close();
  // Save the grid as a string
  // cout<<grid_str<<endl;
}
// END: Generate puzzle


// START: Printing into SVG file
void Sudoku::printSVG(string path="")
{
  string fileName = path + "svgHead.txt";
  ifstream file1(fileName.c_str());
  stringstream svgHead;
  svgHead << file1.rdbuf();

  ofstream outFile("puzzle.svg");
  outFile << svgHead.rdbuf();

  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      if(this->grid[i][j]!=0)
      {
        int x = 50*j + 16;
        int y = 50*i + 35;

        stringstream text;
        text<<"<text x=\""<<x<<"\" y=\""<<y<<"\" style=\"font-weight:bold\" font-size=\"30px\">"<<this->grid[i][j]<<"</text>\n";

        outFile << text.rdbuf();
      }
    }
  }

    outFile << "<text x=\"50\" y=\"500\" style=\"font-weight:bold\" font-size=\"15px\">Difficulty Level (0 being easiest): "                  <<this->difficultyLevel<<"</text>\n";
    outFile << "</svg>";

}
// END: Printing into SVG file


// START: Calculate branch difficulty score
int Sudoku::branchDifficultyScore()
{
   int emptyPositions = -1;
   int tempGrid[9][9];
   int sum=0;

   for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      tempGrid[i][j] = this->grid[i][j];
    }
  }

   while(emptyPositions!=0)
   {
     vector<vector<int> > empty; 

     for(int i=0;i<81;i++)
     {
        if(tempGrid[(int)(i/9)][(int)(i%9)] == 0)
        {
       	  vector<int> temp;
	  temp.push_back(i);
	
	  for(int num=1;num<=9;num++)
	  {
	    if(isSafe(tempGrid,i/9,i%9,num))
	    {
	      temp.push_back(num);
	    }
	  }

	  empty.push_back(temp);
        }
      
     }

     if(empty.size() == 0)
     { 
       cout<<"Hello: "<<sum<<endl;
       return sum;
     } 

     int minIndex = 0;

     int check = empty.size();
     for(int i=0;i<check;i++)
     {
       if(empty[i].size() < empty[minIndex].size())
	  minIndex = i;
     }

     int branchFactor=empty[minIndex].size();
     int rowIndex = empty[minIndex][0]/9;
     int colIndex = empty[minIndex][0]%9;

     tempGrid[rowIndex][colIndex] = this->solnGrid[rowIndex][colIndex];
     sum = sum + ((branchFactor-2) * (branchFactor-2)) ;

     emptyPositions = empty.size() - 1;
   }

   return sum;

}
// END: Finish branch difficulty score


// START: Calculate difficulty level of current grid
void Sudoku::calculateDifficulty()
{
  int B = branchDifficultyScore();
  int emptyCells = 0;

  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
	if(this->grid[i][j] == 0)
	   emptyCells++;
    }
  } 

  this->difficultyLevel = B*100 + emptyCells;
}
// END: calculating difficulty level

void Sudoku::saveGrid(const string kind)
{

  std::string path = construct_filename(kind);
  ofstream outFile(path.c_str());
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      outFile << this->grid[i][j];
    }
  }
  outFile.close();
  cout<<"The puzzle stored in "<<path<<endl;
}

std::string construct_filename(const std::string& kind){

  std::string folder_path;
  if (kind == "puzzle"){
    folder_path = "../../data/puzzles/";
  } else if (kind == "solution") {
    folder_path = "../../data/solutions";
  } else {
    std::cerr << "You can only use 'puzzle' or 'solution' to generate the data." << std::endl;
    return "";
  }

  int file_count = 0;
  for(const auto& entry: std::filesystem::directory_iterator(folder_path)){
    if(entry.is_regular_file()){
      file_count++;
    }
  }
  std::cout << "Number of files in the folder: " << file_count << std::endl;

  // Construct the file name
  // std::string filename = kind + "_";
  // if (kind == "puzzle") while(filename.length() < 15) filename += "0";
  // else if (kind == "solution") while(filename.length() < 13) filename += "0";
  // filename += std::to_string(file_count);
  // filename += ".txt";
  std::string filename = format_filename(kind, file_count);

  std::filesystem::path full_path = std::filesystem::path(folder_path) / filename;
  std::cout << "Full path of the file: " << full_path << std::endl;

  return full_path.string();
}

std::string format_filename(const std::string& prefix, int index) {
  std::ostringstream ss;
  ss << prefix << "_" << std::setw(6) << std::setfill('0') << index << ".txt";
  return ss.str();
}
