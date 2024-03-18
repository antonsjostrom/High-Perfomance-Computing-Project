#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int possible_move(char* board, int index) {
	return board[index] == '-';
}

char win(char* board) {
	//Put the != '-' before everything to short circuit
	if (board[0] != '-' && board[0] == board[1] && board[0] == board[2] && board[0] == board[3]) {
		return board[0];
	}
	if (board[4] != '-' && board[4] == board[5] && board[4] == board[6] && board[4] == board[7]) {
		return board[4];
	}
	if (board[8] != '-' && board[8] == board[9] && board[8] == board[10] && board[8] == board[11]) {
		return board[8];
	}
	if (board[12] != '-' && board[12] == board[13] && board[12] == board[14] && board[12] == board[15]) {
		return board[12];
	}
	if (board[0] != '-' && board[0] == board[4] && board[0] == board[8] && board[0] == board[12]) {
		return board[0];
	}
	if (board[1] != '-' && board[1] == board[5] && board[1] == board[9] && board[1] == board[13]) {
		return board[1];
	}
	if (board[2] != '-' && board[2] == board[6] && board[2] == board[10] && board[2] == board[14]) {
		return board[2];
	}
	if (board[3] != '-' && board[3] == board[7] && board[3] == board[11] && board[3] == board[15]) {
		return board[3];
	}
	if (board[0] != '-' && board[0] == board[5] && board[0] == board[10] && board[0] == board[15]) {
		return board[0];
	}
	if (board[12] != '-' && board[12] == board[9] && board[12] == board[6] && board[12] == board[3]) {
		return board[12];
	}
	else {
		int i;
		for (i=0; i<16; i++) {
			if (board[i] == '-') {
				return '-';   //undecided
			}
		}
		return 'd';  //draw
	}
}

//just prints the board
void boardToString(char* board) {
	printf("%c %c %c %c \n", board[0], board[1], board[2], board[3]);
	printf("%c %c %c %c \n", board[4], board[5], board[6], board[7]);
	printf("%c %c %c %c \n", board[8], board[9], board[10], board[11]);
	printf("%c %c %c %c \n", board[12], board[13], board[14], board[15]);
	printf("\n");
}

//the minimax function. This is the main algorithm and will decide the outcome of each branch if both players play perfectly
int minimax(char* board, char player, int alpha, int beta) {
	//Check winner:
	char winner = win(board);
	if (winner == 'x') {
		return -1;
	}
	else if(winner == 'o') {
		return 1;
	}
	else if (winner == 'd') {
		return 0;
	}

	//play out game:
	int i;
	if (player == 'o') {
		int max = -1;
		for (i = 0; i < 16; i++) {
			if (possible_move(board, i)) {
				board[i] = player;
				int eval = minimax(board, 'x', alpha, beta);
				if (eval > max) {
					max = eval;
				}
				if (eval > alpha) {
					alpha = eval;
				}
				if (beta <= alpha) {
					board[i] = '-';
					break;
				}
				board[i] = '-';
			}
		}
		return max;
		}

	//if player not 'o', then it must be 'x'
	int min = 1;
	for (i = 0; i < 16; i++) {
		if (possible_move(board, i)) {
			board[i] = player;
			int eval = minimax(board, 'o', alpha, beta);
			if (eval < min) {
				min = eval;
			}
			if (eval < beta) {
				beta = eval;
			}
			if (beta <= alpha) {
				board[i] = '-';
				break;
			}
			board[i] = '-';
		}
	}
	return min;
}

int place_piece(char* board) {
	int eval_array[16] = {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2};  //initialize with -2 so we wont pick one we can't
	int i;

	for (i = 0; i < 16; i++) {
		if (possible_move(board, i)) {
			board[i] = 'o';
			eval_array[i] = minimax(board, 'x',-1,1);
			board[i] = '-';
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
	
	char board[16];
	board[0] = '-'; board[1] = '-'; board[2] = '-'; board[3] = '-';
	board[4] = '-'; board[5] = '-'; board[6] = '-'; board[7] = '-';
	board[8] = '-'; board[9] = '-'; board[10] = 'x'; board[11] = '-';
	board[12] = '-'; board[13] = '-'; board[14] = '-'; board[15] = '-';


	boardToString(board);
	int index = place_piece(board);
	board[index] = 'o';
	boardToString(board);

	} //end of script


