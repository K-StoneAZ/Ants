#pragma once
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include<thread>
#include<chrono>
#include "Field.h"

using namespace std;
class Player
{
protected:
	int numPlayers{ 9 };

	struct PLAYER_STATS {

		int playerID{ 0 };
		string playerName{ "" };
		int playertype{ 0 }; // 0 for reserved, 1 for human, 2 for AI
		int cells_owned{ 0 };
		int srow{ 0 }; // source row
		int scol{ 0 }; // source col
		int trow{ 0 }; // target row
		int tcol{ 0 }; // target col
		int sant{ 0 }; // source ants
		int tant{ 0 }; // target ants
	};
	vector<PLAYER_STATS> playerStats;

public:
	Player(int p = 9) :numPlayers(p) {
		playerStats.resize(numPlayers);
		for (int i = 0; i < numPlayers; i++) {
			playerStats[i].playerID = i;
			playerStats[i].playerName = "Player " + to_string(i + 1);
			playerStats[i].playertype = 0; // default to reserved
			playerStats[i].cells_owned = 0;
			playerStats[i].srow = 0;
			playerStats[i].scol = 0;
			playerStats[i].trow = 0;
			playerStats[i].tcol = 0;
			playerStats[i].sant = 0;
			playerStats[i].tant = 0;
		}
	}
	~Player() {
		playerStats.clear();
	}
	// load player data from save string
	void loaddata(const string& data) {
		istringstream ss(data);
		string playerInfo;
		int playerID, playertype;
		string playerName;
		for (int i = 0; i < numPlayers; i++) {
			if (!getline(ss, playerInfo, '|')) {
				cout << "Error reading player data." << endl;
				return;
			}
			istringstream pss(playerInfo);
			if (!(pss >> playerID >> playerName >> playertype)) {
				cout << "Error parsing player data." << endl;
				return;
			}
			if (playerID < 0 || playerID >= numPlayers) {
				cout << "Invalid player ID in save data." << endl;
				return;
			}
			playerStats[playerID].playerID = playerID;
			playerStats[playerID].playerName = playerName;
			playerStats[playerID].playertype = playertype;
		}
	}
	// getters and setters
	int getNumPlayers() const {
		return numPlayers;
	}
	int getPlayerID(int playerID) const {
		return playerStats[playerID].playerID;
	}
	string getPlayerName(int playerID) const {
		return playerStats[playerID].playerName;
	}
	int getPlayerType(int playerID) const {
		return playerStats[playerID].playertype;
	}
	int getCellsOwned(int playerID) const {
		return playerStats[playerID].cells_owned;
	}
	int getSourceRow(int playerID) const {
		return playerStats[playerID].srow;
	}
	int getSourceCol(int playerID) const {
		return playerStats[playerID].scol;
	}
	int getSourceAnts(int playerID) const {
		return playerStats[playerID].sant;
	}
	int getTargetRow(int playerID) const {
		return playerStats[playerID].trow;
	}
	int getTargetCol(int playerID) const {
		return playerStats[playerID].tcol;
	}
	int getTargetAnts(int playerID) const {
		return playerStats[playerID].tant;
	}
	string getSaveString() const {
		string saveData{ "" };
		for (int i = 0; i < numPlayers; i++) {
			saveData += to_string(playerStats[i].playerID) + " " + playerStats[i].playerName + " " + to_string(playerStats[i].playertype) + "|";
		}
		return saveData + "\n";
	}
	void editPlayerInfo() {
		string playerName;
		int playertype;
		playerStats[0].playerName = "Unowned"; // first player is always null player
		playerStats[0].playertype = 0; // reserved player type
		for (int i = 1; i < numPlayers; i++) {
			cout << "Select player type for Player " << i << " (1 for Human, 2 for AI): ";
			cin >> playertype;
			while (playertype < 1 || playertype > 2) {
				cout << "Invalid player type. Please enter 1 for Human or 2 for AI: ";
				cin >> playertype;
			}
			playerStats[i].playertype = playertype;
			if (playerStats[i].playertype == 1) { // only set name for human players
				cout << "Enter name for Player " << i << ": ";
				cin >> playerName;
				playerStats[i].playerName = playerName;
			}
			else {
				playerStats[i].playerName = "Bot " + to_string(i);
			}

		}
	}
	void setSourceCell(int playerID, int srow, int scol) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].srow = srow;
		playerStats[playerID].scol = scol;
	}
	void setSource(int playerID, int srow, int scol, int sant) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].srow = srow;
		playerStats[playerID].scol = scol;
		playerStats[playerID].sant = sant;
	}
	void setTargetCell(int playerID, int trow, int tcol) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].trow = trow;
		playerStats[playerID].tcol = tcol;
	}

	void setTarget(int playerID, int trow, int tcol, int tant) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].trow = trow;
		playerStats[playerID].tcol = tcol;
		playerStats[playerID].tant = tant;
	}
	void setSourceAnts(int playerID, int sant) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].sant = sant;
	}
	void setTargetAnts(int playerID, int tant) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].tant = tant;
	}
	void setCellsOwned(int playerID, int count) {
		if (playerID < 1 || playerID >= numPlayers) {
			cout << "Invalid player ID." << endl;
			return;
		}
		playerStats[playerID].cells_owned = count;
	}
	//printers
	void delay(string msg, int num) const {//simple delay function
		cout << msg << " >";
		for (int i = 0; i < num; i++) {
			this_thread::sleep_for(chrono::seconds(1));
			cout << " >";
		}cout << "\n";
	}
	void printPlayerInfo() const {
		for (int i = 0; i < numPlayers; i++) {
			cout << "Player ID: " << playerStats[i].playerID << ", Name: " << playerStats[i].playerName << ", Type: ";
			if (playerStats[i].playertype == 0) {
				cout << "Reserved" << endl;
			}
			else if (playerStats[i].playertype == 1) {
				cout << "Human" << endl;
			}
			else {
				cout << "AI" << endl;
			}
		}
	}
	void PrintPlayerSummary() const {
		for (int i = 1; i < numPlayers; i++) {
			cout << playerStats[i].playerName << " owns " << playerStats[i].cells_owned << " cells." << endl;
		}
	}
	void PrintAll() const {
		for (int i = 0; i < numPlayers; i++) {
			cout << "Player ID: " << playerStats[i].playerID << ", Name: " << playerStats[i].playerName << ", Type: ";
			if (playerStats[i].playertype == 0) {
				cout << "Reserved" << endl;
			}
			else if (playerStats[i].playertype == 1) {
				cout << "Human" << endl;
			}
			else {
				cout << "AI" << endl;
			}
			cout << "Cells Owned: " << playerStats[i].cells_owned << endl;
			cout << "Source: (" << playerStats[i].srow << ", " << playerStats[i].scol << ") with " << playerStats[i].sant << " ants." << endl;
			cout << "Target: (" << playerStats[i].trow << ", " << playerStats[i].tcol << ") with " << playerStats[i].tant << " ants." << endl;
		}
	}
    // Count cells owned by each player

	void countCellsOwned(Field& game) {
        // Reset counts
        for (int i = 0; i < numPlayers; i++) {
            playerStats[i].cells_owned = 0;
        }
		// Count cells
        int rows = game.getRows();
        int cols = game.getCols();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int owner = game.getOwner(i, j);
                if (owner >= 0 && owner < numPlayers) {
                    playerStats[owner].cells_owned++;
                }
            }
        }
    }
	bool Win(Field& game) const {
		int row = game.getRows(), col = game.getCols(), winningNum = ((row * col) / 3) * 2;
		for (int i = 1; i < numPlayers; i++) {
			if (playerStats[i].cells_owned >= winningNum) {
				cout << playerStats[i].playerName << " wins the game!" << endl;
				return true;
			}
		}
		return false;
	}
	// function to reset source and target info after each turn
	void resetTurnInfo() {
		for (int i = 0; i < numPlayers; i++) {
			playerStats[i].srow = 0;
			playerStats[i].scol = 0;
			playerStats[i].trow = 0;
			playerStats[i].tcol = 0;
			playerStats[i].sant = 0;
			playerStats[i].tant = 0;
		}
	}
	int runGrowth(int playerNum, Field& game) {
		int growth = getSourceAnts(playerNum);
		int tempant = 0, trow = 0, tcol = 0;
		cout << "Running Growth sequence for " << getPlayerName(playerNum);
		cout << " has " << getSourceAnts(playerNum) << " ants to deploy. \n";
		cout << "Target Cell: " << getTargetRow(playerNum) << ", " << getTargetCol(playerNum);
		cout << " add " << getTargetAnts(playerNum) << " ants.\n";
		//do the work
		while (growth > 0) {
			trow = getTargetRow(playerNum);
			tcol = getTargetCol(playerNum);
			tempant = getTargetAnts(playerNum);
			growth -= tempant;
            tempant += game.getAnts(trow, tcol);//get current ants in cell
			game.setAnts(trow, tcol, tempant);//update ants in cell
			setSourceAnts(playerNum, growth);//update available ants
			setTargetAnts(playerNum, 0);//reset target ants
			if (growth > 0) {
				return false;//to loop until growth is empty
			}
			else { return true; }
		}
		return true;
	}

	int runAttack(int playerNum, Field& game) const {
		cout<<" Running Attack sequence:\n";
		int attack = getSourceAnts(playerNum);//selected by player
		int trow = getTargetRow(playerNum); int tcol = getTargetCol(playerNum);//selected by player
		int defend = game.getAnts(trow,tcol);//get ants from Field
		int srow = getSourceRow(playerNum); int scol = getSourceCol(playerNum);//selected by player
		int sOwner = game.getOwner(srow, scol); string sName = getPlayerName(sOwner);//from Player
		int tOwner = game.getOwner(trow, tcol); string tName = getPlayerName(tOwner);//From Player
		cout <<sName<< " attacking from cell: " << srow << ", " << scol << " with " << attack << " ants.\n";
		cout <<tName<< " defending from cell: " << trow << ", " << tcol << " with " << defend << " ants.\n";
		if (game.getOwner(trow, tcol) == 0) {//step into unowned cell
			game.setOwner(trow, tcol, playerNum);
			game.setAnts(trow, tcol, attack);
			return true;
		}
		else {
			srand((unsigned)time(0));//battle between players
			while (defend != 0 && attack != 0) {
				int atroll = (1 + rand() % 99);
				int defroll = (1 + rand() % 99);
				cout << "Att: " << attack << ", " << atroll << "| Def: " << defend << ", " << defroll << "\n";
				if (atroll > defroll) {//attack roll wins
					defend--;
				}
				else {//defend roll wins
					attack--;
				}
				delay("", 2);
				if (defend == 0) {//attacker wins battle
					cout << "Att: " << attack << " | Def: " << defend << "\n";
					game.setOwner(trow, tcol, playerNum);
					game.setAnts(trow, tcol, attack);
					return true;
				}
				else if (attack == 0) {//defender wins battle
					game.setAnts(trow, tcol, defend);
					return false;
				}
			}
		}
		return true;
	}

	int runMove(int playerNum, Field& game) const {
		cout << " Running Move sequence for " << getPlayerName(playerNum) << ".\n";
		int current = getTargetAnts(playerNum);
		int moving = getSourceAnts(playerNum);
		int r = getTargetRow(playerNum);
		int c = getTargetCol(playerNum);
		game.setAnts(r, c, current + moving);
		return true;
	}
};

