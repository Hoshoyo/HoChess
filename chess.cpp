#include "chess.h"

// @TODO dont differentiate between pieces of the same type

void board_init(Game_State* gs) {
	for (int i = 0; i < 8; ++i) {
		board_put(gs, PIECE_WHITE_PAWN, 1, i);
		board_put(gs, PIECE_BLACK_PAWN, 6, i);
	}

	board_put(gs, PIECE_WHITE_ROOK,   0, 0);
	board_put(gs, PIECE_WHITE_ROOK,   0, 7);
	board_put(gs, PIECE_WHITE_KNIGHT, 0, 1);
	board_put(gs, PIECE_WHITE_KNIGHT, 0, 6);
	board_put(gs, PIECE_WHITE_BISHOP, 0, 2);
	board_put(gs, PIECE_WHITE_BISHOP, 0, 5);
	board_put(gs, PIECE_WHITE_QUEEN,    0, 3);
	board_put(gs, PIECE_WHITE_KING,     0, 4);

	board_put(gs, PIECE_BLACK_ROOK,   7, 0);
	board_put(gs, PIECE_BLACK_ROOK,   7, 7);
	board_put(gs, PIECE_BLACK_KNIGHT, 7, 1);
	board_put(gs, PIECE_BLACK_KNIGHT, 7, 6);
	board_put(gs, PIECE_BLACK_BISHOP, 7, 2);
	board_put(gs, PIECE_BLACK_BISHOP, 7, 5);
	board_put(gs, PIECE_BLACK_QUEEN,    7, 3);
	board_put(gs, PIECE_BLACK_KING,     7, 4);
}

void board_clear(Game_State* gs) {
	memset(gs->board.piece, PIECE_NONE, sizeof(gs->board.piece));
}

