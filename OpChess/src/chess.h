#pragma once
#include <assert.h>
#include "common.h"

struct Board
{
	union {
		struct { s8 square[8][8]; };
		struct {
			u64 ranks[8];
		};
	};
};

const u64 GAME_STATE_WHITE_CASTLE_KINGSIDE  = FLAG(0);
const u64 GAME_STATE_WHITE_CASTLE_QUEENSIDE = FLAG(1);
const u64 GAME_STATE_BLACK_CASTLE_KINGSIDE  = FLAG(2);
const u64 GAME_STATE_BLACK_CASTLE_QUEENSIDE = FLAG(3);
const u64 GAME_STATE_WHITE_IN_CHECK = FLAG(4);
const u64 GAME_STATE_BLACK_IN_CHECK = FLAG(5);
struct Game_State {
	u32 flags;
	s8 en_passant_file;
	s8  half_move_clock;	// 50 move rule
	s16 full_move_count;
};

typedef u64 Board_Select;
typedef s8 Piece;
typedef u32 Move;

const Piece PIECE_NONE			= 0;

// test 4th bit 0 for white
const Piece PIECE_WHITE_PAWN	= 1;	// 0001
const Piece PIECE_WHITE_BISHOP	= 2;	// 0010
const Piece PIECE_WHITE_KNIGHT	= 3;	// 0011
const Piece PIECE_WHITE_ROOK	= 4;	// 0100
const Piece PIECE_WHITE_QUEEN	= 5;	// 0101
const Piece PIECE_WHITE_KING	= 6;	// 0110

// test 4th bit 1 for black
const Piece PIECE_BLACK_PAWN	= 9;	// 1001
const Piece PIECE_BLACK_BISHOP	= 10;	// 1010
const Piece PIECE_BLACK_KNIGHT	= 11;	// 1011
const Piece PIECE_BLACK_ROOK	= 12;	// 1100
const Piece PIECE_BLACK_QUEEN	= 13;	// 1101
const Piece PIECE_BLACK_KING	= 14;	// 1110

#define PIECE_BLACK(P) (((P) & 8) != 0)
#define PIECE_WHITE(P) ((((P) & 8) == 0) && ((P) != 0))

static void game_state_init(Game_State* game_state) {
	game_state->flags = 0 | GAME_STATE_WHITE_CASTLE_KINGSIDE | GAME_STATE_WHITE_CASTLE_QUEENSIDE | GAME_STATE_BLACK_CASTLE_KINGSIDE | GAME_STATE_BLACK_CASTLE_QUEENSIDE;
	game_state->en_passant_file = -1;
	game_state->half_move_clock = 0;
	game_state->full_move_count = 0;
}

static void board_startpos(Board* board) 
{
	board->square[0][0] = PIECE_WHITE_ROOK;
	board->square[0][1] = PIECE_WHITE_KNIGHT;
	board->square[0][2] = PIECE_WHITE_BISHOP;
	board->square[0][3] = PIECE_WHITE_QUEEN;
	board->square[0][4] = PIECE_WHITE_KING;
	board->square[0][5] = PIECE_WHITE_BISHOP;
	board->square[0][6] = PIECE_WHITE_KNIGHT;
	board->square[0][7] = PIECE_WHITE_ROOK;
	board->square[1][0] = PIECE_WHITE_PAWN;
	board->square[1][1] = PIECE_WHITE_PAWN;
	board->square[1][2] = PIECE_WHITE_PAWN;
	board->square[1][3] = PIECE_WHITE_PAWN;
	board->square[1][4] = PIECE_WHITE_PAWN;
	board->square[1][5] = PIECE_WHITE_PAWN;
	board->square[1][6] = PIECE_WHITE_PAWN;
	board->square[1][7] = PIECE_WHITE_PAWN;

	board->square[7][0] = PIECE_BLACK_ROOK;
	board->square[7][1] = PIECE_BLACK_KNIGHT;
	board->square[7][2] = PIECE_BLACK_BISHOP;
	board->square[7][3] = PIECE_BLACK_QUEEN;
	board->square[7][4] = PIECE_BLACK_KING;
	board->square[7][5] = PIECE_BLACK_BISHOP;
	board->square[7][6] = PIECE_BLACK_KNIGHT;
	board->square[7][7] = PIECE_BLACK_ROOK;
	board->square[6][0] = PIECE_BLACK_PAWN;
	board->square[6][1] = PIECE_BLACK_PAWN;
	board->square[6][2] = PIECE_BLACK_PAWN;
	board->square[6][3] = PIECE_BLACK_PAWN;
	board->square[6][4] = PIECE_BLACK_PAWN;
	board->square[6][5] = PIECE_BLACK_PAWN;
	board->square[6][6] = PIECE_BLACK_PAWN;
	board->square[6][7] = PIECE_BLACK_PAWN;
}

