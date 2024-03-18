#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//just prints the board
void boardToString(char* board) {
	printf("%c %c %c %c \n", board[0], board[1], board[2], board[3]);
	printf("%c %c %c %c \n", board[4], board[5], board[6], board[7]);
	printf("%c %c %c %c \n", board[8], board[9], board[10], board[11]);
	printf("%c %c %c %c \n", board[12], board[13], board[14], board[15]);
	printf("\n");
}

//is used to split the board to more easy change into integers later
void splitBoard(char* board, char* cross_board, char* circle_board) {
	int i;
	for (i=0; i<16; i++) {
		if (board[i] == 'x') {
			cross_board[i] = 'x';
		}
		else {
			cross_board[i] = '-';
		}
		
		if (board[i] == 'o') {
			circle_board[i] = 'o';
		}
		else {
			circle_board[i] = '-';
		}
	}
}

//change the board-array representation to an int representation
unsigned short int boardToInt(char* board) {
	unsigned short int number = 0;
	int i;
	for (i = 0; i<16; i++) {
		if (board[i] != '-') {
			number |= 1 << i;    //insert a bit
		}
	}
	return number;
}

//one can see if a player wins depending on the bit structure of the player and these integers corresponds to a winnig bit structure
inline int possible_move(unsigned short int cross, unsigned short int circle, int where) {
	int check = 0;
	check |= 1 << where;   //insert a bit
	return (cross & check) != check && (circle & check) != check;
}

//one can see if a player wins depending on the bit structure of the player and these integers corresponds to a winnig bit structure
inline int win(unsigned short int player) {
	if ((player & 15) == 15 || (player & 240)  == 240 || (player & 3840)  == 3840 || (player & 61440) == 61440) {  //the rows
		return 1;
	}

	else if ((player & 4369) == 4369 || (player & 8738) == 8738 || (player & 17476) == 17476 || (player & 34952) == 34952) {  //the columns
		return 1;
	}
	else if ((player & 33825) == 33825 || (player & 4680) == 4680) {   //the diagonal
		return 1;
	}

	return 0;
}

//the game is a draw only if every cell is picked. This is when the sum of both integers are equal to this number
inline int draw(unsigned short int player1, unsigned short int player2) {
	return (player1 + player2) == 65535;
}

//the minimax function. This is the main algorithm and will decide the outcome of each branch if both players play perfectly
int minimax(unsigned short int cross, unsigned short int circle, char player) {
	//Check winner and draw:
	if (win(cross)) {
		return -1;
	}
	else if (win(circle)) {
		return 1;
	}
	else if (draw(cross,circle)) {
		return 0;
	}

	//play out game if nothing has been returned:
	int i;
	if (player == 'o') {
		int max = -1;
		for (i = 0; i < 16; i++) {
			if (possible_move(cross, circle, i)) {
				circle |= 1 << i;   //insert a bit
				int eval = minimax(cross, circle, 'x');
				if (eval > max) {
					max = eval;
				}
				circle &= ~(1 << i);   //resets the bit
			}
		}
		return max;
	}

	//must be 'x' since nothing has returned yet
	int min = 1;
	for (i = 0; i < 16; i++) {
		if (possible_move(cross, circle, i)) {
			cross |= 1 << i;
			int eval = minimax(cross, circle, 'o');
			if (eval < min) {
				min = eval;
			}
			cross &= ~(1 << i);
		}
	}
	return min;
}

int place_piece(unsigned short int cross, unsigned short int circle) {
	int eval_array[16] = {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2};  //initialize with -2 so we wont pick one we can't
	int i;

	for (i = 0; i < 16; i++) {
		if (possible_move(cross, circle, i)) {
			circle |= 1 << i;

			eval_array[i] = minimax(cross, circle, 'x');

			circle &= ~(1 << i);
		}
	}
	//return the best one
	int best_eval_index = 0;
	int best_eval = -1;
	for (i = 0; i < 16; i++) {
		if (eval_array[i] == 1) {
			return i;
		}
		else if (eval_array[i] > best_eval) {
			best_eval = eval_array[i];
			best_eval_index = i;
		}
	}
	return best_eval_index;
}

int main(int argc, char const* argv[]) {
	
	//for splitting the board later after placing down in the initial board, this is just
	//easier for the eye
	char cross_board[16];
	char circle_board[16];
	char board[16];

	board[0] = '-'; board[1] = '-'; board[2] = '-'; board[3] = '-';
	board[4] = '-'; board[5] = '-'; board[6] = 'o'; board[7] = '-';
	board[8] = 'x'; board[9] = 'o'; board[10] = 'x'; board[11] = '-';
	board[12] = '-'; board[13] = '-'; board[14] = '-'; board[15] = '-';

	boardToString(board);

	splitBoard(board, cross_board, circle_board);

	int index = place_piece(boardToInt(cross_board), boardToInt(circle_board));

	board[index] = 'o';
	boardToString(board);

	} //end of script


