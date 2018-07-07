#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

void cout_grid(int (&Grid)[4][4]);
bool rand_tile(int* GridPtr);
bool move_up(int (&Grid)[4][4]);
bool move_down(int (&Grid)[4][4]);
bool move_left(int (&Grid)[4][4]);
bool move_right(int (&Grid)[4][4]);
bool move_test(const int* GridPtr);


int main() {
	//Initialize Variables
	//Array
	int Grid[4][4]; //4x4 board
	//Pointer
	int* GridPtr = *Grid; //Pointer for initialising the grid.
	//Character;
	char Move; //The last move (w, s, a or d) entered by the user.
	//String
	string ConfigName; //File-name of the initial configuration file.
	//Boolean
	bool IsMoved; //Did any tile move?
	bool GameOver = false; //Used for the main while loop.
	//File-stream
	ifstream ConfigStream; //File-stream of the initial configuration file.

	srand((unsigned int) time(NULL)); //Initialise random seed by getting current time converted to unsigned int.

	cout << "-----------------------" << endl;
	cout << "|    -=2048 v1.5=-    |" << endl;
	cout << "-----------------------\n" << endl;

	cout << "Please enter the initial configuration file-name:" << endl; 
	cin >> ConfigName;

	//Initialise Grid
	ConfigStream.open(ConfigName.c_str());
	if (!ConfigStream.is_open()) {
		cout << endl << "File not found, using default start configuration." << endl;
		for (int i = 0; i < 15; i++) { *(GridPtr + i) = 0; }
		*(GridPtr + 15) = 2;
	}
	else { 
		while (ConfigStream >> *(GridPtr++)); //GridPtr is incremented after value assign.
		GameOver = !move_test(*Grid); //Ensures that the grid copied from ConfigStream is playable.
	}
	ConfigStream.close(); //Close the file-stream as it is no longer required.

	cout_grid(Grid);
	
	while (!GameOver) {
		cin >> Move;

		if (Move == 'w') { IsMoved = move_up(Grid); }
		else if (Move == 's') { IsMoved = move_down(Grid); }
		else if (Move == 'a') { IsMoved = move_left(Grid); }
		else if (Move == 'd') { IsMoved = move_right(Grid); }
		else { IsMoved = false, cout << "Invalid Move - Characters allowed are: 'w', 's', 'a' & 'd'" << endl; }

		if (IsMoved) { 
			if (!rand_tile(*Grid)) { GameOver = !move_test(*Grid); } //Add a tile and check for move possibility if grid is full.
			cout_grid(Grid); //Output the grid, even when GameOver is true.
		}
	}

	cout << "Game Over!" << endl; //This line will only be executed when while loop exits.

	return 0;
}

void cout_grid(int (&Grid)[4][4]) {
	cout << endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			cout << Grid[i][j] << "\t";
		}
		cout << Grid[i][3] << endl;
	}
	cout << endl;
}

bool rand_tile(int* GridPtr) {
	//GridPtr = The address of the very first element in Grid
	int Zeros = 0, RandNo; //Return a random value between 1 - Max
	vector<int> ZerosIndex;
	
	//Only single loop required for pointers
	for (int i = 0; i < 16; i++) { if (*(GridPtr + i) == 0) { Zeros++, ZerosIndex.push_back(i); } }
	RandNo = rand() % Zeros + 1;
	*(GridPtr + ZerosIndex[RandNo - 1]) = 2; //Dereference the correct elem from GridPtr and make it = 2
	Zeros--; //One zero (empty) tile is now occupied

	return (Zeros > 0); //If no more empty tiles return false, else return true
}

