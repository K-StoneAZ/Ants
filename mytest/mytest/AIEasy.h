#pragma once
#include <iostream>
#include <cstdlib>
#include <vector>
#include<string>
#include <ctime>
#include<thread>
#include<chrono>
#include"Field.h"
#include"Player.h"

class AIEasy
{
protected:
	int currentPlayer{ 0 };
	int count{ 100 };
	bool growth{ false };
	bool attack{ false };
	bool attack2{ false };
	bool move{ false };

	struct ai {
		int rows{ 0 };
		int cols{ 0 };
	};
	vector<ai> aiStats;//holds the locations of all cells owned by current player
	vector<ai> MaxCell;//holds the growth target and source attack cell
	vector<ai> enemy;//holds the locations of enemy cells
	vector<ai> Move;//holds the locations of cells with str of 8 and > 1 ant

// Movement only looks at the attack2 source and target
//needs to be expanded to look at attack source also
//will need to add storage for primary and secondary attack cells
//and/or create vector of cells owned that have str of 8
	
public:
	//count is the number of cells owned by current player
	AIEasy(int currentPlayer = 0, int count = 100)
		: currentPlayer(currentPlayer)
		, count(count) {
		aiStats.resize(count);
		for (int i = 0; i < count; ++i) {
			aiStats[i].rows = 0;
			aiStats[i].cols = 0;
		}
	}
	//create vector of ai struct by resizing it to number of cells owned
	void populate(Field& game) {
		aiStats.resize(count);
		int i = 0;
		while (i < count) {
			for (int j = 0; j < game.getRows(); j++) {
				for (int k = 0; k < game.getCols(); k++) {
					if (game.getOwner(j, k) == currentPlayer) {
						aiStats[i].rows = j;
						aiStats[i].cols = k;
						i++;
					}
				}
			}
		}
	}
	void delay(string msg, int num) {//simple delay function
		cout << msg <<" >";
		for (int i = 0; i < num; i++) {
			this_thread::sleep_for(chrono::seconds(1));
			cout << " >";
		}cout << "\n";
	}
	//build a traffic cop to break out the elements of the turn function
	// determine all variables that need to pass between subfunctions

