#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "ui.h"
#include "chess.h"

int main(int argc, char** argv) {
	Board board = {};
	Game_State state = {};
	//game_state_init(&state);
	//board_startpos(&board);
	Board_Select b_select = 0;
	
	//parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board, &state);
	parse_fen("r1bqk2r/p2nppbp/2pp1np1/1p6/3PP3/2N1BP2/PPPQN1PP/R3KB1R w KQkq 4 5", &board, &state);

	piece_select(&board, &state, &b_select, 7, 4, king_valid_move);
	Position pos = { 7, 4 };
	generate_king_moves(pos, &state, &board, possible_moves, &possible_moves_count);

	print_board(&board, b_select);

	return 0;
}
