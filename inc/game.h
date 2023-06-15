#ifndef F4_GAME_HH
#define F4_GAME_HH

#include "shared_memory.h"

void print_game(int rows, int cols, int mat[rows][cols], char  p1_sign, char p2_sign);
void F4_game(game_t *game_data, int matrix_game[game_data->rows][game_data->cols]);
/*
 * return 0: ok
 * return -1: the column select is full
 */
int play(game_t *game_data, int matrix_game[game_data->rows][game_data->cols], int choice,int player);
/*
 * return 1: win
 * return 0: not win
 */
int check_win(int rows, int cols, int matrix_game[rows][cols]);

#endif
