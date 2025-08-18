#pragma once
#include <vector>
#include <iostream>
#include <cstdlib>
#include "Gamefield.h"
using namespace std;

//create class FieldCount
//function to count cells owned by each player that returns a vector of counts for all players
//int playerCellCount[9] = { 0 }; Array to count cells owned by each player (0-8)
//	int i = 0; Loop counter for players
//	while (i < playerNum) {
//		for (int row = 0; row < 10; ++row) {
//			for (int col = 0; col < 10; ++col) {
//				int owner = game.getOwner(row, col);
//				if (owner == i) {
//					playerCellCount[i]++; // Increment count for the player
//				}
//			}
//		}
//		i++; // Move to the next player
//	}
// 

class FieldCount
{
	private:
		vector<int> playerCellCount; // Vector to count cells owned by each player (0-8)
public:
	FieldCount(int numPlayers = 9) : playerCellCount(numPlayers, 0) {} // Initialize with default size 9 (0-8 players)

	void countCells(const Gamefield& game) {
        size_t playerNum = playerCellCount.size();
		int rows = game.getRows();
		int cols = game.getCols();
		for (int i = 0; i < playerNum; ++i) {
			playerCellCount[i] = 0; // Reset count for each player
		}
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < cols; ++col) {
				int owner = game.getOwner(row, col);
				if (owner >= 0 && owner < playerNum) {
					playerCellCount[owner]++; // Increment count for the player
				}
			}
		}
	}
	void printCounts() const {
		for (int i = 0; i < playerCellCount.size(); ++i) {
			cout << " | Player " << i << ", "<< playerCellCount[i];
		} cout << " |" << endl;
	}
	//setters and getters for playerCellCount
	void setPlayerCellCount(int player, int count) {
		if (player >= 0 && player < playerCellCount.size()) {
			playerCellCount[player] = count;
		}
	}
	int getPlayerCellCount(int player) const {
		if (player >= 0 && player < playerCellCount.size()) {
			return playerCellCount[player];
		}
		return -1; // Invalid player index
	}
	//gamewin function
	// Returns Gamewin = true if a player has won (i.e., owns all cells), false otherwise
	bool gameWin() const {
		for (int i = 0; i < playerCellCount.size(); ++i) {
			if (playerCellCount[i] == 100) { // Assuming a 10x10 grid, total cells = 100
				cout << "Player " << i << " has won the game!" << endl;
				return true; // Player has won
			}
		}
		return false; // No player has won yet
	}
	int getGrowth(int player) const {
		if (player >= 0 && player < playerCellCount.size()) {
			return 1 + (playerCellCount[player] / 3); // Growth based on cells owned
		}
		return 0; // Invalid player index or no growth
	}
	//function to destroy field count
	void destroyCellCount() {
		playerCellCount.clear(); // Clear the vector to free memory

	}
};

