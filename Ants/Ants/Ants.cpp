// Ants.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ctime>
#include <cstdlib>
#include "Field.h"
#include "Count.h"
#include "Param.h"
#include "Work.h"

using namespace std;

static int Action(char a = ' ', bool battle = false)
{
	while (a != 'A' && a != 'M' && a != 'S' && a != 'Q') {
		if (battle){
			cout << "Your attack was successful. You may (A)ttack, (M)ove, (S)kip or (Q)uit: ";
		}
		else if (!battle) {
			cout << "You have just completed an attack. You may (M)ove, (S)kip or (Q)uit: ";
		}
		else {
			cout << "Choose your action: (A)ttack, (M)ove, (S)kip or (Q)uit: ";
		}
		cin >> a; a = toupper(a);
		if (a != 'A' && a != 'M' && a != 'S' && a != 'Q') {
			continue;
		}
		else { return a; }
	}
}

int main()
{
	//int random seed
	srand(static_cast<unsigned int>(time(0)));
	//Initalize Game
	cout << "Welcome to Ants!\n\n";
	//Game Variables
	int numPlayer = 0, skill = -1;
	while (numPlayer < 2 && numPlayer > 8) {
		cout << "Enter the number of players: (2 - 8)";
		cin >> numPlayer;
	}
	int const player = numPlayer + 1;//constant for cell ownership
	//skill level for AI
	while (skill < 0 && skill > 3) {
		cout << "Enter skill level: (0 = Easy, 1 = Medium, 2 = Hard, 3 = Expert)";
		cin >> skill;
	}
	// Game field size FUTURE DEVELOPMENT
	// default = 10 x 10, medium = 12 x 12, large = 15 x 15, huge = 20 x 20
	//		100 cells,			144 cels,		225 cells,		400 cells

	// Game complexity FUTURE DEVELOPMENT
	// 0 = base field
	// 1 = "Water" Water can be crossed but not owned
	// 2 = "Rocks" can not be crossed
	// 3 = Both water and rocks

	//Initalize and populate the game field
	Field game(10, 10);
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			game.setCell(i, j, 0 + (rand() % player));
		}
	}
	Count mycount(player);
	mycount.countCells(game);
	Param para(0, 0, 0, 0, 0, 0);
	Work work;
	//instantiate Work class obj
	cout << "          RULES:\n";
	cout << "1. Each player starts with a random number of cells with one ant in each cell.\n";
	cout << "2. Each player begins their turn by placing ants in their owned cells.\n";
	cout << "3. Player may then attack, move, or skip their turn.\n";
	cout << "4. If attack is choosen. Upon completion the turn will move to the next player.\n";
	cout << "5. If move is choosen. Upon completion the turn will move to the next player.\n";
	cout << "6. If skip is choosen. The turn will move to the next player.\n";
	cout << "7. The game ends when one player owns all cells.\n";
	cout << "\n          GOOD LUCK!\n";
	bool gameWin = false;
	bool nextTurn = false;
	bool battle = false, move = false;
	int battlecount = 0;
	while (!gameWin) {
		nextTurn = false;
		while (!nextTurn) {
			//things to do at the begining of each loop
			gameWin = mycount.GameWin();
			if (gameWin) {
				cout << "GAME OVER!";
				break;
			}
			if (mycount.getPlayerCount(1) == 0) {
				cout << "You have no cells remaining.\n     GAME OVER!\n";
				break;
			}
			//player loop
			for (int p = 1; p < player; p++) {//p is current player
				battlecount = 0;
				game.setCellstr(player);
				mycount.countCells(game);
				if (mycount.getPlayerCount(p) == 0) {
					continue;
				}
				else {
					cout << "\n";
					game.printField();
					cout << "     STATS:\n";
					mycount.printCount();
				}
				//Growth phase  Param class and Work class
				int growth = mycount.getGrowth(p);//growth for current player
				bool endGrowth = false;
				para.setSant(growth);
				if (p == 1) {
					cout << "Player " << p << " you have " << growth << " ants to deploy.\n";
					while (!endGrowth && p == 1) {
						work.growthPara(p, game, para);
						endGrowth = work.setGrowth(p, game, para);
					}
					para.Reset();
					//Action phase
					bool validaction = false;
					while (!validaction && endGrowth) {//loop until validaction true
						char action = Action(' ', battle);
						if (action == 'Q') {
							cout << "You have chosen to quit the game.\n     GAME OVER!\n";
							validaction = true;
							nextTurn = true;
							gameWin = true;
						}
						while (action == 'A') {
							cout << "Attack selected.\n";
							work.attackPara(p, game, para);
							battle = work.Battle(p, game, para);
							para.Reset();
							if (battle) {//Successful attack
								validaction = false;
								battlecount++;
								cout << "Your attack was successful!\n";
								battle = false;
								action = ' ';
							}
							else {
								cout << "Your attack failed.\n";
								validaction = true;
								action = ' ';
							}
						}
						if (action == 'M') {
							cout << "Move selected.\n";
							work.movePara(p, game, para);
							move = work.TheMove(p, game, para);
							para.Reset();
							if (move) { validaction = true; }
						}
						if (action == 'S') {
							cout << "Turn skipped.\n";
							validaction = true;
						}
						if (!battle && battlecount > 1) {
							validaction = true; battle = true;
						}
						else {validaction = false; }
					}
				}
				if (p > 1) {}//future development for AI players
				cout << "MADE IT through player. "<<p<<"\n";
				break;
			}
		}
	}
}
