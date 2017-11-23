#pragma once
#include "common.h"

enum Player {
	PLAYER_WHITE,
	PLAYER_BLACK,
};

enum Piece {
	PIECE_NONE = 0,
	
	PIECE_WHITE_PAWN_0,
	PIECE_WHITE_PAWN_1,
	PIECE_WHITE_PAWN_2,
	PIECE_WHITE_PAWN_3,
	PIECE_WHITE_PAWN_4,
	PIECE_WHITE_PAWN_5,
	PIECE_WHITE_PAWN_6,
	PIECE_WHITE_PAWN_7,

	PIECE_BLACK_PAWN_0,
	PIECE_BLACK_PAWN_1,
	PIECE_BLACK_PAWN_2,
	PIECE_BLACK_PAWN_3,
	PIECE_BLACK_PAWN_4,
	PIECE_BLACK_PAWN_5,
	PIECE_BLACK_PAWN_6,
	PIECE_BLACK_PAWN_7,

	PIECE_WHITE_KING,
	PIECE_BLACK_KING,

	PIECE_WHITE_QUEEN,
	PIECE_BLACK_QUEEN,

	PIECE_WHITE_ROOK_0,
	PIECE_WHITE_ROOK_1,
	PIECE_BLACK_ROOK_0,
	PIECE_BLACK_ROOK_1,

	PIECE_WHITE_BISHOP_0,
	PIECE_WHITE_BISHOP_1,
	PIECE_BLACK_BISHOP_0,
	PIECE_BLACK_BISHOP_1,

	PIECE_WHITE_KNIGHT_0,
	PIECE_WHITE_KNIGHT_1,
	PIECE_BLACK_KNIGHT_0,
	PIECE_BLACK_KNIGHT_1,

	NUM_PIECES,
};

struct Piece_State {
	s32 line;
	s32 column;
	s32 num_moves;
	bool alive;
	bool moved;
};

struct Board {
	Piece piece[8][8];

	Board() {
		memset(piece, PIECE_NONE, sizeof(piece));
		for (int i = 0; i < 8; ++i) {
			piece[1][i] = (Piece)(PIECE_WHITE_PAWN_0 + i);
			piece[6][i] = (Piece)(PIECE_BLACK_PAWN_0 + i);
		}
	}
};

struct Game_State {
	Player turn;
	s32 move_count = 0;
	s32 move_draw = 0;		// number of moves without advancing a pawn or capturing a piece
	Piece_State piece_state[NUM_PIECES];
	Board board;
};

static bool is_inside_board(s32 value) {
	if (value < 8 && value >= 0)
		return true;
	return false;
}

static bool is_occupied(Game_State* state, s32 line, s32 column) {
	if (state->board.piece[line][column] == PIECE_NONE)
		return false;
	return true;
}

static bool is_black_pawn(Piece p) {
	if (p >= PIECE_BLACK_PAWN_0 && p <= PIECE_BLACK_PAWN_7)
		return true;
	return false;
}

static bool is_white_pawn(Piece p) {
	if (p >= PIECE_WHITE_PAWN_0 && p <= PIECE_WHITE_PAWN_7)
		return true;
	return false;
}

static bool is_white_in_check() {
	return false;
}

static bool is_black_in_check() {
	return false;
}

static bool is_valid(Game_State* state, Piece p, s32 line, s32 column) {
	if (!state->piece_state[p].alive)
		return false;
	if (!is_inside_board(line))
		return false;
	if (!is_inside_board(column))
		return false;
	
	Piece_State* piece_state = &state->piece_state[p];
	bool valid = false;

	switch (p) {
		case PIECE_WHITE_PAWN_0:
		case PIECE_WHITE_PAWN_1:
		case PIECE_WHITE_PAWN_2:
		case PIECE_WHITE_PAWN_3:
		case PIECE_WHITE_PAWN_4:
		case PIECE_WHITE_PAWN_5:
		case PIECE_WHITE_PAWN_6:
		case PIECE_WHITE_PAWN_7: {
			if (piece_state->column == column) {
				// advance pawn, see if is not blocked
				if (line == piece_state->line + 1) {
					if (!is_occupied(state, line, column)) {
						// pawn is free to move forward
						valid = true;
					}
				} else if (line == piece_state->line + 2 && !piece_state->moved) {
					if (!is_occupied(state, line - 1, column) && !is_occupied(state, line, column)) {
						valid = true;
					}
				}
			} else if((column == piece_state->column - 1 || column == piece_state->column + 1) && line == piece_state->line + 1){
				// capture move check en passant
				if (state->board.piece[line][column] != PIECE_NONE) {
					// it can capture a piece, check 
					valid = true;
				} else if(piece_state->line == 4) {
					Piece capture = state->board.piece[piece_state->line][column];
					if (is_black_pawn(capture) && state->piece_state[capture].num_moves == 1)
						valid = true;
				}
			}
			// check if the move leaves the king in check
			if (is_white_in_check())
				return false;
			return valid;
		} break;

		case PIECE_BLACK_PAWN_0:
		case PIECE_BLACK_PAWN_1:
		case PIECE_BLACK_PAWN_2:
		case PIECE_BLACK_PAWN_3:
		case PIECE_BLACK_PAWN_4:
		case PIECE_BLACK_PAWN_5:
		case PIECE_BLACK_PAWN_6:
		case PIECE_BLACK_PAWN_7: {
			if (piece_state->column == column) {
				// advance pawn, see if is not blocked
				if (line == piece_state->line - 1) {
					if (!is_occupied(state, line, column)) {
						// pawn is free to move forward
						valid = true;
					}
				} else if (line == piece_state->line - 2 && !piece_state->moved) {
					if (!is_occupied(state, line + 1, column) && !is_occupied(state, line, column)) {
						valid = true;
					}
				}
			} else if ((column == piece_state->column - 1 || column == piece_state->column + 1) && line == piece_state->line - 1) {
				// capture move check en passant
				if (state->board.piece[line][column] != PIECE_NONE) {
					// it can capture a piece, check 
					valid = true;
				} else if (piece_state->line == 3) {
					Piece capture = state->board.piece[piece_state->line][column];
					if (is_black_pawn(capture) && state->piece_state[capture].num_moves == 1)
						valid = true;
				}
			}
		}break;

		case PIECE_WHITE_KING:
		case PIECE_BLACK_KING: {

		}break;

		case PIECE_WHITE_QUEEN:
		case PIECE_BLACK_QUEEN: {

		}break;

		case PIECE_WHITE_ROOK_0:
		case PIECE_WHITE_ROOK_1:
		case PIECE_BLACK_ROOK_0:
		case PIECE_BLACK_ROOK_1: {

		}break;

		case PIECE_WHITE_BISHOP_0:
		case PIECE_WHITE_BISHOP_1:
		case PIECE_BLACK_BISHOP_0:
		case PIECE_BLACK_BISHOP_1: {

		}break;

		case PIECE_WHITE_KNIGHT_0:
		case PIECE_WHITE_KNIGHT_1:
		case PIECE_BLACK_KNIGHT_0:
		case PIECE_BLACK_KNIGHT_1: {

		}break;
	}
	// check if the move leaves the king in check
	if (state->turn == PLAYER_BLACK && is_black_in_check())
		return false;
	else if(is_white_in_check())
		return false;
	return valid;
}