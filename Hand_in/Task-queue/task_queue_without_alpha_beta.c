#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>

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

//looks to see if a specific index is already occupied
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

int minimax(unsigned short int cross, unsigned short int circle, char player) {
	//Check winner or draw:
	if (win(cross)) {
		return -1;
	}
	else if (win(circle)) {
		return 1;
	}
	else if (draw(cross,circle)) {
		return 0;
	}

	int i;
	if (player == 'o') {
		int max = -1;
		for (i = 0; i < 16; i++) {
			if (possible_move(cross, circle, i)) {
				circle |= 1 << i;   //insert a bit, this could be done directly in the function probably and therefor never had to be reset?
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

int minimax_tasked(unsigned short int cross, unsigned short int circle, char player, int depth, int startindex, int* recursive_array) {
	//Check winner or draw:
	if (win(cross)) {
		return -1;
	}
	else if (win(circle)) {
		return 1;
	}
	else if (draw(cross,circle)) {
		return 0;
	}

	if (depth > 0) {
		if (player == 'o') {
			int i;
			for (i = 0; i < 16; i++) {
				if (possible_move(cross, circle, i)) {
					#pragma omp task firstprivate(cross, circle, i, depth)
					{
						circle |= 1 << i;
						recursive_array[startindex + i] = minimax_tasked(cross, circle, 'x', depth-1, (i+1)*startindex/17, recursive_array);
						//Don't need to reset since it's a private copy
					}
				}
			}
			//need to wait until the initialized tasked has finished
			#pragma omp taskwait
			int max = -1;
			for (i = 0; i < 16; i++) {
				if (recursive_array[startindex + i] > max) {
					max = recursive_array[startindex + i];
				}
			}
			return max;
		}

		//must be 'x' since nothing has returned yet
		int i;
		for (i = 0; i < 16; i++) {
			if (possible_move(cross, circle, i)) {
				#pragma omp task firstprivate(cross, circle, i, depth, startindex)
				{
					cross |= 1 << i;
					recursive_array[startindex + i] = minimax_tasked(cross, circle, 'o', depth-1, (i+1)*startindex/17, recursive_array);
				}
			}
		}
		//need to wait until the initialized tasked has finished
		#pragma omp taskwait
		int min = 1;
		for (i = 0; i < 16; i++) {
			if (recursive_array[startindex + i] < min) {
				min = recursive_array[startindex + i];
			}
		}
		return min;
	}

	//if depth is zero
	if (player == 'o') {
		int i;
		for (i = 0; i < 16; i++) {
			if (possible_move(cross, circle, i)) {
				circle |= 1 << i;
				recursive_array[startindex + i] = minimax(cross, circle, 'x');
				circle &= ~(1 << i);
				}
			}

		//return max
		int max = -1;
		for (i = 0; i < 16; i++) {
			if (recursive_array[startindex + i] > max) {
				max = recursive_array[startindex + i];
			}
		}
		return max;
	}

	//must be 'x' since nothing has returned yet
	int i;
	for (i = 0; i < 16; i++) {
		if (possible_move(cross, circle, i)) {
			cross |= 1 << i;
			recursive_array[startindex + i] = minimax(cross, circle, 'o');
			cross &= ~(1 << i);
		}
	}
		
	//return min
	int min = 1;
	for (i = 0; i < 16; i++) {
		if (recursive_array[startindex + i] < min) {
			min = recursive_array[startindex + i];
		}
	}
	return min;
}

int place_piece(unsigned short int cross, unsigned short int circle, int threads, int depth) {
	int eval_array[16] = {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2};  //initialize with -2 so we wont pick one we can't
	int i;

	omp_set_nested(1);
	if (depth == 0) {
		omp_set_num_threads(1);
	}
	else {
		omp_set_num_threads(threads);
	}

	int rec_size = pow(17,depth+1);                         
	int* recursive_array = malloc(rec_size*sizeof(int));
	#pragma omp parallel
	{
		#pragma omp single private(i)
		{
			for (i = 0; i < 16; i++) {
				if (possible_move(cross, circle, i)) {
					#pragma omp task firstprivate(cross, circle, i, depth)
					{
						circle |= 1 << i;
						if (depth > 1) {
							eval_array[i] = minimax_tasked(cross, circle, 'x', depth-1, (i+1)*rec_size/17, recursive_array);
						}
						else {
							eval_array[i] = minimax(cross, circle, 'x');
						}
					}
				}
			}
		}
	}
	//Since out of the parallel loop one have a implicit barrier so no need for wait command
	//we need to free the array
	free(recursive_array);
	//return the best one, this section is easiest done in serial
	int best_eval_index = 0;
	int best_eval = -2;
	for (i = 0; i < 16; i++) {
		if (eval_array[i] == 1) {
			return i;
		}
		else if (eval_array[i] > best_eval) {
			best_eval = eval_array[i];
			best_eval_index = i;
		}
	}
	printf("the best eval: %d \n", best_eval);
	return best_eval_index;
}

int main(int argc, char const* argv[]) {

	if (argc != 3) {
		printf("expected 2 input argument, number of threads and depth \n");
		return 0;
	}
	
	//for splitting the board later after placing down in the initial board, this is just
	//easier for the eye
	char cross_board[16];
	char circle_board[16];
	char board[16];

	board[0] = '-'; board[1] = '-'; board[2] = '-'; board[3] = '-';
	board[4] = '-'; board[5] = '-'; board[6] = '-'; board[7] = '-';
	board[8] = '-'; board[9] = 'x'; board[10] = 'x'; board[11] = 'x';
	board[12] = '-'; board[13] = '-'; board[14] = '-'; board[15] = '-';

	boardToString(board);

	int threads = atoi(argv[1]);  //number of threads
	int parallel_depth = atoi(argv[2]);  //parallel depth

	splitBoard(board, cross_board, circle_board);

	//use the time measure here, since this is the interesting part
	double time1 = omp_get_wtime();
	int index = place_piece(boardToInt(cross_board), boardToInt(circle_board), threads, parallel_depth);
	double time2 = omp_get_wtime();

	printf("Elapsed time in seconds: %f \n", time2-time1);

	board[index] = 'o';
	boardToString(board);

	printf("optimized with %d threads \n", threads);

	} //end of script


