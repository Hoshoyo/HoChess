#include "chess.h"

Board::Board()
{
	memset(piece, PIECE_NONE, sizeof(piece));
	for (int i = 0; i < 8; ++i) {
		piece[1][i] = (Piece)(PIECE_WHITE_PAWN_0 + i);
		piece[6][i] = (Piece)(PIECE_BLACK_PAWN_0 + i);
	}
	piece[0][0] = PIECE_WHITE_ROOK_0;
	piece[0][7] = PIECE_WHITE_ROOK_1;
	piece[0][1] = PIECE_WHITE_KNIGHT_0;
	piece[0][6] = PIECE_WHITE_KNIGHT_1;
	piece[0][2] = PIECE_WHITE_BISHOP_0;
	piece[0][5] = PIECE_WHITE_BISHOP_1;
	piece[0][3] = PIECE_WHITE_QUEEN;
	piece[0][4] = PIECE_WHITE_KING;

	piece[7][0] = PIECE_BLACK_ROOK_0;
	piece[7][7] = PIECE_BLACK_ROOK_1;
	piece[7][1] = PIECE_BLACK_KNIGHT_0;
	piece[7][6] = PIECE_BLACK_KNIGHT_1;
	piece[7][2] = PIECE_BLACK_BISHOP_0;
	piece[7][5] = PIECE_BLACK_BISHOP_1;
	piece[7][3] = PIECE_BLACK_QUEEN;
	piece[7][4] = PIECE_BLACK_KING;
}

void board_clear(Game_State* gs) {
	memset(gs->board.piece, PIECE_NONE, sizeof(gs->board.piece));
	for (s32 p = PIECE_NONE; p < NUM_PIECES; p += 1) {
		gs->piece_state[p].alive = false;
		gs->piece_state[p].column = -1;
		gs->piece_state[p].line = -1;
		gs->piece_state[p].moved = false;
		gs->piece_state[p].num_moves = 0;
	}
	memset(gs->DEBUG_marks.piece, PIECE_NONE, sizeof(gs->DEBUG_marks.piece));
}

void board_put(Game_State* gs, Piece p, s32 line, s32 column) {
	gs->board.piece[line][column] = p;
	if (p == PIECE_MARK)
		return;
	gs->piece_state[p].alive = true;
	gs->piece_state[p].column = column;
	gs->piece_state[p].line = line;
	gs->piece_state[p].moved = false;
	gs->piece_state[p].num_moves = 0;
}

void board_mark(Game_State* gs, s32 line, s32 column) {
	gs->DEBUG_marks.piece[line][column] = PIECE_MARK;
}

void board_unmark(Game_State* gs, s32 line, s32 column) {
	gs->DEBUG_marks.piece[line][column] = PIECE_NONE;
}

void board_mark_clear(Game_State* gs) {
	memset(gs->DEBUG_marks.piece, PIECE_NONE, sizeof(gs->DEBUG_marks.piece));
}

bool is_inside_board(s32 value) {
	if (value < 8 && value >= 0)
		return true;
	return false;
}

bool is_occupied(Game_State* state, s32 line, s32 column) {
	if (state->board.piece[line][column] == PIECE_NONE)
		return false;
	return true;
}

bool is_black_pawn(Piece p) {
	if (p >= PIECE_BLACK_PAWN_0 && p <= PIECE_BLACK_PAWN_7)
		return true;
	return false;
}

bool is_white_pawn(Piece p) {
	if (p >= PIECE_WHITE_PAWN_0 && p <= PIECE_WHITE_PAWN_7)
		return true;
	return false;
}

bool is_white_piece(Piece p) {
	if (p >= PIECE_WHITE_PAWN_0 && p <= PIECE_WHITE_KNIGHT_1)
		return true;
	return false;
}
bool is_black_piece(Piece p) {
	if (p >= PIECE_BLACK_PAWN_0 && p <= PIECE_BLACK_KNIGHT_1)
		return true;
	return false;
}

bool is_white_in_check() {
	return false;
}

bool is_black_in_check() {
	return false;
}

bool is_same_color_piece(Piece p1, Piece p2) {
	if (is_white_piece(p1)) {
		if (is_white_piece(p2))
			return true;
		else 
			return false;
	} else {
		if (is_black_piece(p2))
			return true;
		return false;
	}
}