static bool is_same_color_piece(Piece p1, Piece p2) {
	assert(p1 && p2);
	return !((p1 & 8) ^ (p2 & 8));
}

static bool bishop_valid_move(Board* board, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Game_State* state) {
	s32 rank_diff = dst_rank - src_rank;
	s32 file_diff = dst_file - src_file;

	if (rank_diff == 0 || file_diff == 0) return false;

	s32 rank_sign = (rank_diff < 0) ? -1 : 1;
	s32 file_sign = (file_diff < 0) ? -1 : 1;

	if (rank_diff * rank_sign - file_diff * file_sign != 0)
		return false;

	Piece p = board->square[src_rank][src_file];
	assert(p == PIECE_WHITE_BISHOP || p == PIECE_BLACK_BISHOP);

	for (s32 r = src_rank + rank_sign, f = src_file + file_sign;
		r != dst_rank + rank_sign && f != dst_file + file_sign;
		r += rank_sign, f += file_sign)
	{
		Piece checked = board->square[r][f];
		if (checked == PIECE_NONE)
			continue;
		if (is_same_color_piece(p, checked)) {
			return false;
		} else {
			return (dst_rank == r && dst_file == f);
		}
	}

	return true;
}

static bool rook_valid_move(Board* board, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Game_State* state) {
	s32 rank_sign = (dst_rank - src_rank < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
	s32 file_sign = (dst_file - src_file < 0) ? -1 : (dst_file - src_file == 0) ? 0 : 1;

	if ((rank_sign != 0 && file_sign != 0) || (rank_sign == 0 && file_sign == 0))
		return false;
	
	Piece p = board->square[src_rank][src_file];
	assert(p == PIECE_WHITE_ROOK || p == PIECE_BLACK_ROOK);

	for (s32 r = src_rank + rank_sign, f = src_file + file_sign;
		r != dst_rank + rank_sign || f != dst_file + file_sign;
		r += rank_sign, f += file_sign)
	{
		Piece checked = board->square[r][f];

		if (checked == PIECE_NONE)
			continue;
		if (is_same_color_piece(p, checked)) {
			return false;
		} else {
			return (dst_rank == r && dst_file == f);
		}
	}

	return true;
}

static bool queen_valid_move(Board* board, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Game_State* state) {
	s32 rank_diff = dst_rank - src_rank;
	s32 file_diff = dst_file - src_file;

	if (rank_diff == 0 && file_diff == 0) return false;

	s32 rank_sign = (dst_rank - src_rank < 0) ? -1 : (dst_rank - src_rank == 0) ? 0 : 1;
	s32 file_sign = (dst_file - src_file < 0) ? -1 : (dst_file - src_file == 0) ? 0 : 1;

	if (rank_diff * rank_sign - file_diff * file_sign != 0) {
		if (rank_sign != 0 && file_sign != 0)
			return false;
	}

	Piece p = board->square[src_rank][src_file];
	assert(p == PIECE_WHITE_QUEEN || p == PIECE_BLACK_QUEEN);

	for (s32 r = src_rank + rank_sign, f = src_file + file_sign;
		r != dst_rank + rank_sign || f != dst_file + file_sign;
		r += rank_sign, f += file_sign)
	{
		Piece checked = board->square[r][f];
		if (checked == PIECE_NONE)
			continue;
		if (is_same_color_piece(p, checked)) {
			return false;
		} else {
			return (dst_rank == r && dst_file == f);
		}
	}

	return true;
}

static bool knight_valid_move(Board* board, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Game_State* state) {
	s32 y_diff = dst_rank - src_rank;
	y_diff = (y_diff > 0) ? y_diff : -y_diff;
	s32 x_diff = dst_file - src_file;
	x_diff = (x_diff > 0) ? x_diff : -x_diff;

	Piece p = board->square[src_rank][src_file];
	assert(p == PIECE_WHITE_KNIGHT || p == PIECE_BLACK_KNIGHT);
	
	if ((y_diff == 2 && x_diff == 1) || (y_diff == 1 && x_diff == 2)) {
		Piece checked = board->square[dst_rank][dst_file];
		if (checked == PIECE_NONE)
			return true;
		return !(is_same_color_piece(p, checked));
	}
	return false;
}

//
// Note(psv): This requires on little endianess
//
// @TODO check here for attacks to these squares
inline bool white_king_side_castle_space_available(Board* board) {
	u64 backrank = board->ranks[0];
	return ((backrank & 0x00ffff0000000000) == 0);
}
// @TODO check here for attacks to these squares
inline bool black_king_side_castle_space_available(Board* board) {
	u64 backrank = board->ranks[7];
	return ((backrank & 0x00ffff0000000000) == 0);
}
// @TODO check here for attacks to these squares
inline bool white_queen_side_castle_space_available(Board* board) {
	u64 backrank = board->ranks[0];
	return ((backrank & 0x00000000ffffff00) == 0);
}
// @TODO check here for attacks to these squares
inline bool black_queen_side_castle_space_available(Board* board) {
	u64 backrank = board->ranks[7];
	return ((backrank & 0x00000000ffffff00) == 0);
}

static bool king_valid_move(Board* board, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Game_State* state) {
	s32 rank_diff = dst_rank - src_rank;
	s32 file_diff = dst_file - src_file;

	if (rank_diff == 0 && file_diff == 0)
		return false;

	Piece p = board->square[src_rank][src_file];

	// if normal move (1 square distance)
	if (rank_diff >= -1 && rank_diff <= 1 && file_diff >= -1 && file_diff <= 1) {
		Piece checked = board->square[dst_rank][dst_file];
		if (checked == PIECE_NONE)
			return true;
		return !(is_same_color_piece(p, checked));
	}

	// otherwise try castle
	if (PIECE_WHITE(p)) {
		if (file_diff == 2 && rank_diff == 0) {
			// WHITE KINGSIDE CASTLE
			if (state->flags & GAME_STATE_WHITE_IN_CHECK) {
				return false;
			}
			if (state->flags & GAME_STATE_WHITE_CASTLE_KINGSIDE) {
				return white_king_side_castle_space_available(board);
			}
		} else if (file_diff == -2 && rank_diff == 0) {
			// WHITE QUEENSIDE CASTLE
			if (state->flags & GAME_STATE_WHITE_IN_CHECK) {
				return false;
			}
			if (state->flags & GAME_STATE_WHITE_CASTLE_QUEENSIDE) {
				return white_queen_side_castle_space_available(board);
			}
		}
	} else if(PIECE_BLACK(p)) {
		if (file_diff == 2 && rank_diff == 0) {
			// BLACK KINGSIDE CASTLE
			if (state->flags & GAME_STATE_BLACK_IN_CHECK) {
				return false;
			}
			if (state->flags & GAME_STATE_BLACK_CASTLE_KINGSIDE) {
				return black_king_side_castle_space_available(board);
			}
		} else if (file_diff == -2 && rank_diff == 0) {
			// BLACK QUEENSIDE CASTLE
			if (state->flags & GAME_STATE_BLACK_IN_CHECK) {
				return false;
			}
			if (state->flags & GAME_STATE_BLACK_CASTLE_KINGSIDE) {
				return black_queen_side_castle_space_available(board);
			}
		}
	}

	return false;
}