	int Turn(Field& game, Player& player) {
		string name = player.getPlayerName(currentPlayer);
		name +=" Growth Phase";
		player.ToDialog(0, name, game);
		player.delay(4);
		growth = grow(game, player);
		// may need player.ToGrowth(currentPlayer, game); // here
		growth = player.runGrowth(currentPlayer, game);
		game.printPlayer(currentPlayer);
		name = player.getPlayerName(currentPlayer);
		name += " Growth complete!";
		if (growth){
			player.ToDialog(0, name, game);
			player.delay(4);}
		attack = att(game, player);
		attack = player.runAttack(currentPlayer, game);
		name = player.getPlayerName(currentPlayer);
		name +=" Primary attack complete!";
		player.ToDialog(0, name, game);
		player.delay(4);
		game.printField();
		player.PrintPlayerSummary(game);
		player.ToDialog(1, "Setting up secondary attack.", game);
		if(attack && !attack2){
			game.printPlayer(currentPlayer);
			attack2 = att2(game, player);
			if (attack2) {
				attack2 = player.runAttack(currentPlayer, game);
				attack2 = true;
			}
			else { attack2 = true; }
		}
		if(attack2){
			name = player.getPlayerName(currentPlayer);
			name +=" Attack complete!";
			player.ToDialog(0, name, game);
			player.delay(4);
		}
		game.printPlayer(currentPlayer);
		move = movement(game, player);

		if (move) {
			move = player.runMove(currentPlayer, game);
			name = player.getPlayerName(currentPlayer);
			name +=" Move complete!\n";
			player.ToDialog(0, name, game);
			player.delay(4);
		}
		else { move = true;
		name = player.getPlayerName(currentPlayer);
		name += " Move complete!";
		player.ToDialog(0, name, game);
		player.delay(4);
		}
		if(growth && attack && move){
			enemy.clear(); MaxCell.clear(); aiStats.clear(); Move.clear();
			growth = false; attack = false; attack2 = false; move = false;
			return true;
		}//turn over
		return true;
	}
	int grow(Field& game, Player& player) {//Max str
		int row1 = 0, col1 = 0, smax = 0, tempMax = 0, r = 0, c = 0, maxCount = 0, random = 0;
		srand(static_cast<unsigned int>(time(0)));
		for (int i = 0; i < count; i++) {
			row1 = aiStats[i].rows;
			col1 = aiStats[i].cols;
			smax = game.getStr(row1, col1);
			if (smax == 8) {}
			else if (smax > tempMax) {
				tempMax = smax;//get highest cell str < 8
			}
		}
		for (int i = 0; i < count; i++) {//count cells of tempmax str
			row1 = aiStats[i].rows;	col1 = aiStats[i].cols;
			if (game.getStr(row1, col1) == tempMax) {
				maxCount++;//count number of cells of tempmax str
			}
		}
		int i = 0; MaxCell.resize(maxCount);
		for (int j = 0; j < count; j++) {
			row1 = aiStats[j].rows;	col1 = aiStats[j].cols;
			if (game.getStr(row1, col1) == tempMax) {
				MaxCell[i].rows = row1;	MaxCell[i].cols = col1;
				i++;//populate vector with cell locations of tempmax str
			}
			else if (i > maxCount) { break; }
		}
		if (maxCount == 1) {//growth target cell and attck source cell
			player.setSourceCell(currentPlayer, MaxCell[0].rows, MaxCell[0].cols);
			player.setTargetCell(currentPlayer, MaxCell[0].rows, MaxCell[0].cols);
		}
		else {
			random = 0 + rand() % maxCount;
			player.setSourceCell(currentPlayer, MaxCell[random].rows, MaxCell[random].cols);
			player.setTargetCell(currentPlayer, MaxCell[random].rows, MaxCell[random].cols);
		}
		//ants for growth
		int temp = 0;
		temp = player.getSourceAnts(currentPlayer);//growth ants from Main()
		player.setTargetAnts(currentPlayer, temp);//place all growth in one cell
		int row = player.getSourceRow(currentPlayer), col = player.getSourceCol(currentPlayer);
		player.ToGrowth(currentPlayer, game);
		//ready to run Growth Phase
		return true;
	}
	int att(Field& game, Player& player){
		//begim Attack Phase
		//find enemy cells as potential target cell
		srand(static_cast<unsigned int>(time(0)));
		game.setCellstr(currentPlayer);
		int temp = player.getSourceAnts(currentPlayer);
		int Rows = game.getRows();
		int Cols = game.getCols();
		int row = player.getSourceRow(currentPlayer);
		int col = player.getSourceCol(currentPlayer);
		Field::Area area = game.getArea(row, col);
		int rmin = area.rmin, rmax = area.rmax, cmin = area.cmin, cmax = area.cmax;
		int enemys = 8 - game.getStr(row, col);
		int k = 0;
		if (enemys == 0) { return true; }
		enemy.resize(enemys);
		player.ToDialog(0,"Finding enemy cells:", game);
		temp = 0;
		for (int i = rmin; i <= rmax; i++) {
			for (int j = cmin; j <= cmax; j++) {//something in here is messed up
				temp = game.getOwner(i, j);
				if (temp != currentPlayer) {
					enemy[k].rows = i;
					enemy[k].cols = j;
					k++;
				}
			}
		}//find enemy target and Source ants
		if (enemys == 1) {
			int r = enemy[0].rows, c = enemy[0].cols;
			int row = player.getSourceRow(currentPlayer);
			int col = player.getSourceCol(currentPlayer);
			int ants = game.getAnts(row, col) - 1;
			player.setTargetCell(currentPlayer, r, c);//set target cell with enemy cell
			int number = game.getOwner(r, c);
			if (number == 0) {//Target cell owner
				player.setSourceAnts(currentPlayer, ants);//set source = ants to all available
				game.setAnts(row, col, 1);//update game cell ants
			}
			else {
				int enemy_ant = game.getAnts(r, c);
				enemy_ant = enemy_ant + 2;
				if (enemy_ant <= ants) {
					player.setSourceAnts(currentPlayer, enemy_ant);
					ants = ants - enemy_ant + 1;
					game.setAnts(row, col, ants);//reduce source cell ants
				}
				else {
					player.setSourceAnts(currentPlayer, ants);
					game.setAnts(row, col, 1);//update game cell ants
				}
			}
			//cout << "Attacking with " << player.getSourceAnts(currentPlayer) << " ants.";
			//cout << "\nEnemy Cell: " << r << ", " << c << " is " << player.getPlayerName(number);
			//cout << " defending with " << game.getAnts(r, c) << " ants.\n";
		}
		else {
			int random = 0 + rand() % enemys;
			int row = player.getSourceRow(currentPlayer);
			int col = player.getSourceCol(currentPlayer);
			int r = enemy[random].rows, c = enemy[random].cols;
			int ants = game.getAnts(row, col) - 1;//all available ants
			player.setTargetCell(currentPlayer, r, c);
			int number = game.getOwner(r, c);
			if (number == 0) {//Target cell owner
				player.setSourceAnts(currentPlayer, ants);//set source = ants to all available
				game.setAnts(row, col, 1);//update game cell ants
			}
			else {
				int enemy_ant = game.getAnts(r, c);
				enemy_ant += 2;
				if (enemy_ant <= ants) {
					player.setSourceAnts(currentPlayer, enemy_ant);
					ants = (ants + 1) - enemy_ant;
					game.setAnts(row, col, ants);//reduce source cell ants
				}
				else {
					player.setSourceAnts(currentPlayer, ants);
					game.setAnts(row, col, 1);//update game cell ants
				}
			}
		}
		//Ready for attack phase
		return true;
	}

