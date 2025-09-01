#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include "Field.h"
#include "Param.h"

using namespace std;

class Work
{
public:
	void growthPara(int player, Field& game, Param& para) {
		int growth = para.getSant();
		int tempant = 0;
		int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validTarget = false;
		game.printPlayer(player);
		while (!validTarget) {
			cout << "Select cell to distribute the ants (row col):";
			cin >> r >> c;
			if (r <= 0 || r > rows || c <= 0 || c > cols) {
				cout << "Cell is outside of game field!";
				continue;
			}
			if (game.getCorner(r, c) > 0) {
				cout << "This is a corner cell.\n";
			}
			if (game.getEdge(r, c) > 0) {
				cout << "This is a edge cell.\n";
			}
			if (game.getStr(r, c) == 8) {
				cout << "This cell is surrounded by cells you own.\n";
			}
			if (game.getOwner(r, c) != player) {
				cout << "You must own the cell.\n";
				continue;
			}
			else {
				char confirm = ' ';
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  (Y)es ";
				cin >> confirm; confirm = toupper(confirm);
				if (confirm != 'Y') {
					para.setTarget(r, c);
					validTarget = true;
				}
				else { validTarget = false; }
			}
		}
		bool validant = false;
		while (!validant) {
			cout << "Player " << player << " you have " << growth << " ants to deploy.\n";
			cout << "Enter the number of anst to place in this cell: ";
			cin >> amount;
			if (amount < 1) {
				cout << "You must place at least 1 ant.";
				amount = 0;
			}
			else if (amount > growth) {
				cout << "You don't have that many ants to deploy! ";
				amount = 0;
			}
			else {
				para.setTant(amount);
				validant = true;
				cout << "\n";
			}
		}
	}
	int setGrowth(int player, Field& game, Param& para) const {
		int growth = para.getSant();
		int tempant = 0;
		int amount = para.getTant();
		bool endGrowth = false;
		tempant = game.getAnts(para.getTrow(), para.getTcol());// current ants in cell
		tempant += amount;//add amount to current
		game.setAnts(para.getTrow(), para.getTcol(), tempant);//update cell
		para.setSant(growth -= amount);//update param
		endGrowth = (para.getSant() <= 0);//set return value
		if (endGrowth) { para.Reset();}
		return endGrowth;
	}
	void attackPara(int player, Field& game, Param& para) {
		int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validSource = false;
		game.printPlayer(player);
		while (!validSource) {
			cout << "Select cell to attack from (row col):";
			cin >> r >> c;
			if (r <= 0 || r > rows || c <= 0 || c > cols) {
				cout << "Cell is outside of game field!";
				continue;
			}
			else if (game.getOwner(r, c) != player) {
				cout << "You must own the cell.\n";
				continue;
			}
			else if (game.getAnts(r, c) < 2) {
				cout << "You must have at least 2 ants in the cell to attack.\n";
				continue;
			}
			else if (game.getStr(r, c) == 8) {
				cout << "This cell is surrounded by cells you own.\n";
				continue;
			}
			else {
				char confirm = ' ';
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  (Y)es ";
				cin >> confirm; confirm = toupper(confirm);
				if (confirm == 'Y') {
					para.setSource(r, c);
					validSource = true;
				}
				else { validSource = false; }
			}
		}
		// Valid ants
		bool validant = false;
		int maxant = game.getAnts(para.getSrow(), para.getScol()) - 1;
		while (!validant) {
			cout << "You have " << maxant << " ants available to attack with.\n";
			cout << "Enter the number of ants to use in the attack: ";
			cin >> amount;
			if (amount < 1) {
				cout << "You must use at least 1 ant.";
				amount = 0;
			}
			else if (amount > maxant) {
				cout << "You don't have that many ants available! ";
				amount = 0;
			}
			else {
				para.setSant(amount);
				maxant -= amount;
				game.setAnts(para.getSrow(), para.getScol(), maxant);//update cell
				validant = true;
				cout << "\n";
			}
		}
		bool validTarget = false;
		int srow = para.getSrow(); int scol = para.getScol();
		int rmin = max(0, srow - 1);// min row limit
		int rmax = min(rows - 1, srow + 1);// max row limit
		int cmin = max(0, scol - 1);// min col limit
		int cmax = min(cols - 1, scol + 1);//max col limit
		while (!validTarget) {
			game.printMinMax(srow, scol, player);
			cout << "Select target cell to attack (row col):";
			cin >> r >> c;
			if (r < rmin || r > rmax || c < cmin || c > cmax) {
				cout << "Target cell must be adjacent to source cell!\n";
				continue;
			}
			else if (r <= 0 || r > rows || c <= 0 || c > cols) {
				cout << "Cell is outside of game field!";
				continue;
			}
			else if (r==srow && c==scol) {
				cout << "Target cell must be different from source cell!\n";
				continue;
			}
			else if (game.getOwner(r, c) == player) {
				cout << "You must attack a cell you do not own.\n";
				continue;
			}
			else {
				char confirm = ' ';
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  (Y)es ";
				cin >> confirm; confirm = toupper(confirm);
				if (confirm == 'Y') {
					para.setTarget(r, c);
					para.setTant(game.getAnts(r, c));//set target ants
					validTarget = true;
				}
				else { validTarget = false; }
			}
		}
	}
	int Battle(int player, Field& game, Param& para) const {
		int attack = para.getSant();//attack ants
		int defend = para.getTant();//defend ants
		int srow = para.getSrow(); int scol = para.getScol();
		int trow = para.getTrow(); int tcol = para.getTcol();
		srand((unsigned)time(0));
		while (defend != 0 && attack != 0) {
			int atroll = (1 + rand() % 101);
			int defroll = (1 + rand() % 101);
			if (atroll > defroll) {//attack wins
				defend--;
			}
			else if (atroll <= defroll) {
				attack--;
			}

			if (defend == 0) {
				game.setOwner(trow, tcol, player);
				game.setAnts(trow, tcol, attack);
				return true;
			}
			else {
				return false;
			}
		}
		return endBattle;
	}
	void movePara(int player, Field& game, Param& para) {
		int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validSource = false;
		game.printPlayer(player);
		while (!validSource) {
			cout << "Select cell to move from (row col):";
			cin >> r >> c;
			if (r <= 0 || r > rows || c <= 0 || c > cols) {
				cout << "Cell is outside of game field!";
				continue;
			}
			else if (game.getOwner(r, c) != player) {
				cout << "You must own the cell.\n";
				continue;
			}
			else if (game.getAnts(r, c) < 2) {
				cout << "You must have at least 2 ants in the cell to attack.\n";
				continue;
			}
			else {
				char confirm = ' ';
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  (Y)es ";
				cin >> confirm; confirm = toupper(confirm);
				if (confirm == 'Y') {
					para.setSource(r, c);
					validSource = true;
				}
				else { validSource = false; }
			}
		}
		// Valid ants
		bool validant = false;
		int maxant = game.getAnts(para.getSrow(), para.getScol()) - 1;
		while (!validant) {
			cout << "You have " << maxant << " ants available.\n";
			cout << "Enter the number of ants to move: ";
			cin >> amount;
			if (amount < 1) {
				cout << "You must use at least 1 ant.";
				amount = 0;
			}
			else if (amount > maxant) {
				cout << "You don't have that many ants available! ";
				amount = 0;
			}
			else {
				para.setSant(amount);
				maxant -= amount;
				game.setAnts(para.getSrow(), para.getScol(), maxant);//update cell
				validant = true;
				cout << "\n";
			}
		}
		bool validTarget = false;
		int srow = para.getSrow(); int scol = para.getScol();
		int rmin = max(0, srow - 1);// min row limit
		int rmax = min(rows - 1, srow + 1);// max row limit
		int cmin = max(0, scol - 1);// min col limit
		int cmax = min(cols - 1, scol + 1);//max col limit
		while (!validTarget) {
			game.printMinMax(srow, scol, player);
			cout << "Select target cell (row col):";
			cin >> r >> c;
			if (r < rmin || r > rmax || c < cmin || c > cmax) {
				cout << "Target cell must be adjacent to source cell!\n";
				continue;
			}
			else if (r <= 0 || r > rows || c <= 0 || c > cols) {
				cout << "Cell is outside of game field!";
				continue;
			}
			else if (r == srow && c == scol) {
				cout << "Target cell must be different from source cell!\n";
				continue;
			}
			else if (game.getOwner(r, c) != player) {
				cout << "You can't move to a cell you do not own.\n";
				continue;
			}
			else {
				char confirm = ' ';
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  (Y)es ";
				cin >> confirm; confirm = toupper(confirm);
				if (confirm == 'Y') {
					para.setTarget(r, c);
					para.setTant(game.getAnts(r, c));//set target ants
					validTarget = true;
				}
				else { validTarget = false; }
			}
		}
	}
		int TheMove(int player, Field& game, Param& para) const {
			int current = para.getTant();
			int moving = para.getSant();
			game.setAnts(para.getTrow(), para.getTcol(), current + moving);
			return true;
	}

};

