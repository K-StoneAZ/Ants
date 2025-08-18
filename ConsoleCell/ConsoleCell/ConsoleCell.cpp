// ConsoleCell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits> // For numeric_limits
#include <vector> 
#include <string>
#include "Gamefield.h" // Include the Gamefield class header
#include "FieldCount.h" // Include the FieldCount class header
using namespace std;
int main()
{
	// Initialize random seed
	srand(static_cast<unsigned int>(time(0)));
	// Game initialization
	cout << "Welcome to ConsoleCell!" << endl;
	cout << "Initializing game environment..." << endl;
	// Game variables
	int numPlayers = 0;
	int skillLevel = -1; // 0 = easy, 1 = medium, 2 = hard, 3 = expert
	// Get number of players
	while (numPlayers == 0) {
		cout << "Enter number of players (2-8): ";
		cin >> numPlayers;
		if (numPlayers < 2 || numPlayers > 8) {
			cout << "Invalid number of players. Try again." << endl;
			numPlayers = 0; // Reset to allow re-entry
		}
	}
	// Get skill level
	while (skillLevel < 0) {
		cout << "Select skill level (0 = easy, 1 = medium, 2 = hard, 3 = expert): ";
		cin >> skillLevel;
		if (skillLevel < 0 || skillLevel > 3) {
			skillLevel = -1; // Reset to allow re-entry
		}
	}
	// FUTURE DEVELOPMENT: Add more game environment variables here

	// Initialize game environment
	cout << "Game environment initialized with ";
	cout << numPlayers << " players and skill level " << skillLevel << "." << endl;
	cout << "Starting game..." << endl;

	//convert numPlayers to a constant to use in the rest of the game
	const int playerNum = numPlayers + 1; // used to initialize max owner number

	// Initialize the game field

	Gamefield game(10, 10); // Create a game field with default size 10x10
	// Set initial cell ownership and ants for each player
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) { // Example: Set first 10 cells for each player
			game.setCell(i, j, 0 + (rand() % playerNum)); // Set owner
		}
	}
	FieldCount myCount(playerNum); // Create an object of player cells
	myCount.countCells(game); // Count cells owned by each player
	cout << "          RULES:\n";
	cout << "1. Each player starts with a random number of cells with one ant in each cell.\n";
	cout << "2. Each player begins their turn by placing ants in their owned cells.\n";
	cout << "3. Player may then attack, move, or skip their turn.\n";
	cout << "4. If attack is choosen. Upon completion the turn will move to the next player.\n";
	cout << "5. If move is choosen. Upon completion the turn will move to the next player.\n";
	cout << "6. If skip is choosen. The turn will move to the next player.\n";
	cout << "7. The game ends when one player owns all cells.\n";
	cout << "\n          GOOD LUCK!\n";
	//set up game loop
	bool gameWin = false; // Flag to check if the game is won
	bool nextTurn = false; // Flag to control turn progression
	// Game loop to handle player turns and game logic
	while (!gameWin) { // Continue until a player wins
		//turn loop begin
		nextTurn = false;
		while (!nextTurn) { // Loop for each player's turn
			//things to do at the begining of every turn
			nextTurn = false; // reset nextTurn to false for the next player's turn
			// Initialize player cell count array "myCount" in FieldCount class
			gameWin = myCount.gameWin(); // Check if any player has won
			if (gameWin) {
				cout << "Game Over! A player has won!" << endl;
				break; // Exit the turn loop if a player has won
			}//if player 1 has zero cells then game is over.
			if (myCount.getPlayerCellCount(1) == 0) {
				gameWin = true; // Set gameWin to true to exit the loop
				break; // Exit the loop if player 1 has no cells
			}
			// Loop through players for turn completion
			for (int player = 1; player < playerNum; ++player) { // Loop through players
				game.setCellAttributes(playerNum);//update attributes for each player
				myCount.countCells(game);
				if(myCount.getPlayerCellCount(player) == 0) {
					continue; // Skip turn for players with no cells
				}
				else {// Print the game field and stats
					cout << "\n";
					game.printGamefield();
					cout << "Stats: \n";
					myCount.printCounts();
				}
				// automatic growth based on cells owned
				int growth = 0, tempAnt = 0; // Initialize growth variable
				growth = myCount.getGrowth(player);
				// Get player input for cell selection
				while (growth > 0) { // Loop until all ants are placed
					int row = 0, col = 0, amount = 0; bool validPlayer = false;//Local Variables
					game.printPlayerField(player);
					while (!validPlayer) { // Loop until valid player input
						cout << "Player " << player << " you have " << growth << " ants to place." << endl;
						cout << "Select a cell to place ant(s) (row col): ";
						cin >> row >> col; // Get input from player
						if (game.getIsCorner(row, col) > 0) {
							cout << "This is a corner cell.\n";
						}
						if (game.getIsEdge(row, col) > 0) {
							cout << "This is an edge cell.\n";
						}
						if (game.getStrength(row, col) == 8) {
							cout << "This cell is surrounded by your cells.";
						}
						if (game.getOwner(row, col) != player) {
							cout << "You must own the cell. Try again." << endl;
							continue; // Skip to the next iteration of the loop
						}
						else {
							// get amount of ants to place
							amount = 0;
							while (amount < 1) { // Loop until valid
								cout << "You have " << growth << " ants to place." << endl;
								cout << "Enter the number of ants to place in this cell: ";
								cin >> amount;
								if (amount > growth) {
									cout << "Too many ants! Retry: ";
									amount = 0; // Reset amount to allow re-entry
								}
								else if (amount < 1) {
									cout << "You must place at least one ant. Retry: ";
									amount = 0; // Reset amount to allow re-entry
								}
								else {
									tempAnt = game.getAnts(row, col); // Get current ants
									tempAnt += amount; // Add ants to the cell
									game.setAnts(row, col, tempAnt); // Update cell
									growth -= amount; // Decrease growth by ants added
									validPlayer = (growth <= 0); //end the loop
								}
							}
							cout << "\n";
							game.printPlayerField(player);
						}
					}
				}// End of growth loop.  Make this a function in gamefield class.
				cout << "\n\n\n\n\n";
				game.printGamefield();
				cout << "Stats: \n";
				myCount.printCounts(); // Print counts of cells owned by each player
				bool endAttack = false;
			// Attack, Move, or Skip turn console player only
			cout << "Would you like to Attack, Move, or Skip your turn? (A/M/S): ";
			char action = ' ';
			while (action != 'A' && action != 'M' && action != 'S') {
				cin >> action; // Get action from player
				action = toupper(action); // Convert to uppercase for consistency
			}
			if (action == 'A') { // Attack action
				// Variables for cells
				int targetRow = 0, targetCol = 0, row = 0, col = 0, attAnt = 0, defAnt = 0;
				// Flag for valid cells
				bool  validTarget = false, validSource = false, validAnts = false;
				bool Battle = false;
				while (!endAttack) {
					// Attack logic begins here
					while (!validSource) { // Loop until valid source cell
						cout << "Enter the row and column of the cell to attack from: ";
						cin >> row >> col; // Get source cell from player
						if (game.getOwner(row, col) != player) { // Check if the cell is owned by the player
							cout << "You cannot attack from a cell you don't own." << endl;
							continue;// Skip to the next iteration of the loop
						}
						else if (action == 'A' && game.getStrength(row, col) == 8) { // Check an edge
							cout << "This cell is surrounded by your cells.";
							continue;// Skip to the next iteration of the loop
							}
						else if (game.getAnts(row, col) < 1) {
							cout << "You must have at least 2 ants in the cell.  Retry";
							continue; // Skip to the next iteration of the loop
						}
						else {
							game.printPlayerCellAttribs(row, col, player); // Print cell attributes
							validSource = true; // Valid source cell, exit loop
						}
					}// End of valid source cell loop
					//must evaluate cell strength to prevent attacking from a cell that is 
					// surrounded by cells owned by the same player
					while (!validAnts) { // Loop until valid number of ants
						attAnt = game.getAnts(row, col) - 1; // Get number of ants in the source cell
						cout << "You have " << attAnt << " ants to attack with." << endl;
						int tempAnt = 0; // Temporary variable
						while (tempAnt == 0) { // Loop until valid number of ants to attack with
							cout << "Enter the number of ants to attack with (1-" << attAnt << "): ";
							cin >> tempAnt; // Get number of ants to attack with
							if (tempAnt <= 0 || tempAnt > attAnt) { // Check if the input is valid
								cout << "Invalid number of ants. Retry: ";
								tempAnt = 0; // Reset tempAnt
							}
							else {// Update number of ants in the source cell
								attAnt = game.getAnts(row, col);
								attAnt -= tempAnt; // Set attack ants
								game.setAnts(row, col, attAnt); // Update source ants
								attAnt = tempAnt;
								validAnts = true; // Valid number of ants, exit loop
							}
						}
					}//end of valid ants loop
					while (!validTarget) { // Loop until valid target cell
						cout << "Enter the row and column of the cell to attack: ";
						cin >> targetRow >> targetCol; // Get target cell from player
						// Check if the target cell is within bounds
						if (targetRow < 0 || targetRow >= game.getRows() || targetCol < 0 || targetCol >= game.getCols()) {
							cout << "Target cell is out of bounds. Retry.";
						} // Check if the target is the same source cell
						else if (targetRow == row && targetCol == col) {
							cout << "You cannot attack the cell you are attacking from. Retry." << endl;
						} // Check within range
						else if (targetRow <= (row - 2) || targetRow >= (row + 2) || targetCol <= (col - 2) || targetCol >= (col + 2)) {
							cout << "Target is to far away.  Retry." << endl;
						} // Check if the cell is owned by the player
						else if (game.getOwner(targetRow, targetCol) == player) {
							cout << "You cannot attack your own cell. Retry." << endl;
						}
						else {
							validTarget = true; // Valid target cell, exit loop	
							defAnt = game.getAnts(targetRow, targetCol); // Get number of ants
						}
					}
					// Battle logic begins here
					while (!Battle) { // Loop until battle is resolved
						// Check if the target cell is unowned
						if (game.getOwner(targetRow, targetCol) == 0) {
							game.setOwner(targetRow, targetCol, player);//Set owner to player
							game.setAnts(targetRow, targetCol, attAnt); //Set ants 
							Battle = true; // End of Battle
							endAttack = true; //end Attack logic
						}
						else { //target cell is owned
							int tempAttant = attAnt; // Temporary variable for total attack ants
							while (defAnt != 0 && attAnt != 0) { // Loop until zero ants
								//generate random number for attack and defense
								int attRoll = rand() % (101); // Random roll
								int defRoll = rand() % (101); // Random roll
								if (attRoll <= defRoll) { // If attack is greater
									attAnt--; // Reduce attack ants
								}
								else { // If defense is greater or equal
									defAnt--; // Reduce defense ants
								}
								if (defAnt == 0) { // If the target cell has no ants left
									game.setOwner(targetRow, targetCol, player); // Set owner
									game.setAnts(targetRow, targetCol, attAnt); // Set ants
									Battle = true; // End of battle loop
									endAttack = true; // End of attack logic
									defAnt = 0; // Reset defense ants
								}
								if (attAnt == 0) {
									game.setAnts(targetRow, targetCol, defAnt); //Set remaining
									Battle = true; // End of battle loop
									endAttack = true;// End of attack logic
									attAnt = 0; // Reset attack ants
								}
							}
						}
					}
					cout << "Attack completed for Player " << player << "\n\n\n\n\n";
					action = ' '; // Reset action for next player
				}
			}

			else if (action == 'M') { // Move action
				int targetRow = 0, targetCol = 0, row = 0, col = 0, attAnt = 0, defAnt = 0; // Variables for cells
				bool validTarget = false, validSource = false, validAnts = false; // Flag for valid cells
				game.printPlayerField(player); // Print player field
				// Move logic begins here
				while (!validSource) { // Loop until valid move source cell
					cout << "Enter the row and column of the cell to move from: ";
					cin >> row >> col; // Get move source cell from player
					attAnt = game.getAnts(row, col) - 1;
					if (game.getOwner(row, col) != player) { // cell is owned by this player
						cout << "You cannot move from a cell you don't own.  Retry:";
						continue;// Skip to the next iteration of the loop
					}
					else if (attAnt < 1) { // Check if the cell has ants
						cout << "You must have at least 2 ants in the cell. Retry: ";
						continue; // Skip to the next iteration of the loop
					}
					else {
						game.printPlayerCellAttribs	(row, col, player); // Print cell attributes
						validSource = true; // Valid source cell, exit loop
					}
				}// Move
				while (!validAnts) { // Loop until valid number of ants
					attAnt = game.getAnts(row, col) - 1; // Get number of ants in the source cell
					cout << "You have " << attAnt << " ants available to move." << endl;
					int tempAnt = 0; // Temporary variable
					while (tempAnt < 1 ) { // Loop until valid number of ants
						cout << "Enter the number of ants to move (1-" << attAnt << "): ";
						cin >> tempAnt; // Get number of ants
						if (tempAnt < 1 || tempAnt > attAnt) { // Check if the input is valid
							tempAnt = 0; // Reset tempAnt
							cout << "Invalid number of ants. Retry: ";
							continue; // Skip to the next iteration of the loop
						}
						else {
							attAnt = tempAnt; // Set the number of ants to move
							validAnts = true; // Valid number of ants, exit loop
						}
					}//end Move tempAnt loop
				}//end of valid ants loop
				while (!validTarget) { // Loop until valid move target cell
					cout << "Enter the row and column of the cell to move to: ";
					cin >> targetRow >> targetCol; // Get target cell from player
					// Check if the target cell is within bounds
					if (targetRow < 0 || targetRow >= game.getRows() || targetCol < 0 || targetCol >= game.getCols()) {
						cout << "Target cell is outside of game field.";
						continue; // Skip to the next iteration of the loop
					}// Check if the target cell is owned by the player
					else if (game.getOwner(targetRow, targetCol) != player) { // Check if the cell is owned by the player
						cout << "You cannot move to a cell you don't own.";
						continue; // Skip to the next iteration of the loop
					}// Check if the target is the same source cell
					else if (targetRow == row && targetCol == col) {
						cout << "You must move to a different cell.";
						continue; // Skip to the next iteration of the loop
					} // Check within range
					else if (targetRow < (row - 2) || targetRow >(row + 2) || targetCol < (col - 2) || targetCol >(col + 2)) {
						cout << "Cell is to far away.";
						continue; // Skip to the next iteration of the loop
					}
					else {// Move logic
						int currentAnts = game.getAnts(targetRow, targetCol); // Get number of ants in the target cell
						currentAnts = currentAnts + attAnt; // Add ants to the target cell
						game.setAnts(targetRow, targetCol, currentAnts); // Update number of ants in the target cell
						int tempAnt = game.getAnts(row, col); // Get number of ants in the source cell
						tempAnt = tempAnt - attAnt; // Subtract ants from the source cell
						game.setAnts(row, col, tempAnt); // Update number of ants in the source cell
						validTarget = true; // Valid target cell, exit loop
					}// End validTarget loop
				}//end of move logic loop
			}// End of move action
			else if (action == 'S') { // Skip turn action
				cout << "Skipping turn." << endl;
			}
			else {
				cout << "Invalid action. Please enter A, M, or S." << endl;
			} // End of console player turn loop
			//begin AI player(s) turn
		}
		//Next turn : bool(true = next turn, false = game win)
		nextTurn = true; // Set to true to move to next game turn
	}


			// FUTURE DEVELOPEMENT
	/*What does the game graphics environment need to know and variable type:
	Window size:  int (width, height)
	Window title:  string
	Window background color:  string (color name or hex code)
	Window border color:  string (color name or hex code)
	Window border width:  int (in pixels)
	Window border style:  string (solid, dashed, dotted, etc.)
	Window font:  string (font name)
	Window font size:  int (in pixels)
	Window font color:  string (color name or hex code)
	Window font style:  string (normal, bold, italic, etc.)

	Saving and recalling a game
	Game complexity:  int (0-3) (0 = easy, 1 = medium, 2 = hard, 3 = expert)
		added features like rocks, water, etc.
	Sound effects enabled:  bool

	game win:  bool (true = game win, false = next turn)

	*/
	// AI player strategy
/*What is the strategy for the AI:
In all skill levels the AI will exclude any owned cells with a cell strength of 8.

The first step is to determine the selected skill level from the game environment.

If the skill is easy then the AI player will randomly select an owned cell.

If the skill is medium then the AI will select a random owned cell that has a cell strength of 3-7
	Once game play has progressed the AI will select a random owned cell that has a cell strength > 7

If the skill is hard then the AI will select a random owned cell that is a corner or border and
		has a strength of 5-7

If the skill is expert then the AI will select an owned cell that:
	The goal of the expert AI is to develope a colony of cells that have
		the least vulnerabity.
	The starting cell will be selected based on the following criteria:
	Is a corner and has a cell strength >5 and <7
	Or is a border and has a cell strength >3 and atleast 1 unowned adjacent cell.
	Or has the highest cell strength and has >1 unowned adjacent cell.
	Or has the highest cell strength and has adjacent cells owned by >2 other players.
		As play continues this AI will select a cell that is on the edge of the colony.
*/
	}	// End of gamewin loop
	//congratulation winner message
	cout << "Congratulations! The game has ended." << endl;
	//new game option
	cout << "Would you like to play again? (Y/N): ";
	char playAgain = ' ';
	std::cin >> playAgain; // Get input from player
	if (toupper(playAgain) == 'Y') {
		game.destroyGamefield(); // Destroy the game field
		myCount.destroyCellCount(); // Destroy the field count
		main(); // Restart the game
	} else {
		cout << "Thank you for playing ConsoleCell!" << endl; // Exit message
	}

}