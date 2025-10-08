#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include<thread>
#include<chrono>
#include"Field.h"
#include"Player.h"

using namespace std;

class Human
{
public:

	//turn functions
	void delay(string msg, int num) {//simple delay function
		cout << msg << " >";
		for (int i = 0; i < num; i++) {
			this_thread::sleep_for(chrono::seconds(1));
			cout << " >";
		}cout << "\n";
	}
	// Growth Phase
	int Growth(int playerNum, Field& game, Player& player) {
		string name = player.getPlayerName(playerNum);
		name += " Growth Phase. ";
		delay(name, 4);
		int temp = 0; int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validTarget = false, grow = false;
		while (!grow) {
			validTarget = false;
			game.printPlayer(playerNum);
			while (!validTarget) {
				cout << "Select cell to distribute the ants (row col):";
				cin >> r >> c;
				if (cin.fail()) { cin.clear(); }
				else if (r < 0 || r > rows || c < 0 || c > cols) {
					cout << "Cell is outside of game field!\n";
					validTarget = false;
				}
				else if (game.getOwner(r, c) != playerNum) {
					cout << "You must own the cell.\n";
					validTarget = false;
				}
				else {
					int a;
					cout << "You have choosen cell row " << r << ", col " << c << ".\n";
					cout << "Are you sure?  1= Yes ";
					cin>> a;
					if (cin.fail()) { cin.clear(); }
					else if (a == 1) {
						player.setTargetCell(playerNum, r, c);
						validTarget = true;
					}
					else { validTarget = false; }
				}
			}
			bool validant = false;
			while (!validant) {
				int growth = player.getSourceAnts(playerNum);
				cout << "Player " << player.getPlayerName(playerNum) << " you have " << growth << " ants to deploy.\n";
				cout << "Enter the number of ants to place in this cell: ";
				cin >> amount;
				if (cin.fail()) { cin.clear(); }
				else if (amount < 1) {
					cout << "You must place at least 1 ant.";
					amount = 0;
				}
				else if (amount > growth) {
					cout << "You don't have that many ants to deploy! ";
					amount = 0;
				}
				else {
					player.setTargetAnts(playerNum, amount);
					validant = true;
					cout << "\n";
				}
			}
			grow = player.runGrowth(playerNum, game);
		}
		return true;
	}
	int Attack(int playerNum, Field& game, Player& player) {
		string name = player.getPlayerName(playerNum);
		name += " Attack Phase. ";
		delay(name, 4);
		int rows = game.getRows(), cols = game.getCols();
		int r = 0, c = 0, amount = 0;
		bool attack = false, second = false, valid = false;
		while (!attack && !second) {
			while (!valid) {//get source cell
				cout << "Select cell to attack from (row col):";
				cin >> r >> c;
				if (cin.fail()) { cin.clear(); }
				else if (r < 0 || r > rows || c < 0 || c > cols) {
					cout << "Cell is outside of game field!\n";
					valid = false;
				}
				else if (game.getOwner(r, c) != playerNum) {
					cout << "You must own the cell.\n";
					valid = false;
				}
				else if (game.getAnts(r, c) < 2) {
					cout << "You must have at least 2 ants in the cell to attack.\n";
					valid = false;
				}
				else if (game.getStr(r, c) == 8) {
					cout << "This cell is surrounded by cells you own.\n";
					valid = false;
				}
				else {
					int a;
					cout << "You have choosen cell row " << r << ", col " << c << ".\n";
					cout << "Are you sure?  1 = Yes: ";
					cin>> a;
					if (cin.fail()) { cin.clear(); valid = false; }
					else if (a == 1) {
						player.setSourceCell(playerNum, r, c);
						valid = true;
					}
					else { valid = false; }
				}
			}//end source cell
		// Valid source ants
			bool validant = false;
			int r = player.getSourceRow(playerNum);
			int c = player.getSourceCol(playerNum);
			int maxant = game.getAnts(r, c) - 1;
			while (!validant) {
				cout << player.getPlayerName(playerNum) << " you have " << maxant << " ants available to attack with.\n";
				cout << "Enter the number of ants to use in the attack: ";
				cin >> amount;
				if (cin.fail()) { cin.clear(); }
				else if (amount < 1) {
					cout << "You must use at least 1 ant.";
					amount = 0;
				}
				else if (amount > maxant) {
					cout << "You don't have that many ants available! ";
					amount = 0;
				}
				else {
					player.setSourceAnts(playerNum, amount);
					maxant -= amount;
					game.setAnts(r, c, 1 + maxant);//update game cell
					validant = true;
					cout << "\n";
				}
			}//end source ants
		// valid target cell
			int srow = player.getSourceRow(playerNum);
			int scol = player.getSourceCol(playerNum);
			int rmin = max(0, srow - 1);// min row limit
			int rmax = min(rows - 1, srow + 1);// max row limit
			int cmin = max(0, scol - 1);// min col limit
			int cmax = min(cols - 1, scol + 1);//max col limit
			bool validTarget = false;
			while (!validTarget) {
				game.printMinMax(srow, scol, playerNum);
				cout << "Select target cell to attack (row col):";
				cin >> r >> c;
				if (cin.fail()) { cin.clear(); }
				else if (r < rmin || r > rmax || c < cmin || c > cmax) {
					cout << "Target cell must be adjacent to source cell!\n";
					validTarget = false;
				}
				else if (r < 0 || r > rows || c < 0 || c > cols) {
					cout << "Cell is outside of game field!";
					validTarget = false;
				}
				else if (r == srow && c == scol) {
					cout << "Target cell must be different from source cell!\n";
					validTarget = false;
				}
				else if (game.getOwner(r, c) == playerNum) {
					cout << "You must attack a cell you do not own.\n";
					validTarget = false;
				}
				else {
					int a;
					cout << "You have choosen cell row " << r << ", col " << c << ".\n";
					cout << "Are you sure?  1 = Yes ";
					cin >> a;
					if (cin.fail()) { cin.clear(); }
					else if (a == 1) {
						player.setTargetCell(playerNum, r, c);
						player.setTargetAnts(playerNum, game.getAnts(r, c));//set target ants
						validTarget = true;
					}
					else { validTarget = false; }
				}
			}
			attack = player.runAttack(playerNum, game);
			if (!attack) { return false; }
		}
		return true;
	}
	int Move(int playerNum, Field& game, Player& player) {
		bool move = false;
		string name = player.getPlayerName(playerNum);
		name += " Move Phase. ";
		delay(name, 4);
		int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validSource = false;
		game.printPlayer(playerNum);
		while (!validSource) {
			cout << "Select cell to move from (row col):";
			cin >> r >> c;
			if (cin.fail()) { cin.clear(); }
			else if (r < 0 || r > rows || c < 0 || c > cols) {
				cout << "Cell is outside of game field!";
				validSource = false;
			}
			else if (game.getOwner(r, c) != playerNum) {
				cout << "You must own the cell.\n";
				validSource = false;
			}
			else if (game.getAnts(r, c) < 2) {
				cout << "You must have at least 2 ants in the cell to move.\n";
				validSource = false;
			}
			else {
				int a;
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  1 = Yes ";
				cin>>a;
				if (cin.fail()) { cin.clear(); }
				else if (a == 1) {
					player.setSourceCell(playerNum, r, c);
					validSource = true;
				}
				else { validSource = false; }
			}
		}
		// Valid ants
		bool validant = false;
		int maxant = game.getAnts(r, c);
		while (!validant) {
			cout << "You have " << (maxant - 1) << " ants available.\n";
			cout << "Enter the number of ants to move: ";
			cin >> amount;
			if (cin.fail()) { cin.clear(); }
			else if (amount < 1) {
				cout << "You must use at least 1 ant.";
				amount = 0;
			}
			else if (amount > maxant) {
				cout << "You don't have that many ants available! ";
				amount = 0;
			}
			else {
				player.setSourceAnts(playerNum, amount);
				maxant -= amount;
				game.setAnts(r, c, maxant);//update cell
				validant = true;
				cout << "\n";
			}
		}
		bool validTarget = false;
		int srow = player.getSourceRow(playerNum); int scol = player.getSourceCol(playerNum);
		int rmin = max(0, srow - 1);// min row limit
		int rmax = min(rows - 1, srow + 1);// max row limit
		int cmin = max(0, scol - 1);// min col limit
		int cmax = min(cols - 1, scol + 1);//max col limit
		while (!validTarget) {
			game.printMinMax(srow, scol, playerNum);
			cout << "Select target cell (row col):";
			cin >> r >> c;
			if (cin.fail()) { cin.clear(); }
			else if (r < rmin || r > rmax || c < cmin || c > cmax) {
				cout << "Target cell must be adjacent to source cell!\n";
				validTarget = false;
			}
			else if (r < 0 || r > rows || c < 0 || c > cols) {
				cout << "Cell is outside of game field!";
				validTarget = false;
			}
			else if (r == srow && c == scol) {
				cout << "Target cell must be different from source cell!\n";
				validTarget = false;
			}
			else if (game.getOwner(r, c) != playerNum) {
				cout << "You can't move to a cell you do not own.\n";
				validTarget = false;
			}
			else {
				int a;
				cout << "You have choosen cell row " << r << ", col " << c << ".\n";
				cout << "Are you sure?  1 = Yes ";
				cin >> a;
				if (cin.fail()) { cin.clear(); }
				else if (a == 1) {
					player.setTargetCell(playerNum, r, c);
					player.setTargetAnts(playerNum, game.getAnts(r, c));//set target ants
					validTarget = true;
				}
				else { validTarget = false; }
			}
		}
		move = player.runMove(playerNum, game);
		return true;
	}
};

