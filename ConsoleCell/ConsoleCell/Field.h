#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

class Field //develope Ants playing field
{
protected:
	int rows;
	int cols;
	struct Cell {
		int owner;
		int ants;
		Cell() : owner(0), ants(0) {}
	};
	vector<vector<Cell>> field;
public:
	Field(int r = 10, int c = 10) : rows(r), cols(c) {
		field.resize(rows, vector<Cell>(cols));
		srand(static_cast<unsigned int>(time(0))); // Seed for random number generation
	}
	void setCell(int row, int col, int owner) {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			field[row][col].owner = owner;
			field[row][col].ants = (owner == 0) ? 0 : 1; // If owner is 0, ants are 0; otherwise, ants are 1
		}
	}
	pair<int, int> getCell(int row, int col) {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return make_pair(field[row][col].owner, field[row][col].ants);
		}
		return make_pair(-1, -1); // Invalid cell
	}
	void printGamefield() {
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				cout << "|  " << field[i][j].owner << ", " << field[i][j].ants << "  | ";
			}
			cout << endl;
		}
	}
	void destroyGamefield() {
		field.clear();
		rows = 0;
		cols = 0;
	}
	void resetGamefield() {
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				field[i][j].owner = 0;
				field[i][j].ants = 0;
			}
		}
	}
	friend class ConsoleCell; // Make ConsoleCell a friend class to access private members
};