void board_put(Game_State* gs, Piece p, s32 line, s32 column) {
	gs->board.piece[line][column] = p;
	if (p == PIECE_WHITE_KING) {
		gs->king_state[PLAYER_WHITE].file = column;
		gs->king_state[PLAYER_WHITE].rank = line;
	}
	if (p == PIECE_BLACK_KING) {
		gs->king_state[PLAYER_BLACK].file = column;
		gs->king_state[PLAYER_BLACK].rank = line;
	}
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

bool is_white_piece(Piece p) {
	if (p >= PIECE_WHITE_PAWN && p <= PIECE_WHITE_KNIGHT)
		return true;
	return false;
}
bool is_black_piece(Piece p) {
	if (p >= PIECE_BLACK_PAWN && p <= PIECE_BLACK_KNIGHT)
		return true;
	return false;
}

bool is_white_in_check_after_white_move(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece moving = state->board.piece[src_rank][src_file];
	Piece previous = state->board.piece[dst_rank][dst_file];
	state->board.piece[src_rank][src_file] = PIECE_NONE;
	state->board.piece[dst_rank][dst_file] = moving;

	s32 king_rank = state->king_state[PLAYER_WHITE].rank;
	s32 king_file = state->king_state[PLAYER_WHITE].file;

	bool in_check = false;

	for (s32 i = 0; i < 8; ++i) {
		for (s32 j = 0; j < 8; ++j) {
			Piece p = state->board.piece[i][j];
			switch (p) {
				case PIECE_BLACK_QUEEN: {
					if (king_rank == i || king_file == j || king_rank - i == king_file - j) {
						in_check = is_valid(state, i, j, king_rank, king_file);
						if (in_check)
							goto end_and_restore;
					}
				}break;
				case PIECE_BLACK_BISHOP: {
					if (king_rank - i == king_file - j) {
						in_check = is_valid(state, i, j, king_rank, king_file);
						if(in_check)
							goto end_and_restore;
					}
				}break;
				case PIECE_BLACK_ROOK: {
					if (king_rank == i || king_file == j) {
						in_check = is_valid(state, i, j, king_rank, king_file);
						if (in_check)
							goto end_and_restore;
					}
				}break;
				case PIECE_BLACK_KNIGHT: {
					s32 y_diff = i - king_rank;
					y_diff = (y_diff > 0) ? y_diff : -y_diff;
					s32 x_diff = j - king_file;
					x_diff = (x_diff > 0) ? x_diff : -x_diff;

					if (y_diff < 1 || y_diff > 2 || x_diff < 1 || x_diff > 2)
						break;
					else if ((y_diff == 2 && x_diff == 1) || (y_diff == 1 && x_diff == 2)) {
						in_check = is_valid(state, i, j, king_rank, king_file);
						if (in_check)
							goto end_and_restore;
					}
				}break;
				default: break;
			}
		}
	}

end_and_restore:
	// restore game state
	state->board.piece[dst_rank][dst_file] = previous;
	state->board.piece[src_rank][src_file] = moving;
	return in_check;
}

bool is_black_in_check_after_black_move(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece moving = state->board.piece[src_rank][src_file];
	Piece previous = state->board.piece[dst_rank][dst_file];
	state->board.piece[src_rank][src_file] = PIECE_NONE;
	state->board.piece[dst_rank][dst_file] = moving;

	s32 king_rank = state->king_state[PLAYER_BLACK].rank;
	s32 king_file = state->king_state[PLAYER_BLACK].file;

	bool in_check = false;

	for (s32 i = 0; i < 8; ++i) {
		for (s32 j = 0; j < 8; ++j) {
			Piece p = state->board.piece[i][j];
			switch (p) {
			case PIECE_WHITE_QUEEN: {
				if (king_rank == i || king_file == j || king_rank - i == king_file - j) {
					in_check = is_valid(state, i, j, king_rank, king_file);
					if (in_check)
						goto end_and_restore;
				}
			}break;
			case PIECE_WHITE_BISHOP: {
				if (king_rank - i == king_file - j) {
					in_check = is_valid(state, i, j, king_rank, king_file);
					if (in_check)
						goto end_and_restore;
				}
			}break;
			case PIECE_WHITE_ROOK: {
				if (king_rank == i || king_file == j) {
					in_check = is_valid(state, i, j, king_rank, king_file);
					if (in_check)
						goto end_and_restore;
				}
			}break;
			case PIECE_WHITE_KNIGHT: {
				s32 y_diff = i - king_rank;
				y_diff = (y_diff > 0) ? y_diff : -y_diff;
				s32 x_diff = j - king_file;
				x_diff = (x_diff > 0) ? x_diff : -x_diff;

				if (y_diff < 1 || y_diff > 2 || x_diff < 1 || x_diff > 2)
					break;
				else if ((y_diff == 2 && x_diff == 1) || (y_diff == 1 && x_diff == 2)) {
					in_check = is_valid(state, i, j, king_rank, king_file);
					if (in_check)
						goto end_and_restore;
				}
			}break;
			default: break;
			}
		}
	}

end_and_restore:
	// restore game state
	state->board.piece[dst_rank][dst_file] = previous;
	state->board.piece[src_rank][src_file] = moving;
	return in_check;
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

bool is_valid(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file)
{
	// LINE = RANK
	// COL  = FILE

	Piece moving = state->board.piece[src_rank][src_file];
	if (moving == PIECE_NONE)
		return false;
	if (!is_inside_board(dst_rank))
		return false;
	if (!is_inside_board(dst_file))
		return false;

	if (src_file == dst_file && src_rank == dst_rank) return false;

	bool valid = false;

	switch (moving) {
	case PIECE_WHITE_PAWN: {
		if (src_file == dst_file) {
			// advance pawn, see if is not blocked
			if (dst_rank == src_rank + 1) {
				if (!is_occupied(state, dst_rank, dst_file)) {
					// pawn is free to move forward
					valid = true;
				}
			} else if (dst_rank == src_rank + 2 && src_rank == 1) {//!piece_state->moved) {
				if (!is_occupied(state, dst_rank - 1, dst_file) && !is_occupied(state, dst_rank, dst_file)) {
					valid = true;
				}
			}
		} else if ((dst_file == src_file - 1 || dst_file == src_file + 1) && dst_rank == src_rank + 1) {
			// capture move check en passant
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				// it can capture a piece, check 
				valid = true;
			} else if (src_rank == 4) {
				Piece capture = state->board.piece[src_rank][dst_file];
				// @TODO TEST EN PASSANT
				if (capture == PIECE_BLACK_PAWN && state->en_passant_file == dst_file)
					valid = true;
			}
		}
	} break;

	case PIECE_BLACK_PAWN: {
		if (src_file == dst_file) {
			// advance pawn, see if is not blocked
			if (dst_rank == src_rank - 1) {
				if (!is_occupied(state, dst_rank, dst_file)) {
					// pawn is free to move forward
					valid = true;
				}
			} else if (dst_rank == src_rank - 2 && src_rank == 6) {//!piece_state->moved) {
				if (!is_occupied(state, dst_rank + 1, dst_file) && !is_occupied(state, dst_rank, dst_file)) {
					valid = true;
				}
			}
		} else if ((dst_file == src_file - 1 || dst_file == src_file + 1) && dst_rank == src_rank - 1) {
			// capture move check en passant
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				// it can capture a piece, check 
				valid = true;
			} else if (src_rank == 3) {
				Piece capture = state->board.piece[src_rank][dst_file];
				// @TODO TEST EN PASSANT
				if (capture == PIECE_WHITE_PAWN && state->en_passant_file == dst_file)
					valid = true;
			}
		}
	}break;

	case PIECE_WHITE_KING:  
	case PIECE_BLACK_KING: {
		if ((src_rank + 1 >= dst_rank && src_rank - 1 <= dst_rank) &&			// is within 1 line
			(src_file + 1 >= dst_file && src_file - 1 <= dst_file))				// is within 1 column
		{
			valid = true;
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				if (is_same_color_piece(moving, state->board.piece[dst_rank][dst_file]))
					valid = false;
			}
		}
	}break;

	case PIECE_WHITE_QUEEN:
	case PIECE_BLACK_QUEEN: {
		s32 y_dir = (dst_rank - src_rank < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
		s32 x_dir = (dst_file - src_file < 0) ? -1 : (dst_file - src_file == 0) ? 0 : 1;

		for (s32 j = src_file + x_dir, i = src_rank + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == dst_rank && j == dst_file) {
					valid = true;
					break;
				}
				continue;
			} else if (is_same_color_piece(moving, state->board.piece[i][j])) {
				valid = false;
				break;
			} else {
				if (i == dst_rank && j == dst_file) {
					valid = true;
					break;
				} else {
					valid = false;
					break;
				}
			}
		}

	}break;

	case PIECE_WHITE_ROOK:
	case PIECE_BLACK_ROOK: {
		s32 y_dir = (dst_rank - src_rank < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
		s32 x_dir = (dst_file - dst_file < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
		if (y_dir != 0 && x_dir != 0)
			return false;

		for (s32 j = src_file + x_dir, i = src_rank + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == dst_rank && j == dst_file) {
					valid = true;
					break;
				}
				continue;
			} else if (is_same_color_piece(moving, state->board.piece[i][j])) {
				valid = false;
				break;
			} else {
				if (i == dst_rank && j == dst_file) {
					valid = true;
					break;
				} else {
					valid = false;
					break;
				}
			}
		}
	}break;

	case PIECE_WHITE_BISHOP:
	case PIECE_BLACK_BISHOP: {
		s32 y_dir = (dst_rank - src_rank < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
		s32 x_dir = (dst_file - src_file < 0) ? -1 : (dst_file - src_file == 0) ? 0 : 1;
		if (y_dir == 0 || x_dir == 0)
			return false;

		for (s32 j = src_file + x_dir, i = src_rank + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == dst_rank && j == dst_file) {
					valid = true;
					break;
				}
				continue;
			} else if (is_same_color_piece(moving, state->board.piece[i][j])) {
				valid = false;
				break;
			} else {
				if (i == dst_rank && j == dst_file) {
					valid = true;
					break;
				} else {
					valid = false;
					break;
				}
			}
		}
	}break;

	case PIECE_WHITE_KNIGHT:
	case PIECE_BLACK_KNIGHT: {
		s32 y_diff = dst_rank - src_rank;
		y_diff = (y_diff > 0) ? y_diff : -y_diff;
		s32 x_diff = dst_file - src_file;
		x_diff = (x_diff > 0) ? x_diff : -x_diff;
		if (y_diff < 1 || y_diff > 2 || x_diff < 1 || x_diff > 2)
			return false;
		else if ((y_diff == 2 && x_diff == 1) || (y_diff == 1 && x_diff == 2))
			valid = true;
		
	}break;
	}
	// check if the move leaves the king in check
	
	if (is_black_piece(moving) && is_black_in_check_after_black_move(state, src_rank, src_file, dst_rank, dst_file))
		return false;
	else if (is_white_in_check_after_white_move(state, src_rank, src_file, dst_rank, dst_file))
		return false;
	return valid;
}