	int att2(Field& game, Player& player) {
		srand(static_cast<unsigned int>(time(0)));
		game.setCellstr(currentPlayer);
		int Rows = game.getRows(); int Cols = game.getCols();
		int row = 0, col = 0;
		int srow = player.getSourceRow(currentPlayer); int scol = player.getSourceCol(currentPlayer);
		int trow = player.getTargetRow(currentPlayer); int tcol = player.getTargetCol(currentPlayer);
		int sown = game.getOwner(srow, scol);
		int sant = game.getAnts(srow, scol);
		int sstr = game.getStr(srow, scol);
		int town = game.getOwner(trow, tcol);
		int tant = game.getAnts(trow, tcol);
		int tstr = game.getStr(trow, tcol);
		if (sown == currentPlayer && sstr < 8 && sant > 1) {
			row = srow; col = scol;
			player.setSourceCell(currentPlayer, row, col);
			player.setSourceAnts(currentPlayer, sant);
		}
		else if (town == currentPlayer && tstr < 8 && tant > 1) {
			row = trow; col = tcol;
			player.setSourceCell(currentPlayer, row, col);
			player.setSourceAnts(currentPlayer, tant);
		}
		else { return false; }
		Field::Area area = game.getArea(row, col);
		int rmin = area.rmin, rmax = area.rmax, cmin = area.cmin, cmax = area.cmax;
		int enemys = 8 - game.getStr(row, col);
		int k = 0;
		enemy.resize(enemys);
		int temp = 0;
		for (int i = rmin; i <= rmax; i++) {
			for (int j = cmin; j <= cmax; j++) {//something in here is messed up
				temp = game.getOwner(i, j);
				if (temp != currentPlayer) {
					enemy[k].rows = i;
					enemy[k].cols = j;
					k++;
				}
			}
		}//find enemy target and Source ants
		if (enemys == 1) {
			int r = enemy[0].rows, c = enemy[0].cols;
			int row = player.getSourceRow(currentPlayer);
			int col = player.getSourceCol(currentPlayer);
			int ants = game.getAnts(row, col) - 1;
			player.setTargetCell(currentPlayer, r, c);//set target cell with enemy cell
			int number = game.getOwner(r, c);
			if (number == 0) {//Target cell owner
				player.setSourceAnts(currentPlayer, ants);//set source = ants to all available
				game.setAnts(row, col, 1);//update game cell ants
			}
			else {
				int enemy_ant = game.getAnts(r, c);
				enemy_ant = enemy_ant + 2;
				if (enemy_ant <= ants) {
					player.setSourceAnts(currentPlayer, enemy_ant);
					ants = ants - enemy_ant + 1;
					game.setAnts(row, col, ants);//reduce source cell ants
				}
				else {
					player.setSourceAnts(currentPlayer, ants);
					game.setAnts(row, col, 1);//update game cell ants
				}
			}
		}
		else {
			int random = 0 + rand() % enemys;
			int row = player.getSourceRow(currentPlayer);
			int col = player.getSourceCol(currentPlayer);
			int r = enemy[random].rows, c = enemy[random].cols;
			int ants = game.getAnts(row, col) - 1;
			player.setTargetCell(currentPlayer, r, c);
			int number = game.getOwner(r, c);
			if (number == 0) {//Target cell owner
				player.setSourceAnts(currentPlayer, ants);//set source = ants to all available
				game.setAnts(row, col, 1);//update game cell ants
			}
			else {
				int enemy_ant = game.getAnts(r, c);
				enemy_ant += 2;
				if (enemy_ant <= ants) {
					player.setSourceAnts(currentPlayer, enemy_ant);
					ants = (ants + 1) - enemy_ant;
					game.setAnts(row, col, ants);//reduce source cell ants
				}
				else {
					player.setSourceAnts(currentPlayer, ants);
					game.setAnts(row, col, 1);//update game cell ants
				}
			}
		}
		//Ready for attack phase
		return true;
	}