bool move_up(int (&Grid)[4][4]) {
	/*
	i = RowIndex, j = ColumnIndex
	Moves -> Counts the total number of moves (merges).
	TopMost -> The index of the topmost tile, equals to 0 if it is NOT a merged tile (compliments 'IsDoubled').
	IsMoveValid -> As soon as the [i]th tile has been moved, the '[h] for loop' can exit because [i] cannot move again.
	IsDoubled -> Set to true when a tile merges with another tile of same value. It then cannot  be merged again.
	*/
	int Moves = 0, TopMost; 
	bool IsMoveValid = true, IsDoubled;

	for (int j = 0; j < 4; j++) {
		IsDoubled = false; //Reset this for every [j] - column but not row
		TopMost = 0; //Reset this for every [j] - column but not row

		//Start from i = 1 because first element cannot be checked by any other
		for (int i = 1; i < 4; i++) {
			IsMoveValid = true;

			//Only compare Grid[i] if non-zero
			if (Grid[i][j] != 0) {
				for (int h = (i - 1); h >= 0 && IsMoveValid; h--) {
					
					//Get the first non-zero if-doubled OR (non-zero & non-same) tile if-not-doubled
					if (IsDoubled ? (Grid[h][j] != 0) : (Grid[h][j] != 0 && Grid[h][j] != Grid[i][j])) {
						
						//Add tile [i] to the tile below tile [h] (which will be either 0 or same but not [i] itself)
						if ((h + 1) != i) {
							//If [h+1]=[i] -> IsDoubled==true || If [h+1]==0 -> IsDoubled=false
							IsDoubled = (Grid[h + 1][j] > 0);
							Grid[h + 1][j] += Grid[i][j];
							Grid[i][j] = 0;
							Moves++;
						}
						//Once a move took place or [h+1] == [i], there are no more valid moves left
						IsMoveValid = false; 
					}
					//Special case 4488 -> 8088 -> 8808 -> 8(16)00
					else if (h > 0 && Grid[h][j] == Grid[i][j]) { TopMost = h; }
				}

				//If tile not moved but the adjacent top tile = 0 or same then its an exception i.e. valid move left
				if (IsMoveValid) {
					IsDoubled = (Grid[TopMost][j] > 0); //Important for certain cases
					Grid[TopMost][j] += Grid[i][j];
					Grid[i][j] = 0;
					Moves++;
				}
			}
		}
	}
	
	return (Moves > 0);
}
bool move_down(int (&Grid)[4][4]) { 
	int Moves = 0, BottomMost;
	bool IsMoveValid = true, IsDoubled;

	for (int j = 0; j < 4; j++) {
		IsDoubled = false; //Reset this for every [j] - column but not row
		BottomMost = 3; //Reset this for every [j] - column but not row

		//Start from i = 2 because last element cannot be checked by any other
		for (int i = 2; i >= 0; i--) {
			IsMoveValid = true;

			if (Grid[i][j] != 0) {
				for (int h = (i + 1); h < 4 && IsMoveValid; h++) {
					//Get the first non-zero if-doubled OR (non-zero & non-same) tile if-not-doubled
					if (IsDoubled ? (Grid[h][j] != 0) : (Grid[h][j] != 0 && Grid[h][j] != Grid[i][j])) {
						//The tile above tile-[h] must not be tile-[i] itself
						if ((h - 1) != i) {
							IsDoubled = (Grid[h - 1][j] > 0); //If [h-1]==[i] -> IsDoubled=true || If [h-1]==0 -> IsDoubled=false
							Grid[h - 1][j] += Grid[i][j];
							Grid[i][j] = 0;
							Moves++;
						}
						IsMoveValid = false; //Once a move took place or [h-1] == [i], there are no more valid moves left
					}
					else if (h < 3 && Grid[h][j] == Grid[i][j]) { BottomMost = h; } //Bottom most tile is the lowest tile that [i] can merge to. 
				}

				if (IsMoveValid) {
					IsDoubled = (Grid[BottomMost][j] > 0);
					Grid[BottomMost][j] += Grid[i][j];
					Grid[i][j] = 0;
					Moves++;
				}
			}
		}
	}

	return (Moves > 0);
}
bool move_left(int (&Grid)[4][4]) { 
	int Moves = 0, LeftMost; 
	bool IsMoveValid = true, IsDoubled;

	for (int i = 0; i < 4; i++) {
		IsDoubled = false; //Reset this for every [i] - row but not column
		LeftMost = 0; //Reset this for every [j] - row but not column
		
		//Start from j = 1 because first element cannot be checked by any other
		for (int j = 1; j < 4; j++) {
			IsMoveValid = true;
			
			if (Grid[i][j] != 0) {
				for (int h = (j - 1); h >= 0 && IsMoveValid; h--) {
					//Get the first non-zero if-doubled OR (non-zero & non-same) tile if-not-doubled
					if (IsDoubled ? (Grid[i][h] != 0) : (Grid[i][h] != 0 && Grid[i][h] != Grid[i][j])) {
						if ((h + 1) != j) {
							IsDoubled = (Grid[i][h + 1] > 0); //If [h+1]==[j] -> IsDoubled=true || If [h+1]==0 -> IsDoubled=false
							Grid[i][h + 1] += Grid[i][j];
							Grid[i][j] = 0;
							Moves++;
						}
						IsMoveValid = false; //Once a move took place or [h+1] == [j], there are no more valid moves left
					}
					else if (h > 0 && Grid[i][h] == Grid[i][j]) { LeftMost = h; }
				}

				if (IsMoveValid) {
					IsDoubled = (Grid[i][LeftMost] > 0);
					Grid[i][LeftMost] += Grid[i][j];
					Grid[i][j] = 0;
					Moves++;
				}
			}
		}
	}
	
	return (Moves > 0);
}
bool move_right(int (&Grid)[4][4]) { 
	int Moves = 0, RightMost;
	bool IsMoveValid = true, IsDoubled;


	for (int i = 0; i < 4; i++) {
		IsDoubled = false; //Reset this for every [i] - row but not column
		RightMost = 3; //Reset this for every [j] - row but not column

		//Start from j = 1 because first element cannot be checked by any other
		for (int j = 2; j >= 0; j--) {
			IsMoveValid = true;

			if (Grid[i][j] != 0) {
				for (int h = (j + 1); h < 4 && IsMoveValid; h++) {
					//Get the first non-zero if-doubled OR (non-zero & non-same) tile if-not-doubled
					if (IsDoubled ? (Grid[i][h] != 0) : (Grid[i][h] != 0 && Grid[i][h] != Grid[i][j])) {
						if ((h - 1) != j) {
							IsDoubled = (Grid[i][h - 1] > 0); //If [h-1]==[j] -> IsDoubled=true || If [h-1]==0 -> IsDoubled=false
							Grid[i][h - 1] += Grid[i][j];
							Grid[i][j] = 0;
							Moves++;
						}
						IsMoveValid = false; //Once a move took place or [h-1] == [j], there are no more valid moves left
					}
					else if (h < 3 && Grid[i][h] == Grid[i][j]) { RightMost = h; }
				}

				if (IsMoveValid) {
					IsDoubled = (Grid[i][RightMost] > 0);
					Grid[i][RightMost] += Grid[i][j];
					Grid[i][j] = 0;
					Moves++;
				}
			}
		}
	}

	return (Moves > 0);
}

bool move_test(const int* GridPtr){
	int TestGrid[4][4];
	int* TestGridPtr = *TestGrid;
	const int* EndPtr = (GridPtr + 16); //Last elem in (GridPtr + 15), but loop doesn't run when GridPtr=EndPtr

	//Copy Array to temp array using pointers.
	while (GridPtr != EndPtr) { *(TestGridPtr++) = *(GridPtr++); }

	//If any move is possible, return true, otherwise return false.
	return (move_up(TestGrid) || move_down(TestGrid) || move_left(TestGrid) || move_right(TestGrid));
}