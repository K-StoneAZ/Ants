#pragma once
#include <vector>
#include <iostream>
#include <cstdlib>
#include "Field.h"

using namespace std;

class Count
{
protected:
	vector<int> playerCellCount;//vector to count cells owned by player
public:
	Count(int numPlayer = 9) : playerCellCount(numPlayer, 0) {}//initialize with default

	void countCells(const Field& game) {
		size_t player = playerCellCount.size();
		int rows = game.getRows();
		int cols = game.getCols();
		for (int i = 0; i < player; i++) {
			playerCellCount[i] = 0; //set counts to 0
		}
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				int owner = game.getOwner(row, col);
				if (owner >= 0 && owner < player) {
					playerCellCount[owner]++; //increment player cell count
				}
			}
		}
	}
	void printCount() const {
		for (int i = 0; i < playerCellCount.size(); i++) {
			cout << " | Player " << i << ", " << playerCellCount[i];
		}
		cout << " |\n";
	}
	int getPlayerCount(int player) const {
		if (player >= 0 && player < playerCellCount.size()) {
			return playerCellCount[player];
			}
		}
	bool GameWin() const {
		for (int i = 0; i < playerCellCount.size(); i++) {
			if (playerCellCount[i] == 66) {//rework this to be 66% of total cells
				cout << "Player " << i << " has won the game!\n";
				return true;
			}
			else { return false; }
		}
	}
	int getGrowth(int player) const {
		if (player >= 0 && player < playerCellCount.size()) {
			return 1 + (playerCellCount[player] / 3); 
		}
		else { return 0; }
	}
	void destroyCount() {
		playerCellCount.clear();
	}
};