void switch_turn(Game_State* state) {
	if (state->turn == PLAYER_WHITE)
		state->turn = PLAYER_BLACK;
	else
		state->turn = PLAYER_WHITE;
}

void init_game_state(Game_State* game_state) {
	game_state->turn = PLAYER_WHITE;
	game_state->move_count = 0;
	game_state->move_draw = 0;

	game_state->king_state[PLAYER_WHITE].in_check = false;
	game_state->king_state[PLAYER_WHITE].rank = 0;
	game_state->king_state[PLAYER_WHITE].file = 4;

	game_state->king_state[PLAYER_BLACK].in_check = false;
	game_state->king_state[PLAYER_BLACK].rank = 7;
	game_state->king_state[PLAYER_BLACK].file = 4;
}

// returns false on invalid move
bool interpret_move(Game_State* state, s8* buffer, u32 length) 
{
	// e2e4 e7e8q
	if (length < 4) return false;

	s8* at = buffer;
	s8 src_file = at[0] - 0x61;
	s8 src_rank = at[1] - 0x31;
	s8 dst_file = at[2] - 0x61;
	s8 dst_rank = at[3] - 0x31;

	Piece moving = state->board.piece[src_rank][src_file];

	if (is_valid(state, src_rank, src_file, dst_rank, dst_file)) {
		if ((moving == PIECE_BLACK_PAWN && dst_rank == 0) || (moving == PIECE_WHITE_PAWN && dst_rank == 7)) {
			Piece promotion_piece = PIECE_NONE;
			if (length >= 5) {
				if (state->turn == PLAYER_WHITE) {
					switch (at[4]) {
					case 'q': promotion_piece = PIECE_WHITE_QUEEN; break;
					case 'b': promotion_piece = PIECE_WHITE_BISHOP; break;
					case 'n': promotion_piece = PIECE_WHITE_KNIGHT; break;
					case 'r': promotion_piece = PIECE_WHITE_ROOK; break;
					default: return false;
					}
				}
				else {
					switch (at[4]) {
					case 'q': promotion_piece = PIECE_BLACK_QUEEN; break;
					case 'b': promotion_piece = PIECE_BLACK_BISHOP; break;
					case 'n': promotion_piece = PIECE_BLACK_KNIGHT; break;
					case 'r': promotion_piece = PIECE_BLACK_ROOK; break;
					default: return false;
					}
				}
			}
			if (promotion_piece == PIECE_NONE) return false;
			// promote
			board_put(state, promotion_piece, dst_rank, dst_file);
		} else {
			board_put(state, moving, dst_rank, dst_file);
		}
		board_put(state, PIECE_NONE, src_rank, src_file);
		return true;
	} else {
		return false;
	}
}