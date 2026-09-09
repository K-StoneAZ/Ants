#pragma once
// Field.h  Win32 AntzV2 Game
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include "GameTypes.h"

extern std::mt19937 gRNG;

class Field
{
protected:
	int rows{ 10 };
	int cols{ 10 };
	int players{ 9 };
	int StartCells{ 15 };

public:
	enum VisualMode
	{
		V_NORMAL,
		V_HOVER,
		V_GROWTH,
		V_ATTACK_SOURCE,
		V_ATTACK_TARGET,
		V_MOVE_SOURCE,
		V_MOVE_TARGET
	};

	struct Cells {
		int owner{ 0 };
		int ants{ 0 };
		int corner{ 0 };
		int edge{ 0 };
		int base{ 0 };
		int str{ 0 };
		VisualMode visual{ V_NORMAL };	
	};

private:
	std::vector<std::vector<Cells>> field;

public:
	Field() {}

	void Initialize(const GameConfig& config)
	{
		rows = config.m_FieldSize;
		cols = config.m_FieldSize;
		players = config.m_ActivePlayers + 1;
		StartCells = config.m_StartCells;

		std::uniform_int_distribution<int> rowDist(0, rows - 1);
		std::uniform_int_distribution<int> colDist(0, cols - 1);
		field.clear();
		field.resize(rows, std::vector<Cells>(cols, { 0,0 }));
		int count = 0, p_count = 1, p_layer = 0;
		//Set initial random cells for each player based on StartCells
		while (count <= StartCells && p_count < players) {
			int r = rowDist(gRNG);
			int c = colDist(gRNG);
			if (getOwner(r, c) != 0) {
				continue; }//already owned
			else { 
				setOwner(r, c, p_count); 
				count++;
			}
			if (count == StartCells) { p_count++; count = 0; }
		}
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++)
			{
				p_layer = getOwner(i, j);
				setCell(i, j, p_layer);//Set base, corner, edge, and ants based on owner
			}
		}
		//Sets the str value based on the number of adjacent cells owned by the same player
		setCellstr(players);
	}

	//load data from save string
/*	void loaddata(const string& data) {
		istringstream ss(data);
		string cellInfo;
		int row, col, owner, ants, corner, edge, base, str;
		while (getline(ss, cellInfo, '|')) {
			istringstream cellStream(cellInfo);
			if (!(cellStream >> row >> col >> owner >> ants >> corner >> edge >> base >> str))
			{
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
	}*/
	//getters
	int getRows() const {
		return rows;
	}
	int getCols() const {
		return cols;
	}
	int GetPlayers() const {
		return players;
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
	const Cells& GetCell(int row, int col) const
	{
			return field[row][col];
	}
	int CountPlayerCells(int playerID) const
	{
		int count = 0;

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				if (getOwner(r, c) == playerID)
				{
					count++;
				}
			}
		}

		return count;
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
		int rmin = max(0, row - 1);
		int rmax = min(getRows() - 1, row + 1);
		int cmin = max(0, col - 1);
		int cmax = min(getCols() - 1, col + 1);
		return Area(rmin, rmax, cmin, cmax);
	}
/*	string getSaveString() const {
		string saveData{ "" };
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				saveData += to_string(i) + " " + to_string(j) + " ";
				saveData += to_string(field[i][j].owner) + " " + to_string(field[i][j].ants) + " ";
				saveData += to_string(field[i][j].corner) + " " + to_string(field[i][j].edge) + " ";
				saveData += to_string(field[i][j].base) + " " + to_string(field[i][j].str) +  "|";
			}

		}
		saveData += "\n";
		return saveData;
	}*/
	//Setters
	void SetVisualMode(int row, int col, VisualMode mode)
	{
		field[row][col].visual = mode;
	}
	void ClearVisualMode(int row, int col)
	{
		field[row][col].visual = V_NORMAL;
	}
	void ClearAllVisualModes()
	{
		for (auto& row : field)
		{
			for (auto& cell : row)
			{
				cell.visual = V_NORMAL;
			}
		}
	}
	void SetPlayerVisualMode(int playerID)
	{
		for (int row = 0; row < getRows(); row++)
		{
			for (int col = 0; col < getCols(); col++)
			{
				if (getOwner(row, col) == playerID)
				{
					field[row][col].visual = V_HOVER;
				}
			}
		}
	}
	void ClearPlayerVisualMode(int playerID)
	{
		for (int row = 0; row < getRows(); row++)
		{
			for (int col = 0; col < getCols(); col++)
			{
				if (getOwner(row, col) == playerID)
				{
					field[row][col].visual = V_NORMAL;
				}
			}
		}
	}
	void ClearVisualModeType(VisualMode mode)
	{
		for (auto& row : field)
		{
			for (auto& cell : row)
			{
				if (cell.visual == mode)
				{
					cell.visual = V_NORMAL;
				}
			}
		}
	}
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
		int count = 0;
		for (int p = 0; p <= player; p++) {
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					if (field[i][j].owner != p) { continue; }
					count = getBase(i, j);

					Area area = getArea(i, j);
						for (int k = area.rmin; k <= area.rmax; k++) {
							for (int l = area.cmin; l <= area.cmax; l++) {
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

};