bool is_valid(Game_State* state, Piece p, s32 line, s32 column)
{
	if (!state->piece_state[p].alive)
		return false;
	if (!is_inside_board(line))
		return false;
	if (!is_inside_board(column))
		return false;

	Piece_State* piece_state = &state->piece_state[p];
	if (piece_state->line == line && piece_state->column == column)
		return false;
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
			} else if (line == piece_state->line + 2 && piece_state->line == 1) {//!piece_state->moved) {
				if (!is_occupied(state, line - 1, column) && !is_occupied(state, line, column)) {
					valid = true;
				}
			}
		} else if ((column == piece_state->column - 1 || column == piece_state->column + 1) && line == piece_state->line + 1) {
			// capture move check en passant
			if (state->board.piece[line][column] != PIECE_NONE) {
				// it can capture a piece, check 
				valid = true;
			} else if (piece_state->line == 4) {
				Piece capture = state->board.piece[piece_state->line][column];
				if (is_black_pawn(capture) && state->piece_state[capture].num_moves == 1)
					valid = true;
			}
		}
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
			} else if (line == piece_state->line - 2 && piece_state->line == 6) {//!piece_state->moved) {
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
		if ((piece_state->line + 1 >= line && piece_state->line - 1 <= line) &&			// is within 1 line
			(piece_state->column + 1 >= column && piece_state->column - 1 <= column))		// is within 1 column
		{
			valid = true;
			if (state->board.piece[line][column] != PIECE_NONE) {
				if (is_same_color_piece(p, state->board.piece[line][column]))
					valid = false;
			}
		}
	}break;

	case PIECE_WHITE_QUEEN:
	case PIECE_BLACK_QUEEN: {

		s32 y_dir = (line - piece_state->line < 0) ? -1 : (line - piece_state->line == 0) ? 0 : 1;
		s32 x_dir = (column - piece_state->column < 0) ? -1 : (column - piece_state->column == 0) ? 0 : 1;

		for (s32 j = piece_state->column + x_dir, i = piece_state->line + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == line && j == column) {
					valid = true;
					break;
				}
				continue;
			} else if (is_same_color_piece(p, state->board.piece[i][j])) {
				valid = false;
				break;
			} else {
				if (i == line && j == column) {
					valid = true;
					break;
				} else {
					valid = false;
					break;
				}
			}
		}

	}break;

	case PIECE_WHITE_ROOK_0:
	case PIECE_WHITE_ROOK_1:
	case PIECE_BLACK_ROOK_0:
	case PIECE_BLACK_ROOK_1: {
		s32 y_dir = (line - piece_state->line < 0) ? -1 : (line - piece_state->line == 0) ? 0 : 1;
		s32 x_dir = (column - piece_state->column < 0) ? -1 : (column - piece_state->column == 0) ? 0 : 1;
		if (y_dir != 0 && x_dir != 0)
			return false;

		for (s32 j = piece_state->column + x_dir, i = piece_state->line + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == line && j == column) {
					valid = true;
					break;
				}
				continue;
			} else if (is_same_color_piece(p, state->board.piece[i][j])) {
				valid = false;
				break;
			} else {
				if (i == line && j == column) {
					valid = true;
					break;
				} else {
					valid = false;
					break;
				}
			}
		}
	}break;

	case PIECE_WHITE_BISHOP_0:
	case PIECE_WHITE_BISHOP_1:
	case PIECE_BLACK_BISHOP_0:
	case PIECE_BLACK_BISHOP_1: {
		s32 y_dir = (line - piece_state->line < 0) ? -1 : (line - piece_state->line == 0) ? 0 : 1;
		s32 x_dir = (column - piece_state->column < 0) ? -1 : (column - piece_state->column == 0) ? 0 : 1;
		if (y_dir == 0 || x_dir == 0)
			return false;

		for (s32 j = piece_state->column + x_dir, i = piece_state->line + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == line && j == column) {
					valid = true;
					break;
				}
				continue;
			} else if (is_same_color_piece(p, state->board.piece[i][j])) {
				valid = false;
				break;
			} else {
				if (i == line && j == column) {
					valid = true;
					break;
				} else {
					valid = false;
					break;
				}
			}
		}
	}break;

	case PIECE_WHITE_KNIGHT_0:
	case PIECE_WHITE_KNIGHT_1:
	case PIECE_BLACK_KNIGHT_0:
	case PIECE_BLACK_KNIGHT_1: {
		s32 y_diff = line - piece_state->line;
		y_diff = (y_diff > 0) ? y_diff : -y_diff;
		s32 x_diff = column - piece_state->column;
		x_diff = (x_diff > 0) ? x_diff : -x_diff;
		if (y_diff < 1 || y_diff > 2 || x_diff < 1 || x_diff > 2)
			return false;
		else if ((y_diff == 2 && x_diff == 1) || (y_diff == 1 && x_diff == 2))
			valid = true;
		
	}break;
	}
	// check if the move leaves the king in check
	if (state->turn == PLAYER_BLACK && is_black_in_check())
		return false;
	else if (is_white_in_check())
		return false;
	return valid;
}
