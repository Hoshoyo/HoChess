#include "chess.h"
#include <assert.h>

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
	memset(gs->board.selected, 0, sizeof(gs->board.selected));
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

bool is_inside_board(s32 rank, s32 file)
{
	if (rank < 8 && rank >= 0 && file < 8 && file >= 0)
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

static s32 absolute(s32 value) {
	return (value < 0) ? -value : value;
}

bool is_white_in_check_after_white_move(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece moving = state->board.piece[src_rank][src_file];
	Piece previous = state->board.piece[dst_rank][dst_file];
	state->board.piece[src_rank][src_file] = PIECE_NONE;
	state->board.piece[dst_rank][dst_file] = moving;

	King_State prev_king_state = state->king_state[PLAYER_WHITE];
	if (moving == PIECE_WHITE_KING) {
		state->king_state[PLAYER_WHITE].rank = dst_rank;
		state->king_state[PLAYER_WHITE].file = dst_file;
	}

	s32 king_rank = state->king_state[PLAYER_WHITE].rank;
	s32 king_file = state->king_state[PLAYER_WHITE].file;

	bool in_check = false;

	for (s32 i = 0; i < 8; ++i) {
		for (s32 j = 0; j < 8; ++j) {
			Piece p = state->board.piece[i][j];
			switch (p) {
				case PIECE_BLACK_QUEEN: {
					if (king_rank == i || king_file == j || absolute(king_rank - i) == absolute(king_file - j)) {
						in_check = is_valid(state, i, j, king_rank, king_file);
						if (in_check)
							goto end_and_restore;
					}
				}break;
				case PIECE_BLACK_BISHOP: {
					if (absolute(king_rank - i) == absolute(king_file - j)) {
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
	state->king_state[PLAYER_WHITE] = prev_king_state;
	return in_check;
}

bool is_black_in_check_after_black_move(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece moving = state->board.piece[src_rank][src_file];
	Piece previous = state->board.piece[dst_rank][dst_file];
	state->board.piece[src_rank][src_file] = PIECE_NONE;
	state->board.piece[dst_rank][dst_file] = moving;

	King_State prev_king_state = state->king_state[PLAYER_BLACK];
	if (moving == PIECE_BLACK_KING) {
		state->king_state[PLAYER_BLACK].rank = dst_rank;
		state->king_state[PLAYER_BLACK].file = dst_file;
	}

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
	state->king_state[PLAYER_BLACK] = prev_king_state;
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

s32 is_valid(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file)
{
	// LINE = RANK
	// COL  = FILE

	Piece moving = state->board.piece[src_rank][src_file];
	if (moving == PIECE_NONE)
		return MOVE_INVALID;
	if (!is_inside_board(dst_rank))
		return MOVE_INVALID;
	if (!is_inside_board(dst_file))
		return MOVE_INVALID;

	if (src_file == dst_file && src_rank == dst_rank) return false;

	s32 valid = MOVE_INVALID;

	switch (moving) {
	case PIECE_WHITE_PAWN: {
		if (src_file == dst_file) {
			// advance pawn, see if is not blocked
			if (dst_rank == src_rank + 1) {
				if (!is_occupied(state, dst_rank, dst_file)) {
					// pawn is free to move forward
					valid = MOVE_VALID;
				}
			} else if (dst_rank == src_rank + 2 && src_rank == 1) {//!piece_state->moved) {
				if (!is_occupied(state, dst_rank - 1, dst_file) && !is_occupied(state, dst_rank, dst_file)) {
					valid = MOVE_VALID;
				}
			}
		} else if ((dst_file == src_file - 1 || dst_file == src_file + 1) && dst_rank == src_rank + 1) {
			// capture move check en passant
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				// it can capture a piece, check 
				valid = MOVE_VALID;
			} else if (src_rank == 4) {
				Piece capture = state->board.piece[src_rank][dst_file];
				// @TODO TEST EN PASSANT
				if (capture == PIECE_BLACK_PAWN && state->en_passant_file == dst_file)
					valid = MOVE_VALID_EN_PASSANT;
			}
		}
	} break;

	case PIECE_BLACK_PAWN: {
		if (src_file == dst_file) {
			// advance pawn, see if is not blocked
			if (dst_rank == src_rank - 1) {
				if (!is_occupied(state, dst_rank, dst_file)) {
					// pawn is free to move forward
					valid = MOVE_VALID;
				}
			} else if (dst_rank == src_rank - 2 && src_rank == 6) {//!piece_state->moved) {
				if (!is_occupied(state, dst_rank + 1, dst_file) && !is_occupied(state, dst_rank, dst_file)) {
					valid = MOVE_VALID;
				}
			}
		} else if ((dst_file == src_file - 1 || dst_file == src_file + 1) && dst_rank == src_rank - 1) {
			// capture move check en passant
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				// it can capture a piece, check 
				valid = MOVE_VALID;
			} else if (src_rank == 3) {
				Piece capture = state->board.piece[src_rank][dst_file];
				// @TODO TEST EN PASSANT
				if (capture == PIECE_WHITE_PAWN && state->en_passant_file == dst_file)
					valid = MOVE_VALID_EN_PASSANT;
			}
		}
	}break;

	case PIECE_WHITE_KING: {
		if ((src_rank + 1 >= dst_rank && src_rank - 1 <= dst_rank) &&			// is within 1 line
			(src_file + 1 >= dst_file && src_file - 1 <= dst_file))				// is within 1 column
		{
			valid = MOVE_VALID;
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				if (is_same_color_piece(moving, state->board.piece[dst_rank][dst_file]))
					valid = MOVE_INVALID;
			}
		} else if (src_rank == dst_rank && !state->king_state[PLAYER_WHITE].in_check) {
			if (state->king_state[PLAYER_WHITE].can_castle_kingside && dst_file == src_file + 2) {
				if (state->board.piece[0][6] == PIECE_NONE && state->board.piece[0][5] == PIECE_NONE)
					valid = MOVE_VALID_CASTLE_KINGSIDE;
				else
					return MOVE_INVALID;
			} else if (state->king_state[PLAYER_WHITE].can_castle_queenside && dst_file == src_file - 2) {
				if (state->board.piece[0][1] == PIECE_NONE && state->board.piece[0][2] == PIECE_NONE && state->board.piece[0][3] == PIECE_NONE)
					valid = MOVE_VALID_CASTLE_QUEENSIDE;
				else
					return MOVE_INVALID;
			}
		}
	}break;
	case PIECE_BLACK_KING: {
		if ((src_rank + 1 >= dst_rank && src_rank - 1 <= dst_rank) &&			// is within 1 line
			(src_file + 1 >= dst_file && src_file - 1 <= dst_file))				// is within 1 column
		{
			valid = MOVE_VALID;
			if (state->board.piece[dst_rank][dst_file] != PIECE_NONE) {
				if (is_same_color_piece(moving, state->board.piece[dst_rank][dst_file]))
					valid = MOVE_INVALID;
			}
		} else if (src_rank == dst_rank && !state->king_state[PLAYER_BLACK].in_check) {
			if (state->king_state[PLAYER_BLACK].can_castle_kingside && dst_file == src_file + 2) {
				if (state->board.piece[7][6] == PIECE_NONE && state->board.piece[7][5] == PIECE_NONE)
					valid = MOVE_VALID_CASTLE_KINGSIDE;
				else
					return MOVE_INVALID;
			} else if (state->king_state[PLAYER_BLACK].can_castle_queenside && dst_file == src_file - 2) {
				if (state->board.piece[7][1] == PIECE_NONE && state->board.piece[7][2] == PIECE_NONE && state->board.piece[7][3] == PIECE_NONE)
					valid = MOVE_VALID_CASTLE_QUEENSIDE;
				else
					return MOVE_INVALID;
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
					valid = MOVE_VALID;
					break;
				}
				continue;
			} else if (is_same_color_piece(moving, state->board.piece[i][j])) {
				valid = MOVE_INVALID;
				break;
			} else {
				if (i == dst_rank && j == dst_file) {
					valid = MOVE_VALID;
					break;
				} else {
					valid = MOVE_INVALID;
					break;
				}
			}
		}

	}break;

	case PIECE_WHITE_ROOK:
	case PIECE_BLACK_ROOK: {
		s32 y_dir = (dst_rank - src_rank < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
		s32 x_dir = (dst_file - src_file < 0) ? -1 : (dst_file - src_file == 0) ? 0 : 1;
		if (y_dir != 0 && x_dir != 0)
			return MOVE_INVALID;

		for (s32 j = src_file + x_dir, i = src_rank + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == dst_rank && j == dst_file) {
					valid = MOVE_VALID;
					break;
				}
				continue;
			} else if (is_same_color_piece(moving, state->board.piece[i][j])) {
				valid = MOVE_INVALID;
				break;
			} else {
				if (i == dst_rank && j == dst_file) {
					valid = MOVE_VALID;
					break;
				} else {
					valid = MOVE_INVALID;
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
			return MOVE_INVALID;

		for (s32 j = src_file + x_dir, i = src_rank + y_dir; j >= 0 && j < 8 && i >= 0 && i < 8; j += x_dir, i += y_dir) {
			if (state->board.piece[i][j] == PIECE_NONE) {
				if (i == dst_rank && j == dst_file) {
					valid = MOVE_VALID;
					break;
				}
				continue;
			} else if (is_same_color_piece(moving, state->board.piece[i][j])) {
				valid = MOVE_INVALID;
				break;
			} else {
				if (i == dst_rank && j == dst_file) {
					valid = MOVE_VALID;
					break;
				} else {
					valid = MOVE_INVALID;
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
			return MOVE_INVALID;
		else if ((y_diff == 2 && x_diff == 1) || (y_diff == 1 && x_diff == 2)) {
			if (is_same_color_piece(moving, state->board.piece[dst_rank][dst_file]))
				return MOVE_INVALID;
			valid = MOVE_VALID;
		}
		
	}break;
	}
	// check if the move leaves the king in check
	
	if (is_black_piece(moving) && is_black_in_check_after_black_move(state, src_rank, src_file, dst_rank, dst_file))
		return MOVE_INVALID;
	else if (is_white_piece(moving) && is_white_in_check_after_white_move(state, src_rank, src_file, dst_rank, dst_file))
		return MOVE_INVALID;
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
	game_state->king_state[PLAYER_WHITE].can_castle_kingside = true;
	game_state->king_state[PLAYER_WHITE].can_castle_queenside = true;

	game_state->king_state[PLAYER_BLACK].in_check = false;
	game_state->king_state[PLAYER_BLACK].rank = 7;
	game_state->king_state[PLAYER_BLACK].file = 4;
	game_state->king_state[PLAYER_BLACK].can_castle_kingside = true;
	game_state->king_state[PLAYER_BLACK].can_castle_queenside = true;
}

void pawn_push_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece p = state->board.piece[src_rank][src_file];
	assert(p == PIECE_BLACK_PAWN || p == PIECE_WHITE_PAWN);
	state->board.piece[dst_rank][dst_file] = p;
	state->board.piece[src_rank][src_file] = PIECE_NONE;
}
void pawn_capture_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece p = state->board.piece[src_rank][src_file];
	assert(p == PIECE_BLACK_PAWN || p == PIECE_WHITE_PAWN);
	state->board.piece[dst_rank][dst_file] = p;
	state->board.piece[src_rank][src_file] = PIECE_NONE;
}
void pawn_capture_en_passant_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	Piece p = state->board.piece[src_rank][src_file];
	assert(p == PIECE_BLACK_PAWN || p == PIECE_WHITE_PAWN);
	state->board.piece[dst_rank][dst_file] = p;
	state->board.piece[src_rank][dst_file] = PIECE_NONE;
	state->board.piece[src_rank][src_file] = PIECE_NONE;
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
	if (is_white_piece(moving) && state->turn != PLAYER_WHITE)
		return false;
	if (is_black_piece(moving) && state->turn != PLAYER_BLACK)
		return false;

	s32 move_valid = is_valid(state, src_rank, src_file, dst_rank, dst_file);
	if (move_valid) {
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
			if ((moving == PIECE_BLACK_PAWN && src_rank - dst_rank == 2) || (moving == PIECE_WHITE_PAWN && dst_rank - src_rank == 2))
				state->en_passant_file = src_file;
			else {
				if (moving == PIECE_WHITE_KING) {
					if (move_valid == MOVE_VALID_CASTLE_KINGSIDE) {
						board_put(state, PIECE_WHITE_ROOK, src_rank, dst_file - 1);
						board_put(state, PIECE_NONE, src_rank, 7);
					} else if (move_valid == MOVE_VALID_CASTLE_QUEENSIDE) {
						board_put(state, PIECE_WHITE_ROOK, src_rank, dst_file + 1);
						board_put(state, PIECE_NONE, src_rank, 0);
					}
					state->king_state[PLAYER_WHITE].can_castle_kingside = false;
					state->king_state[PLAYER_WHITE].can_castle_queenside = false;
				} else if (moving == PIECE_BLACK_KING) {
					if (move_valid == MOVE_VALID_CASTLE_KINGSIDE) {
						board_put(state, PIECE_BLACK_ROOK, src_rank, dst_file - 1);
						board_put(state, PIECE_NONE, src_rank, 7);
					} else if (move_valid == MOVE_VALID_CASTLE_QUEENSIDE) {
						board_put(state, PIECE_BLACK_ROOK, src_rank, dst_file + 1);
						board_put(state, PIECE_NONE, src_rank, 0);
					}
					state->king_state[PLAYER_BLACK].can_castle_kingside = false;
					state->king_state[PLAYER_BLACK].can_castle_queenside = false;
				} else if (moving == PIECE_WHITE_ROOK || moving == PIECE_BLACK_ROOK) {
					if (state->king_state[state->turn].can_castle_kingside && src_file == 7)
						state->king_state[state->turn].can_castle_kingside = false;
					else if(state->king_state[state->turn].can_castle_queenside && src_file == 0)
						state->king_state[state->turn].can_castle_queenside = false;
				}
				state->en_passant_file = -1;
			}
			board_put(state, moving, dst_rank, dst_file);
		}
		if (move_valid == MOVE_VALID_EN_PASSANT) {
			if(state->turn == PLAYER_WHITE)
				board_put(state, PIECE_NONE, dst_rank - 1, dst_file);
			else
				board_put(state, PIECE_NONE, dst_rank + 1, dst_file);
		}
		board_put(state, PIECE_NONE, src_rank, src_file);
		return true;
	} else {
		return false;
	}
}

Piece get_piece_from_character(s8 color, s8 piece) {
	if (color == 'B' || color == 'b') {
		switch (piece) {
		case 'p':
		case 'P': return PIECE_BLACK_PAWN;
		case 'b':
		case 'B': return PIECE_BLACK_BISHOP;
		case 'n':
		case 'N': return PIECE_BLACK_KNIGHT;
		case 'r':
		case 'R': return PIECE_BLACK_ROOK;
		case 'q':
		case 'Q': return PIECE_BLACK_QUEEN;
		case 'k':
		case 'K': return PIECE_BLACK_KING;
		default:  return PIECE_NONE;
		}
	} else if(color == 'W' || color == 'w') {
		switch (piece) {
		case 'p':
		case 'P': return PIECE_WHITE_PAWN;
		case 'b':
		case 'B': return PIECE_WHITE_BISHOP;
		case 'n':
		case 'N': return PIECE_WHITE_KNIGHT;
		case 'r':
		case 'R': return PIECE_WHITE_ROOK;
		case 'q':
		case 'Q': return PIECE_WHITE_QUEEN;
		case 'k':
		case 'K': return PIECE_WHITE_KING;
		default:  return PIECE_NONE;
		}
	}
	return PIECE_NONE;
}

void fill_text_move(s8* buffer, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file) {
	buffer[0] = src_file + 0x61;
	buffer[1] = src_rank + 0x31;
	buffer[2] = dst_file + 0x61;
	buffer[3] = dst_rank + 0x31;
}