#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Player.h"
#include "Field.h"
#include "Save.h"
using namespace std;

class Start
{
protected:
	int turncounter = 1;
	int gameSize = 0;
	int numPlayers = 0;
	bool saved = false;
	vector<string> saveData;
public:
	Player getNunPlayers(){
		saved = false;
		cout << "Starting a new game..." << endl;
		while (numPlayers < 2 || numPlayers > 8) {
			cout << "Enter number of players (2-8): ";
			cin >> numPlayers;
		}
		Player players(numPlayers + 1);
		players.editPlayerInfo();
		return players;
	}
	Field setupFieldSize() {
		gameSize = 0;
		while (gameSize < 1 || gameSize > 3) {
			cout << "Enter field size (1-3) 10X10, 12x12, 15x15: ";
			cin >> gameSize;
		}
		if (gameSize == 1) {
			Field Game(10, 10, numPlayers + 1);
			return Game;
		}
		else if (gameSize == 2) {
			Field Game(12, 12, numPlayers + 1);
			return Game;
		}
		else if (gameSize == 3) {
			Field Game(15, 15, numPlayers + 1);
			return Game;
		}
		else { return Field(); }
	}
	Player LoadPlayers(Save& save) {
		//parse SaveData[0]
		saveData = save.readSaveFile();
		istringstream ss(saveData[0]);
		ss >> gameSize >> numPlayers >> turncounter;
		Player players(numPlayers);
		players.loaddata(saveData[1]);
		return players;
	}
	Field LoadField(Save& save) {
		saveData = save.readSaveFile();
		istringstream ss(saveData[0]);
		ss >> gameSize >> numPlayers >> turncounter;
		int rows = 0, cols = 0;
		if (gameSize == 1) { rows = 10; cols = 10; }
		else if (gameSize == 2) { rows = 12; cols = 12; }
		else if (gameSize == 3) { rows = 15; cols = 15; }
		else { cout << "Invalid game size in save data." << endl; return Field(); }
		Field game(rows, cols, numPlayers + 1);
		game.loaddata(saveData[2]);
		return game;
	}

	// Start class to handle game initialization and setup
	bool Start0(Save& savegame) {
		while (!saved) {
			cout << "Welcome to Ants!" << endl;
			cout << "1. Start New Game" << endl;
			cout << "2. Load Game" << endl;
			int choice;
			cin >> choice;
			if (choice <=1 && choice >=2){
				cout << "Invalid input. Please enter a number between 1 and 3." << endl;
				continue;
			}
			switch (choice) {
				case 1: {	// Start new game
					return false;
				}
				case 2: {
				// Load game
				//check if file exists
					ifstream inf("antsave.txt");
					if (!inf.is_open()) {
						cout << "No saved game found. Continue to New game." << endl;
						return false;
					}
					else {
						saveData = savegame.readSaveFile();
						if (saveData.size() < 3) {
							cout << "Saved game data is corrupted. Continue to New game." << endl;
							return false;
						}
						else {
							saved = true;
							return true;
						}
					}
				}
				cout << "Invalid choice. Please try again." << endl;
			}
		}
		return true;
	}
	bool nextTurn(Field& game, Player& player) {
		int a{ 0 };
		player.ToDialog(0,"Continue or Save?  1 or 2 ", game);
		game.SetColor(1, 7);
		while (a < 1 || a > 2) {
			cin >> a;
			if (a == 1) { game.SetColor(7, 0); return false; }
		}
		game.SetColor(7, 0);
		return true;
	}
	int saveGame(Field& game, Player& player, int turncounter) {
		Save savegame; int test{};
		savegame.assembleSaveString(game, player, turncounter);
		test = savegame.writeSaveFile();
		if (test != 0) {
			player.ToDialog(0,"Error saving game.", game);
			return 0;
		}
		player.ToDialog(0,"Game saved successfully.", game);
		return 1;
	}
	int nextTurn1(Field& game, Player& player) {
		player.ToDialog(0,"Continue or quit ? 1 or 2 ", game);
		int a{ 0 };
		game.SetColor(1, 7);
		while (a < 1 || a > 2) {
			cin >> a;
		}
		game.SetColor(7, 0);
		return a;
	}
};

