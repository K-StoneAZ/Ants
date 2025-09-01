#pragma once
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;
class Field
{
protected:
	int rows{10};
	int cols{10};
	struct Cells {
		int owner{ 0 };
		int ants{ 0 };
		int corner{ 0 };
		int edge{ 0 };
		int base{ 0 };
		int str{ 0 };
	};
	vector<vector<Cells>> field;
	public:
		Field(int r = 10, int c = 10) : rows(r), cols(c) {
			field.resize(rows, vector<Cells>(cols, { 0,0 }));
		}
		//getters
		int getRows() const {
			return rows;
		}
		int getCols() const {
			return cols;
		}
		int getOwner(int row, int col) const {
			if (row >= 0 && row < rows && col >= 0 && col< cols) {
				return field[row][col].owner;
			}
			return 0;
		}
		int getAnts(int row, int col) const {
			if (row >= 0 && row < rows && col >= 0 && col< cols) {
				return field[row][col].ants;
			}
			return 0;
		}
		int getCorner(int row, int col) const {
			if (row >= 0 && row < rows && col >= 0 && col< cols) {
				return field[row][col].corner;
			}
			return 0;
		}
		int getEdge(int row, int col) const {
			if (row >= 0 && row < rows && col >= 0 && col< cols) {
				return field[row][col].edge;
			}
			return 0;
		}
		int getBase(int row, int col) const {
			if (row >= 0 && row < rows && col >= 0 && col< cols) {
				return field[row][col].base;
			}
			return 0;
		}
		int getStr(int row, int col) const {
			if (row >= 0 && row < rows && col >= 0 && col< cols) {
				return field[row][col].str;
			}
			return 0;
		}
		//Setters
		void setOwner(int row, int col, int player) {
			field[row][col].owner = player;
		}
		void setAnts(int row, int col, int ant) {
			field[row][col].ants = ant;
		}
		void setStr(int row, int col, int count) {
			field[row][col].str = count;
		}
		void setCell(int row, int col, int player) {
			if (row >= 0 && row < rows && col >= 0 && col < cols) {
				field[row][col].owner = player;
				field[row][col].ants = (player == 0) ? 0 : 1;//set owner and ants
			}
			if (row == 0 && col == 0) {//top left corner and base str set
				field[row][col].base = 5;
				field[row][col].corner = 1;
			}
			else if (row == 0 && col > 0 && col < (cols - 1)) {//top edge and base str set
				field[row][col].base = 3;
				field[row][col].edge = 1;
			}
			else if (row == 0 && col == (cols - 1)) {// top right corner and base str set
				field[row][col].base = 5;
				field[row][col].corner = 2;
			}
			else if (row > 0 && row < (rows - 1) && col == 0) {//left edge and base str set
				field[row][col].base = 3;
				field[row][col].edge = 2;
			}
			else if (row > 0 && row < (rows - 1) && col == (cols - 1)) {//right edge, base str set
				field[row][col].base = 3;
				field[row][col].edge = 3;
			}
			else if (row == (rows - 1) && col == 0) {//bottom left corner, base str set
				field[row][col].base = 5;
				field[row][col].corner = 3;
			}
			else if (row == (rows - 1) && col > 0 && col < (cols - 1)) {//bottom edge and base str set
				field[row][col].base = 3;
				field[row][col].edge = 4;
			}
			else if (row == (rows - 1) && col == (cols - 1)) {//bottom right corner, base set
				field[row][col].base = 5;
				field[row][col].corner = 4;
			}
			else {
				field[row][col].base = 0;
				field[row][col].corner = 0;
				field[row][col].edge = 0;
			}
		}//end of set cell
		//setCellstr
		void setCellstr(int player) {
			int row = getRows();
			int col = getCols();
			int playernum = player;
			int count = 0;
			int rmin = 0, rmax = row - 1;
			int cmin = 0, cmax = col - 1;
			for (int p = 0; p <= player; p++) {
				for (int i = 0; i < row; i++) {
					for (int j = 0; j < col; j++) {
						if (field[i][j].owner == p) {
							count = getBase(i, j);
							rmin = max(0, i - 1);// min row limit
							rmax = min(row - 1, i + 1);// max row limit
							cmin = max(0, j - 1);// min col limit
							cmax = min(col - 1, j + 1);//max col limit
							for (int k = rmin; k <= rmax; k++) {
								for (int l = cmin; l <= cmax; l++) {
									if (k == i && l == j) { continue; }
									else if (field[k][l].owner == p) {
										count++;
									}
								}
							}
							setStr(i, j, count);
						}
					}
				}
			}
		}
		//printers
		void printField() const {
			int i = 0;
			int row = getRows();
			int col = getCols();
			while (i < row) {
				cout << "      ";
				for (int j = 0; j < col; j++) {
					cout << " " << i << ", ";
					cout << j << " |";
				}
				cout << "\nPlayer";
				for (int j = 0; j < col; j++) {
					cout << "   " << field[i][j].owner << "  |";
				}
				for (int j = 0; j < col; j++) {
					if (field[i][j].ants > 9) {//change the padding
						cout << " " << field[i][j].ants << "  |";
					}
					else {
						cout << "   " << field[i][j].ants << "  |";
					}
				}
			}
		}//end printField()
		void printPlayer(int player) const {
			if (player < 0 || player > 8) { return; }
			int row = getRows();
			int col = getCols();
			int i = 0;
			while (i < row) {
				cout << "      ";
				for (int j = 0; j < col; j++) {
					cout << " " << i << ", "<< j << " |";
				}
				cout << "\nPlayer";
				for (int j = 0; j < col; j++) {
					if (field[i][j].owner == player) {
						cout << "   " << field[i][j].owner << "  |";
					}
					else { cout << "     |"; }
				}
				for (int j = 0; j < col; j++) {
					if (field[i][j].owner == player) {
						if (field[i][j].ants > 9) {//change the padding
							cout << " " << field[i][j].ants << "  |";
						}
						else {
							cout << "   " << field[i][j].ants << "  |";
						}
					}
					else {
						cout << "      |";
					}
				}
			}

		}
		void printMinMax(int r, int c, int player) const {
			int row = getRows();
			int col = getCols();
			int playernum = player;
			int count = 0;
			int rmin = max(0, r - 1);// min row limit
			int rmax = min(row - 1, r + 1);// max row limit
			int cmin = max(0, c - 1);// min col limit
			int cmax = min(col - 1, c + 1);//max col limit}
			int i = rmin;
			while (i >= rmin && i <= rmax) {
				for (int j = cmin; j <= cmax; ++j) {
					std::cout << "      ";
					for (int j = cmin; j <= cmax; ++j) {
						std::cout << " " << i << ", ";
						std::cout << j << " |";
					}
					std::cout << "\nPlayer";
					for (int j = cmin; j <= cmax; ++j) {
						std::cout << "   " << field[i][j].owner;
						std::cout << "  |";
					}
					std::cout << "\nAnts  ";
					for (int j = cmin; j <= cmax; ++j) {
						if (field[i][j].ants > 9) {
							std::cout << "  " << field[i][j].ants; // If < 9
						}
						else {
							std::cout << "   " << field[i][j].ants;
						}
						std::cout << "  |";
					}
					std::cout << "\nStrnth";
					for (int j = cmin; j <= cmax; ++j) {
						std::cout << "   " << field[i][j].str;
						std::cout << "  |";
					}
					i++;
					if (i > rmax) {
						std::cout << "\n";
						break; // Exit if we exceed the maximum row limit
					}
					std::cout << "\n";
				}
			}
		}// end of printMinMax
		// cleanup for new game
		void destrotField() {
			field.clear();
		}
};

