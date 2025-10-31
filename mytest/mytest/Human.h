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
	void delay(int num) { this_thread::sleep_for(chrono::seconds(num)); }

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
		player.ToDialog(0, name, game);
		delay(2);
		int temp = 0; int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validTarget = false, grow = false;
		while (!grow) {
			validTarget = false;
			game.printPlayer(playerNum);
			while (!validTarget) {
				player.ToDialog(0, "Select cell for new ants: (Mouse)", game);
				player.getMouse(game);
				r = player.getTemp1(); c = player.getTemp2();
				//game.renderCell(r, c, true);
				if (r < 0 || r > rows || c < 0 || c > cols) {
					player.ToDialog(0, "Cell is outside of game field!  ", game);
					game.renderCell(r, c, false);
					validTarget = false;
				}
				else if (game.getOwner(r, c) != playerNum) {
					player.ToDialog(0, "You must own the cell.", game);
					game.renderCell(r, c, false);
					validTarget = false;
				}
				else {
					player.setTargetCell(playerNum, r, c);
					player.ToGrowth(playerNum, game);
						validTarget = true;
					}
				}
			
			bool validant = false;
			while (!validant) {
				player.setTargetAnts(playerNum, 0);
				int growth = player.getSourceAnts(playerNum);
				string msg = player.getPlayerName(playerNum) + " you have " + to_string(growth) + " ants.";
				player.ToDialog(0, msg , game);
				player.ToDialog(1, "Number of ants to place? (Kbd)", game);
				game.SetColor(1, 7);
				cin >> amount;
				if (amount < 1) {
					player.ToDialog(1,"You must place at least 1 ant. ", game);
					delay(3);
					amount = 0;
				}
				else if (amount > growth) {
					player.ToDialog(1,"You don't have that many ants! ", game);
					delay(3);
					amount = 0;
				}
				else {
					player.setTargetAnts(playerNum, amount);
					player.ToGrowth(playerNum, game);
					validant = true;
				}
			}
			game.SetColor(7, 0);
			grow = player.runGrowth(playerNum, game);
		}
		return true;
	}
	int Attack(int playerNum, Field& game, Player& player) {
		string name = player.getPlayerName(playerNum);
		name += " Attack Phase. ";
		player.Attack(game);
		int rows = game.getRows(), cols = game.getCols();
		int r = 0, c = 0, amount = 0;
		bool attack = false, second = false, valid = false;
		while (!attack && !second) {
			while (!valid) {//get source cell
				player.ToDialog(0, name, game);
				player.ToDialog(1,"Select attacking cell: (Mouse)",game);
				player.getMouse(game);
				r = player.getTemp1(); c = player.getTemp2();
				game.renderCell(r, c, true);
				if (game.getOwner(r, c) != playerNum) {
					player.ToDialog(2,"You must own the cell.      ",game);
					game.renderCell(r, c, false);
					valid = false;
				}
				else if (game.getAnts(r, c) < 2) {
					player.ToDialog(2,"No ants available to attack.",game);
					game.renderCell(r, c, false);
					valid = false;
				}
				else if (game.getStr(r, c) == 8) {
					player.ToDialog(2,"This cell is surrounded.    ",game);
					game.renderCell(r, c, false);
					valid = false;
				}
				else {
					player.setSourceCell(playerNum, r, c);
					game.renderCell(r, c, true);
					//player.ToAttack()
					valid = true;
					}
			}//end source cell
		// Valid source ants
			bool validant = false;
			player.setSourceAnts(playerNum, 0);
			int r = player.getSourceRow(playerNum);
			int c = player.getSourceCol(playerNum);
			int maxant = game.getAnts(r, c) - 1;
			while (!validant) {
				string msg = player.getPlayerName(playerNum) + " you have " + to_string(maxant) + " ants.";
				player.ToDialog(0, msg, game);
				player.ToDialog(1, "Number of attack ants? (Kbd)", game);
				game.SetColor(1, 7);
				cin >> amount;
				if (amount < 1) {
					player.ToDialog(2, "You must use at least 1 ant.",game);
					amount = 0;
				}
				else if (amount > maxant) {
					player.ToDialog(2, "You don't have that many ants! ",game);
					amount = 0;
				}
				else {
					player.setSourceAnts(playerNum, amount);
					maxant -= amount;
					game.setAnts(r, c, 1 + maxant);//update game cell
					validant = true;
				}
				game.SetColor(7, 0);
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
				player.ToDialog(0, name, game);
				player.ToDialog(1, "Select target cell: (Mouse)", game);
				player.getMouse(game);
				r = player.getTemp1(); c = player.getTemp2();
				game.renderCell(r, c, true);
				if (r < rmin || r > rmax || c < cmin || c > cmax) {
					player.ToDialog(1,"Target cell must be adjacent! ", game);
					game.renderCell(r, c, false);
					validTarget = false;
				}
				else if (game.getOwner(r, c) == playerNum) {
					player.ToDialog(1, "You can't attack yourself!    ", game);
					game.renderCell(r, c, false);
					validTarget = false;
				}
				else {
					player.setTargetCell(playerNum, r, c);
					player.setTargetAnts(playerNum, game.getAnts(r, c));//set target ants
					validTarget = true;
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
		int rows = game.getRows(); int cols = game.getCols();
		int r = 0, c = 0, amount = 0; bool validSource = false;
		game.printPlayer(playerNum);
		while (!validSource) {
			player.ToDialog(0, name, game);
			player.ToDialog(1, "Move from cell: (Mouse)", game);
			player.getMouse(game);
			r = player.getTemp1(); c = player.getTemp2();
			game.renderCell(r, c, true);
			if (game.getOwner(r, c) != playerNum) {
				player.ToDialog(2, "You must own the cell.      ", game);
				game.renderCell(r, c, false);
				validSource = false;
			}
			else if (game.getAnts(r, c) < 2) {
				player.ToDialog(2, "No ants available to move.", game);
				game.renderCell(r, c, false);
				validSource = false;
			}
			else {
				player.setSourceCell(playerNum, r, c);
				validSource = true;
			}
			validSource = true;
		}
		// Valid ants
		bool validant = false;
		int maxant = game.getAnts(r, c)-1;
		while (!validant) {
			string msg = player.getPlayerName(playerNum) + " you have " + to_string(maxant) + " ants.";
			player.ToDialog(0, msg, game);
			player.ToDialog(1, "How many ants? (Kbd)", game);
			game.SetColor(1, 7);
			cin >> amount;
			if (amount < 1) {
				player.ToDialog(2, "You must use at least 1 ant.", game);
				amount = 0;
			}
			else if (amount > maxant) {
				player.ToDialog(2, "You don't have that many ants! ", game);
				amount = 0;
			}

			else {
				player.setSourceAnts(playerNum, amount);
				maxant -= amount;
				game.setAnts(r, c, maxant+1);//update cell
				validant = true;
			}
			game.SetColor(7, 0);
		}
		
		bool validTarget = false;
		int srow = player.getSourceRow(playerNum); int scol = player.getSourceCol(playerNum);
		int rmin = max(0, srow - 1);// min row limit
		int rmax = min(rows - 1, srow + 1);// max row limit
		int cmin = max(0, scol - 1);// min col limit
		int cmax = min(cols - 1, scol + 1);//max col limit
		while (!validTarget) {
			//game.printMinMax(srow, scol, playerNum);
			player.ToDialog(0, name, game);
			player.ToDialog(1, "Select target cell: (Mouse)", game);
			player.getMouse(game);
			r = player.getTemp1(); c = player.getTemp2();
			game.renderCell(r, c, true);
			if (r < rmin || r > rmax || c < cmin || c > cmax) {
				player.ToDialog(1, "Target cell must be adjacent! ", game);
				game.renderCell(r, c, false);
				validTarget = false;
			}
			else if (r == srow && c == scol) {
				player.ToDialog(1, "Target cell must be different!", game);
				game.renderCell(r, c, false);
				validTarget = false;
			}
			else if (game.getOwner(r, c) != playerNum) {
				player.ToDialog(1, "You must own the cell!        ", game);
				game.renderCell(r, c, false);
				validTarget = false;
			}
			else {
				player.setTargetCell(playerNum, r, c);
				player.setTargetAnts(playerNum, game.getAnts(r, c));//set target ants
				game.renderCell(r, c, true);
				validTarget = true;
			}
			move = player.runMove(playerNum, game);
			return move;
		}
		return true;
	}
};

