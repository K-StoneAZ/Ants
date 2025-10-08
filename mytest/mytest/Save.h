#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include "Field.h"
#include "Player.h"

using namespace std;
class Save
{
protected:
	vector<string> saveString;
	vector<string> loaddata;

public:
	static string getGamedata(Field& game, Player& player, int turncount) {
		int numplayers = player.getNumPlayers();
		string gamesize;
		int size = game.getRows();
		if (size == 10) { gamesize = "1"; }
		else if (size == 12) { gamesize = "2"; }
		else { gamesize = "3"; }
		return gamesize + " " + to_string(numplayers) + " " + to_string(turncount) + "\n";

	}
	string getPlayerData(Player& player) {
		string playerdata = player.getSaveString();
		return playerdata;
	}
	string getFieldData(Field& game) {
		string fielddata = game.getSaveString();
		return fielddata;
	}
	void assembleSaveString(Field& game, Player& player, int turncount) {
		saveString.resize(3);
		string gamedata, playerdata, fielddata;
		gamedata = getGamedata(game, player, turncount);
		playerdata = getPlayerData(player);
		fielddata = getFieldData(game);
		saveString[0] = gamedata;
		saveString[1] = playerdata;
		saveString[2] = fielddata;
	}
	int writeSaveFile() {
		ofstream outf{ "antsave.txt" };
		// If we couldn't open the output file stream for writing
		if (!outf)
		{
			// Print an error and exit
			cerr << "Uh oh, antsave.txt could not be opened for writing!\n";
			return 1;
		}
		for (int i = 0; i < saveString.size(); i++) {
			outf << saveString[i];
		}
		outf.close();// write complete close file
		return 0;
	}
	//load game functions
	vector<string> readSaveFile() {
		ifstream inf{ "antsave.txt" };
		// If we couldn't open the input file stream for reading
		if (!inf.is_open())
		{
			// Print an error and exit
			std::cerr << "Uh oh, antsave.txt could not be opened for reading!\n";
			return vector<string>();
		}
		else {
			// While there's still stuff left to read and populate the data vector
			string strInput;
			while (std::getline(inf, strInput)) {
				loaddata.push_back(strInput);
			}
			inf.close();// read complete close file
			return loaddata;
		}
	}
	string getGameData() const {
		return loaddata[0];
	}
	string getPlayerData() const {
		return loaddata[1];
	}
	string getFieldData() const {
		return loaddata[2];
	}
};

