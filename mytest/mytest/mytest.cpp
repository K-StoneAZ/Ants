// mytest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include "Player.h"
#include "Field.h"
#include "Human.h"
#include "AIEasy.h"
#include "Start.h"
#include "Save.h"


using namespace std;
int turncounter = 1;
bool started = false;


static void quit() { exit(0); }

static Player Gamestart0(bool started, Start& start, Save& savegame) {
	cout << "Welcome to the Ants Game!" << endl;
	cout << "The objective of the game is to conquer 2/3 of the game field." << endl;
	cout << "Each player starts with a number of cells on the field." << endl;
	cout << "Each turn consists of three phases: Growth, Attack, and Move." << endl;
	cout << "In the Growth phase, players receive new ants based on the number of cells they own." << endl;
	cout << "In the Attack phase, players can attack adjacent enemy cells to conquer them." << endl;
	cout << "In the Move phase, players can move ants between their own cells." << endl;
	cout << "The game continues until one player controls 2/3 of the field or all other players are eliminated." << endl;
	cout << "Good luck and have fun!" << endl;
	if (!started) {
		Player player = start.getNunPlayers();
		return player;
	}
	Player player = start.LoadPlayers(savegame);
	return player;
}
static Field Gamestart1(bool started, Start& start, Save& savegame) {
	if (!started) {
		Field game = start.setupFieldSize();
		return game;
	}
	Field game = start.LoadField(savegame);
	return game;
}

//function to run turn
static void RunTurn(Player& player, Field& game) {
	// for each player
	for (int i = 1; i < player.getNumPlayers(); i++) {
		//count cells owned	at the begining of each player's turn
		bool growth = false, attack = false, second = false, move = false, aiFlag = false;
		player.countCellsOwned(game);
		game.printField();
		player.PrintPlayerSummary();
		if (player.getCellsOwned(i) == 0) {
			continue; // skip turn if player has no cells
		}
		else if (player.getPlayerType(i) == 1) { // human player
			game.setCellstr(i);
			Human PlayerTurn;
			// Growth Phase
			int temp = (player.getCellsOwned(i) / 3);//calculate growth
			player.setSourceAnts(i, temp);//set source = growth
			while (!growth) {
				growth = PlayerTurn.Growth(i, game, player);//human class player
			}
			// Attack Phase
			game.printField();
			if (!attack) {
				attack = PlayerTurn.Attack(i, game, player);
			}
			if (attack && !second) {
				game.printField();
				second = PlayerTurn.Attack(i, game, player);
			}
			// Move Phase
			game.printPlayer(i);
			cout << player.getPlayerName(i)<<", would you like to Move? (Y)es. ";
			char a = ' ';
			cin >> a; a = toupper(a);
			if (a != 'Y') { move = true; }
			else {
				move = PlayerTurn.Move(i, game, player);
			}
		}
		else if (player.getPlayerType(i) == 2) { // AI player
			bool growth = false, attack = false, second = false, move = false, aiFlag = false;
			player.countCellsOwned(game);
			game.setCellstr(i);
			int count = player.getCellsOwned(i);
			AIEasy PlayerTurn(i, count);
			PlayerTurn.populate(game);
			int temp = (count / 3);//calculate growth
			player.setSourceAnts(i, temp);//set source = growth
			aiFlag = PlayerTurn.Turn(game, player);//begin AI "logic??"
		}
	}

}
static void CheckWin(Field& game, Player& player, Start& start) {
	bool won = false;
	player.countCellsOwned(game);
	won = player.Win(game);
	while (!won) {
		cout << "\n" << turncounter << "\n";
		RunTurn(player, game);
		won = player.Win(game);
		game.printField();
		player.PrintPlayerSummary();
		if (start.nextTurn()) {
			if (!start.saveGame(game, player, turncounter)) {// game not saved error}
			}
			else {
				cout << "Game Saved.\n";
				if (start.nextTurn1() == 2) { quit(); }
				else { continue; }
			}
		}
		turncounter++;
	}
}
int main()
{
	// create Player obj and Field obj
	Save savegame;
	Start start;
	started = start.Start0(savegame);
	Player player = Gamestart0(started, start, savegame);
	Field game = Gamestart1(started, start, savegame);
	CheckWin(game, player, start);



}
