#pragma once
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;
class Field
{
protected:
	int rows{ 10 };
	int cols{ 10 };
	int players{ 9 };

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
	Field(int r = 10, int c = 10, int p = 9) : rows(r), cols(c), players(p) {
		field.resize(rows, vector<Cells>(cols, { 0,0 }));
		srand(static_cast<unsigned int>(time(0)));
		int count = 0, p_count = 1, p_layer = 0;
		while (count <= 18 && p_count < players) {
			int r = 0 + rand() % rows;
			int c = 0 + rand() % cols;
			if (r < 0 || r > rows - 1 || c < 0 || c > cols - 1) { continue; }//out of range
			else if (getOwner(r, c) != 0) { continue; }//already owned
			else { setOwner(r, c, p_count); count++; }
			if (count == 18) { p_count++; count = 0; }
		}
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				p_layer = getOwner(i, j);
				setCell(i, j, p_layer);
			}
		}
		setCellstr(players);
	}

	//load data from save string
	void loaddata(const string& data) {
		istringstream ss(data);
		string cellInfo;
		int row, col, owner, ants, corner, edge, base, str;
		while (getline(ss, cellInfo, '|')) {
			istringstream cellStream(cellInfo);
			if (!(cellStream >> row >> col >> owner >> ants >> corner >> edge >> base >> str)) {
				cout << "Error parsing cell data." << endl;
				return;
			}
			if (row < 0 || row >= rows || col < 0 || col >= cols) {
				cout << "Invalid cell coordinates in save data." << endl;
				return;
			}
			field[row][col].owner = owner;
			field[row][col].ants = ants;
			field[row][col].corner = corner;
			field[row][col].edge = edge;
			field[row][col].base = base;
			field[row][col].str = str;
		}
	}
	//getters
	int getRows() const {
		return rows;
	}
	int getCols() const {
		return cols;
	}
	int getOwner(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].owner;
		}
		return 0;
	}
	int getAnts(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].ants;
		}
		return 0;
	}
	int getCorner(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].corner;
		}
		return 0;
	}
	int getEdge(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].edge;
		}
		return 0;
	}
	int getBase(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].base;
		}
		return 0;
	}
	int getStr(int row, int col) const {
		if (row >= 0 && row < rows && col >= 0 && col < cols) {
			return field[row][col].str;
		}
		return 0;
	}
	struct Area {
		int rmin;
		int rmax;
		int cmin;
		int cmax;
		Area(int rmin = -1, int rmax = -1, int cmin = -1, int cmax = -1)
			: rmin(rmin), rmax(rmax), cmin(cmin), cmax(cmax) {
		}
	};

	Area getArea(int row, int col) const {
		int rmin = std::max(0, row - 1);
		int rmax = std::min(getRows() - 1, row + 1);
		int cmin = std::max(0, col - 1);
		int cmax = std::min(getCols() - 1, col + 1);
		return Area(rmin, rmax, cmin, cmax);
	}
	string getSaveString() const {
		string saveData{ "" };
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				saveData += to_string(i) + " " + to_string(j) + " ";
				saveData += to_string(field[i][j].owner) + " " + to_string(field[i][j].ants) + " ";
				saveData += to_string(field[i][j].corner) + " " + to_string(field[i][j].edge) + " ";
				saveData += to_string(field[i][j].base) + " " + to_string(field[i][j].str) + "|";
			}

		}
		saveData += "\n";
		return saveData;
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
						if (field[i][j].owner == 0) { count = 0; }
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
				if (i < 10 && j < 10) {cout << " " << i << ", " << j << " |"; }
				else if (i > 9 && j < 10) { cout << i << ", " << j << " |"; }
				else if (i < 10 && j>9) { cout << " " << i << ", " << j << "|"; }
				else { cout << i << ", " << j << "|"; }
			}
			cout << "\nPlayer";
			for (int j = 0; j < col; j++) {
				cout << "   " << field[i][j].owner << "  |";
			}cout << "\nAnts  ";
			for (int j = 0; j < col; j++) {
				if (field[i][j].ants > 9) {
					cout << "  " << field[i][j].ants << "  |";
				}
				else {
					cout << "   " << field[i][j].ants << "  |";
				}
			}cout << "\n";
			i++;
		}
	}//end printField()
	void printPlayer(int player) const {
		if (player < 0 || player > 8) { return; }
		int row = getRows();
		int col = getCols();
		int i = 0;
		while (i < row) {
			cout << "      ";
			for (int j = 0; j < col; j++) {//screen alignment
				if (i < 10 && j < 10) {cout << " " << i << ", " << j << " |"; }
				else if (i > 9 && j < 10) { cout << i << ", " << j << " |"; }
				else if (i < 10 && j>9) { cout << " " << i << ", " << j << "|"; }
				else { cout << i << ", " << j << "|"; }
			}
			cout << "\nPlayer";
			for (int j = 0; j < col; j++) {
				if (field[i][j].owner == player) {
					cout << "   " << field[i][j].owner << "  |";
				}
				else { cout << "      |"; }
			}
			cout << "\nAnts  ";
			for (int j = 0; j < col; j++) {
				if (field[i][j].owner == player) {
					if (field[i][j].ants > 9) {//change the padding
						cout << "  " << field[i][j].ants << "  |";
					}
					else {
						cout << "   " << field[i][j].ants << "  |";
					}
				}
				else {
					cout << "      |";
				}
			}
			cout << "\nStrgth";
			for (int j = 0; j < col; j++) {
				if (field[i][j].owner == player) {
					cout << "   " << field[i][j].str << "  |";
				}
				else { cout << "      |"; }
			}
			cout << "\n";
			i++;
		}
	}
	//end printPlayer
	void printMinMax(int r, int c, int player) const {

		Area area = getArea(r, c);
		int rmin = area.rmin, rmax = area.rmax, cmin = area.cmin, cmax = area.cmax;
		int i = rmin;
		while (i >= rmin && i <= rmax) {
			for (int j = cmin; j <= cmax; ++j) {
				std::cout << "      ";
				for (int j = cmin; j <= cmax; ++j) {//screen alignment
					if (i < 10 && j < 10) {	cout << " " << i << ", " << j << " |"; }
					else if (i > 9 && j < 10) { cout << i << ", " << j << " |"; }
					else if (i < 10 && j>9) { cout << " " << i << ", " << j << "|"; }
					else { cout << i << ", " << j << "|"; }
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
};
