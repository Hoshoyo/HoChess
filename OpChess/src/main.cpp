#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "ui.h"
#include "chess.h"

int main(int argc, char** argv) {
	Board board = {};
	Game_State state = {};
	game_state_init(&state);
	//board_startpos(&board);

	Board_Select b_select = 0;

	board.square[2][2] = PIECE_WHITE_BISHOP;
	board.square[4][3] = PIECE_BLACK_KNIGHT;
	board.square[1][4] = PIECE_WHITE_QUEEN;
	board.square[5][1] = PIECE_WHITE_PAWN;
	board.square[1][5] = PIECE_BLACK_PAWN;
	board.square[0][4] = PIECE_WHITE_KING;
	board.square[7][4] = PIECE_BLACK_KING;
	board.square[0][0] = PIECE_WHITE_ROOK;
	board.square[0][1] = PIECE_WHITE_KNIGHT;

	//piece_select(&board, &b_select, 3, 3, bishop_valid_move);
	//piece_select(&board, &b_select, 2, 4, queen_valid_move);
	//piece_select(&board, &state, &b_select, 4, 3, knight_valid_move);
	piece_select(&board, &state, &b_select, 7, 4, king_valid_move);

	print_board(&board, b_select);

	return 0;
}
