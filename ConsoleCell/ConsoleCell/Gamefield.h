#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits> // For numeric_limits
using namespace std;
//Build a class called Gamefield.  contains an incrimented index member called Cell
//Cell contains owner 
//Cell contains ants which is 0 if owner is 0, otherwise it is 1
//The class should have a constructor that takes two parameters, rows and cols.
//rows and cols are protected members and default value is 10, 10
//create a function to set and get owner
//create a function to set and get ants
//create a function to print the gamefield
//create a function to destroy the gamefield
// create a function to reset the gamefield

class Gamefield {
	protected:
	int rows; // Number of rows in the game field
	int cols; // Number of columns in the game field
	struct Cell {
		int owner; // Owner of the cell (0-8 for players, 0 if unowned)
		int ants; // Number of ants in the cell (0 if unowned, otherwise 1)
		int base; // Base strength value of the cell (0-5)
		int strength; // Strength of the cell (0-8)
		int iscorner; // 0 = false, 1 = TopLeft, 2 = TopRight, 3 = BottomLeft, 4 = BottomRight
		int isedge; // 0 = false, 1 = Top, 2 = Bottom, 3 = Left, 4 = Right

	};
	vector<vector<Cell>> field; // 2D vector to represent the game field

public:
	Gamefield(int r = 20, int c = 20) : rows(r), cols(c) {
		field.resize(rows, vector<Cell>(cols, {0, 0})); // Initialize blank game field
	}
	int getRows() const {
		return rows; // Return the number of rows
	}
	int getCols() const {
		return cols; // Return the number of columns
	}
	void setCell(int row, int col, int owner) {// initial setup only
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			field[row][col].owner = owner;
			field[row][col].ants = (owner == 0) ? 0 : 1; // If owner 0, ants 0; else ants 1
		}
		if (row ==0 && col == 0) {
			field[row][col].base = 5;
			field[row][col].iscorner = 1;// TopLeft corner
		}
		else if (row == 0 && col == cols - 1) {
			field[row][col].base = 5;
			field[row][col].iscorner = 2; // TopRight corner
		}
		else if (row == rows - 1 && col == 0) {
			field[row][col].base = 5;
			field[row][col].iscorner = 3; // BottomLeft corner
		}
		else if (row == rows - 1 && col == cols - 1) {
			field[row][col].base = 5;
			field[row][col].iscorner = 4; // BottomRight corner
		}
		else {
			field[row][col].iscorner = 0; // Not a corner cell
		}
		if ((row == 0) && (col >= 1 && col < cols - 2)) {
			field[row][col].base = 3;
			field[row][col].isedge = 1; // Top edge
		}
		else if ((row == rows - 1) && (col > 1 && col < cols - 2)) {
			field[row][col].base = 3;
			field[row][col].isedge = 2; // Bottom edge
		}
		else if ((col == 0) && (row > 1 && row < rows - 2)) {
			field[row][col].base = 3;
			field[row][col].isedge = 3; // Left edge
		}
		else if ((col == cols - 1) && (row > 1 && row < rows - 2)) {
			field[row][col].base = 3;
			field[row][col].isedge = 4; // Right edge
		}
		else {
			field[row][col].base = 0; // Not an edge cell
			field[row][col].isedge = 0; // Not an edge cell
		}
	}//end of setCell
	int getOwner(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].owner;
		}
		return -1; // Invalid position
	}
	int setOwner(int row, int col, int owner) {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			field[row][col].owner = owner; // Set the owner of the cell
			return field[row][col].owner; // Return the new owner
		}
		return -1; // Invalid position
	}
	int getAnts(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].ants;
		}
		return -1; // Invalid position
	}
	int setAnts(int row, int col, int amount) {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {// Check if the position is valid
				field[row][col].ants = amount; // Update the number of ants to the cell
				return field[row][col].ants; // Return the new number of ants		
		}
		return -1; // Invalid position or unowned cell
	}
	int getStrength(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].strength; // Return the strength of the cell
		}
		return -1; // Invalid position
	}
	int setStrength(int row, int col, int strength) {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			field[row][col].strength = strength; // Set the strength of the cell
			return field[row][col].strength; // Return the new strength
		}
		return -1; // Invalid position
	}
	int getIsCorner(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].iscorner; // Return if the cell is a corner
		}
		return -1; // Invalid position
	}

	int getIsEdge(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].isedge; // Return if the cell is an edge
		}
		return -1; // Invalid position
	}
	void printGamefield() const {		//print gamefield
		int i = 0;
		while (i < getRows()) {//Print base gamefield.
			std::cout << "      ";
			for (int j = 0; j < getCols(); j++) {
				std::cout << " " << i << ", ";
				std::cout << j << " |";
			}
			std::cout << "\nPlayer";
			for (int j = 0; j < getCols(); j++) {
				std::cout << "   " << field[i][j].owner;
				std::cout << "  |";
			}
			std::cout << "\nAnts  ";
			for (int j = 0; j < getCols(); j++) {
				if (field[i][j].ants > 9) {
					std::cout << "  " << field[i][j].ants; // If < 9
				}
				else {
					std::cout << "   " << field[i][j].ants;
				}
				std::cout << "  |";
			}
			i++;
			std::cout << "\n";
		}
	}//end of printGamefield
	void printPlayerField(int player) const {	// print the game field for current player

		if (player < 0 || player > 8) { // Check if player number is valid
			cout << "Invalid player number." << endl;
			return; // Invalid player number
		}
		int row = getRows(); // Get the number of rows
		int col = getCols(); // Get the number of columns
		int i = 0;
		while (i < getRows()) {//Print base gamefield.
			std::cout << "      ";
			for (int j = 0; j < getCols(); j++) {
				std::cout << " " << i << ", ";
				std::cout << j << " |";
			}
			std::cout << "\nPlayer";
			for (int j = 0; j < getCols(); j++) {
				if (field[i][j].owner == player) { // If the cell is owned by the player
					std::cout << "   " << field[i][j].owner;
					std::cout << "  |";
				}
				else {
					std::cout << "   "; // If not owned by the player, print 0
					std::cout << "   |";
				}
			}
			std::cout << "\nAnts  ";
			for (int j = 0; j < getCols(); j++) {
				if (field[i][j].owner == player) { // If the cell is owned by the player
					if (field[i][j].ants > 9) {
						std::cout << "  " << field[i][j].ants; // If < 9
					}
					else {
						std::cout << "   " << field[i][j].ants;
					}
					std::cout << "  |";
				}
				else {
					std::cout << "   "; // If not owned by the player, print 0
					std::cout << "   |";
				}
			}
			std::cout << "\nStrgth";
			for (int j = 0; j < getCols(); j++) {
				if (field[i][j].owner == player) { // If the cell is owned by the player
					std::cout << "   " << field[i][j].strength;
					std::cout << "  |";
				}
				else {
					std::cout << "   "; // If not owned by the player, print 0
					std::cout << "   |";
				}
			}
			i++;
			std::cout << "\n";
		}
	}//end of printPlayerField
	void setCellAttributes(int numPlayer = 9) {
		int row = getRows(); // Get the number of rows
		int col = getCols(); // Get the number of columns
		// complete cell strength by owner association
		int playerNum = numPlayer; // Number of players
		int str = 0; // Strength counter
		int Rmin = 0, Rmax = row - 1; // Row limits
		int Cmin = 0, Cmax = col - 1; // Column limits
		for (int player = 1; player <= playerNum; ++player) {
			for (int i = 0; i < row; ++i) {
				for (int j = 0; j < col; ++j) {
					if (field[i][j].owner == player) { // If the cell is owned by the player
						str = field[i][j].base; // Get strength of the owned cell
						Rmin = max(0, i - 1); // Set minimum row limit
						Rmax = min(row - 1, i + 1); // Set maximum row limit
						Cmin = max(0, j - 1); // Set minimum column limit
						Cmax = min(col - 1, j + 1); // Set maximum column limit
						// Iterate through surrounding cells to calculate total strength
						for (int k = Rmin; k <= Rmax; ++k) {
							for (int l = Cmin; l <= Cmax; ++l) {
								if (k == i && l == j) { continue; }
								else if (field[k][l].owner == player) { // If the cell is owned
									str ++; // Add strength
								}
							}
						}
						if (str > 8) { // If strength exceeds 8, set to 8
							str = 8; // Cap strength at 8
						}
						setStrength(i,j, str); // Update strength of the owned cell
					}
				}
			}
		}
	}//end of setCellAttributes
	void printPlayerCellAttribs(int r, int c, int player) const {
		if (player < 0 || player > 8) {// max player 8
			cout << "Invalid player number." << endl;
			return; // Invalid player number
		}
		int row = getRows(); // Get the number of rows
		int col = getCols(); // Get the number of columns
		int Rmin = max(0, r - 1); // Set minimum row limit
		int Rmax = min(row - 1, r + 1); // Set maximum row limit)
		int Cmin = max(0, c - 1); // Set minimum column limit
		int Cmax = min(col - 1, c + 1); // Set maximum column limit

		cout << "Player " << player << " Cell Attributes:\n";
		//print adjacent cells
		int i = Rmin;// Start from the minimum row limit
		while (i >= Rmin && i <= Rmax) {
			for (int j = Cmin; j <= Cmax; ++j) {
				std::cout << "      ";
				for (int j = Cmin; j <= Cmax; ++j) {
					std::cout << " " << i << ", ";
					std::cout << j << " |";
				}
				std::cout << "\nPlayer";
				for (int j = Cmin; j <= Cmax; ++j) {
					std::cout << "   " << field[i][j].owner;
					std::cout << "  |";
				}
				std::cout << "\nAnts  ";
				for (int j = Cmin; j <= Cmax; ++j) {
					if (field[i][j].ants > 9) {
						std::cout << "  " << field[i][j].ants; // If < 9
					}
					else {
						std::cout << "   " << field[i][j].ants;
					}
					std::cout << "  |";
				}
				std::cout << "\nStrnth";
				for (int j = Cmin; j <= Cmax; ++j) {
					std::cout << "   " << field[i][j].strength;
					std::cout << "  |";
				}
				i++;
				if (i > Rmax) {
					std::cout << "\n";
					break; // Exit if we exceed the maximum row limit
				}
				std::cout << "\n";
			}
		}		
	}// end of printPlayerCellAttribs
	void ExecuteGrowth() {}

	void ExecuteAttack() {}

	void ExecuteMove() {}
	
	void resetGamefield() {
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				field[i][j] = {0, 0}; // Reset all cells to unowned state
			}
		}
	}
	void destroyGamefield() {
		field.clear(); // Clear the game field
		rows = 0; // Reset rows
		cols = 0; // Reset columns
	}
};