	int movement(Field& game, Player& player) {
		// Movement will update the aiStats vector and
		//create a Move vector that will containe the locations
		//of cells with a strangth of 8 and 3+ ants
		//it will then look at the subset area to find an adjacent location
		//that has a strength less than 8 to move excess ant population.
		srand(static_cast<unsigned int>(time(0)));
		game.setCellstr(currentPlayer);//update cell strength
		player.countCellsOwned(game);//update cells owned after attack
		count = player.getCellsOwned(currentPlayer);//update count after attack
		populate(game);//update aiStats with current gamefield
		//count cells with str of 8 and ants > 2
		int row1 = 0, col1 = 0, smax = 0, tempmax = 0, ant = 0;
		for (int i = 0; i < count; i++) {
			row1 = aiStats[i].rows;
			col1 = aiStats[i].cols;
			smax = game.getStr(row1, col1);
			ant = game.getAnts(row1, col1);
			if (smax == 8 && ant > 3) { tempmax++; }//tempmax = count of cells
		}
		if (tempmax > 0) {
			int i = 0; Move.resize(tempmax);
			for (int j = 0; j < count; j++) {
				row1 = aiStats[j].rows;	col1 = aiStats[j].cols;
				smax = game.getStr(row1, col1);
				ant = game.getAnts(row1, col1);
				if (smax == 8 && ant > 3) {
					Move[i].rows = row1; Move[i].cols = col1;
					i++;//populate vector with cell locations of tempmax str
				}
				else if (i > tempmax) { break; }
			}
		}
		else {return false;}//no move available
		//now the fun begins
		if (tempmax == 1){
			row1 = Move[0].rows; col1 = Move[0].cols;
			Field::Area area = game.getArea(row1, col1);
			int rmin = area.rmin, rmax = area.rmax, cmin = area.cmin, cmax = area.cmax;
			int r = 0, c = 0,sant = 0, tant = 0, current = 0;
			for (int i = rmin; i <= rmax; i++) {
				for (int j = cmin; j <= cmax; j++) {
					if (i == row1 && c == col1) {}
					else if (game.getStr(i, j) < 8) {
						r = i; c = j;
						player.setSourceCell(currentPlayer, row1, col1);
						player.setTargetCell(currentPlayer, r, c);
						sant = game.getAnts(row1, col1);
						current = game.getAnts(r, c);
						tant = current + sant - 1;
						player.setSourceAnts(currentPlayer, tant);
						game.setAnts(row1, col1, 2);
						return true;
					}
				}
			}
		}
		else {
			int random = 0 + rand() % tempmax;
			row1 = Move[random].rows; col1 = Move[random].cols;
			Field::Area area = game.getArea(row1, col1);
			int rmin = area.rmin, rmax = area.rmax, cmin = area.cmin, cmax = area.cmax;
			int r = 0, c = 0, sant = 0, tant = 0,current = 0;
			for (int i = rmin; i <= rmax; i++) {
				for (int j = cmin; j <= cmax; j++) {
					if (i == row1 && c == col1) {}
					else if (game.getStr(i, j) < 8) {
						r = i; c = j;
						player.setSourceCell(currentPlayer, row1, col1);
						player.setTargetCell(currentPlayer, r, c);
						sant = game.getAnts(row1, col1);
						current = game.getAnts(r, c);
						tant = current + sant - 1;
						player.setSourceAnts(currentPlayer, tant);
						game.setAnts(row1, col1, 2);
						return true;
					}
				}
			}
		}
		player.ToDialog(0,"No move this turn.", game);
		return false;//no move
	}
	int getCount() const {
		return count;
	}
	int getPlayer() const {
		return currentPlayer;
	}
};

