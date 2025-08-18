#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "Gamefield.h" // Include the Gamefield class header
using namespace std;

// Create a class to get input from the console.
// Develope source and target location per player within the game field array
// and the number of ants to move or attack with.


class Console
{
	private:
		struct Data {// Define a struct to hold console-related data
			int Srow; // Source row
			int Scol; // Source column
			int Trow; // Target row
			int Tcol; // Target column
			int ants; // Number of ants to move or attack with
			char action; // Action type (A for attack, M for move, S for skip)
		};
		Data data; // Add a nonstatic data member of type Data
	public:
		// Constructor needs structure Data to initialize source and target cells
		Console() : data{ 0, 0, 0, 0, 0, ' ' } {} // Default initializes members to 0 or empty
		//methods to get and set each member of the struct
	int setSource(int row, int col) {
			data.Srow = row; // Set source row
			data.Scol = col; // Set source column
	}
	// get source row and column
	int getSourceRow() const {
			return data.Srow; // Return source row
	}
	int getSourceCol() const {
			return data.Scol; // Return source column
	}
	int setTarget(int row, int col) {
			data.Trow = row; // Set target row
			data.Tcol = col; // Set target column
	}// get target row and column	
	int getTargetRow() const {
		return data.Trow; // Return target row
	}
	int getTargetCol() const {
		return data.Tcol; // Return target column
	}
	int setAction(char act) {
		data.action = act; // Set action type
	}
	int setAnts(int num) {
		data.ants = num; // Set number of ants to move or attack with
	}		
		// Method to get player input for source cell selection
	int getSource(Gamefield& game, int player, int action) {
		int Srow = 0, Scol = 0;
		bool validSource = false;
		while (!validSource) {
			game.printPlayerField(player);
			cout << "Player " << player << ", select a cell to place ant(s) (row, col): ";
			cin >> Srow >> Scol; // Get input from player
			if (game.getStrength(Srow, Scol) == 8 && action == 'A') {
				cout << "This cell is surrounded by your cells.\n";
			}
			else if (game.getAnts(Srow, Scol) < 2) {
				cout << "You must have at least 2 ants in the cell." << endl;
			} 
			else if (Srow < 0 || Scol < 0 || Srow >= game.getRows() || Scol >= game.getCols()) {
				cout << "Invalid cell coordinates. Try again." << endl;
			}
			else if (game.getOwner(Srow, Scol) != player) {
				cout << "You must own the cell. Try again." << endl;
			}
			else {
				setSource(Srow,Scol); // Set source row
				validSource = true; // Valid selection made
				// Return valid selection
			}
		}return 1; // Return 1 to indicate success
	}
	// Method to get action from the player
	char getAction() {
		char action = ' ';
		cout << "Would you like to Attack, Move, or Skip your turn? (A/M/S): ";
		while (action != 'A' && action != 'M' && action != 'S') {
			cin >> action; // Get action from player
			action = toupper(action); // Convert to uppercase for consistency
		}
		setAction(action); // Set action type
		return 1;
	}
	//Method to get the number of ants to move or attack with
	int getAnts(Gamefield& game, int Srow, int Scol, int action) {
		int ants = 0;
		int maxAnts = game.getAnts(Srow, Scol) - 1; // Get maximum ants available
		while (ants < 1 || ants > maxAnts) { // Loop until valid number of ants
			cout << "You have " << maxAnts << " ants available to " << (action == 'A' ? "attack" : "move") << "." << endl;
			cout << "Enter the number of ants to " << (action == 'A' ? "attack with" : "move") << " (1-" << maxAnts << "): ";
			cin >> ants; // Get number of ants from player
			if (ants < 1 || ants > maxAnts) {
				cout << "Invalid number of ants. Retry." << endl; // Invalid input
			}
		}
		setAnts(ants); // Set number of ants
		return 1; // Return valid number of ants
	}
	// Method to get target cell from the player
	int getTarget(Gamefield& game, int player, int Srow, int Scol, int action) {
		int Trow = 0, Tcol = 0;
		bool validTarget = false;
		while (!validTarget) {
			cout << "Enter the row and column of the cell to " << (game.getOwner(Srow, Scol) == player ? "attack" : "move") << ": ";
			cin >> Trow >> Tcol; // Get target cell from player
			if (Trow < 0 || Trow >= game.getRows() || Tcol < 0 || Tcol >= game.getCols()) {
				cout << "Target cell is out of bounds. Retry." << endl; // Out of bounds
			} 
			else if (Trow == Srow && Tcol == Scol) {
				if (action =='a') {
					cout << "You cannot attack from the same cell. Retry." << endl; // Same cell for attack
				} else {
					cout << "You cannot move to the same cell. Retry." << endl; // Same cell for move
				}
			} 
			else if (game.getOwner(Trow, Tcol) != player && action =='M') {
				cout << "You cannot move to a cell you don't own. Retry." << endl; // Not owned by player
			} 
			else {
				validTarget = true; // Valid target cell
				setTarget(Trow, Tcol); // Set target row and column
				return 1; // Return valid target cell
			}
		}
	}
};

