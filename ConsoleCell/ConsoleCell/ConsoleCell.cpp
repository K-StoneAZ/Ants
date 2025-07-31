// ConsoleCell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;


int main()
{	
	/*What does my game need to know and variable type:
	Is this a new or saved game:  bool
	If new then numbers of players:  int (min=2 max = 8)
	If saved then which game:  TBD
	*/

    /*What does my cell need to know and variable type:
	Relative position in the game field:  int (row, column)
	Is the cell owned by a player:  int (0 = unowned, 1-8 = player number)
	Is the cell a border cell:  bool
	Is the cell a corner cell:  bool
	Border location(s):  bool (top, bottom, left, right)
	Base strength of the cell relative to game field position:  int (0-8)
	Base number of ants int = 1
	Cout of all cells in the game field owned by this player:  int (0-100)
	*/

	/*What does my palyer cell need to know: and variable type:
	Ownership of this cell:  int (0-8 = player number) from my cell structure
	Relative position in the game field:  int (row, column)
	Is this cell a border cell:  bool from my cell structure
	Is this cell a corner cell:  bool from my cell structure
	What are the border locations(s):  bool (top, bottom, left, right) from my cell structure
	Neighboring cell locations:  int (tleft (r,c), tcenter (r,c), tright (r,c), left (r,c),
		right (r,c), bleft (r,c), bcenter (r,c), bright (r,c))
	Is the tleft cell a border cell:  bool from my cell structure
	Is the tcenter cell a border cell:  bool from my cell structure
	Is the tright cell a border cell:  bool from my cell structure
	Is the left cell a border cell:  bool from my cell structure
	Is the right cell a border cell:  bool from my cell structure
	Is the bleft cell a border cell:  bool from my cell structure
	Is the bcenter cell a border cell:  bool from my cell structure
	Is the bright cell a border cell:  bool from my cell structure
	Is the tleft cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the tcenter cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the tright cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the left cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the right cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the bleft cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the bcenter cell owned by a player:  int (0-8 = player number) from my cell structure
	Is the bright cell owned by a player:  int (0-8 = player number) from my cell structure
	Count of adjacent cells owned by this player exclusive of this cell:
		Based on the ownership of the neighboring cells:  int (0-8)
	Relative cell strength:  int (0-8) based on the base strength of this cell + 
		count adjacenct cells owned by this player
	Number of adjacent cells owned by other players: by player number (1-8)
	Count of adjacent cells owned by other players:  int (0-8) 
		based on the ownership of the neighboring cells
	Count of all cells in the game field owned by this player:  int (0 - 100) from my cell structure
	Number of ants in this cell:  int (0-1000)*/

	/*What does my game field array need to know and variable type.
	Size of the game field array 2 dimensions:  "fixed 10 by 10"
		int (rows, columns) fixed or variable
	Using cell structure (listed above)
	*/

}