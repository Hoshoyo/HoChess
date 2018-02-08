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

typedef u64 Board_Select;
typedef s8 Piece;

const u64 GAME_STATE_WHITE_CASTLE_KINGSIDE  = FLAG(0);
const u64 GAME_STATE_WHITE_CASTLE_QUEENSIDE = FLAG(1);
const u64 GAME_STATE_BLACK_CASTLE_KINGSIDE  = FLAG(2);
const u64 GAME_STATE_BLACK_CASTLE_QUEENSIDE = FLAG(3);
const u64 GAME_STATE_WHITE_IN_CHECK = FLAG(4);
const u64 GAME_STATE_BLACK_IN_CHECK = FLAG(5);
const u64 GAME_STATE_WHITE_TO_MOVE = FLAG(6);
const u64 GAME_STATE_BLACK_TO_MOVE = FLAG(7);
struct Game_State {
	u32 flags;
	s8 en_passant_file;
	s8  half_move_clock;	// 50 move rule
	s16 full_move_count;
};

struct Position {
	s8 rank;
	s8 file;
};

struct Move {
	s8 src_rank;
	s8 src_file;
	s8 dst_rank;
	s8 dst_file;
	Piece promoting_piece;
};

static u32 possible_moves_count = 0;
static Move possible_moves[1024] = {};

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

static bool black_in_check(Board* board, s8 king_rank = -1, s8 king_file = -1) {
	Position king_pos;
	if (king_rank != -1 && king_file != -1) {
		king_pos.rank = king_rank;
		king_pos.file = king_file;
	} else {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				Piece p = board->square[i][j];
				if (p == PIECE_BLACK_KING) {
					king_pos.file = j;
					king_pos.rank = i;
					goto found_king;
				}
			}
		}
	}
found_king:

	// by pawn
	if (king_pos.rank > 1) {
		// can be attacked by pawn
		if (king_pos.file > 0 && board->square[king_pos.rank + 1][king_pos.file - 1] == PIECE_WHITE_PAWN)
			return true;
		if (king_pos.file < 7 && board->square[king_pos.rank + 1][king_pos.file + 1] == PIECE_WHITE_PAWN)
			return true;
	}

	// by bishop or queen diagonal
	{
		// top left
		for (s32 i = king_pos.rank + 1, j = king_pos.file - 1; i < 8 && j >= 0; ++i, --j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_BISHOP || p == PIECE_WHITE_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// top right
		for (s32 i = king_pos.rank + 1, j = king_pos.file + 1; i < 8 && j < 8; ++i, ++j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_BISHOP || p == PIECE_WHITE_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// bot left
		for (s32 i = king_pos.rank - 1, j = king_pos.file - 1; i >= 0 && j >= 0; --i, --j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_BISHOP || p == PIECE_WHITE_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// bot right
		for (s32 i = king_pos.rank - 1, j = king_pos.file + 1; i >= 0, j < 8; --i, ++j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_BISHOP || p == PIECE_WHITE_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}
	}

	// by rook or queen aligned
	{
		// up
		for (s32 i = king_pos.rank + 1; i < 8; ++i) {
			Piece p = board->square[i][king_pos.file];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_ROOK || p == PIECE_WHITE_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// down
		for (s32 i = king_pos.rank - 1; i >= 0; --i) {
			Piece p = board->square[i][king_pos.file];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_ROOK || p == PIECE_WHITE_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// left
		for (s32 i = king_pos.file - 1; i >= 0; --i) {
			Piece p = board->square[king_pos.rank][i];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_ROOK || p == PIECE_WHITE_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// right
		for (s32 i = king_pos.file + 1; i < 8; ++i) {
			Piece p = board->square[king_pos.rank][i];
			if (p == 0)
				continue;
			if (PIECE_WHITE(p) && (p == PIECE_WHITE_ROOK || p == PIECE_WHITE_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}
	}

	// by knight
	{
		s8 rank = king_pos.rank;
		s8 file = king_pos.file;

		// L top
		rank += 2;
		file -= 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;
		file += 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;

		// L bot
		rank -= 2 + 2;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;
		file -= 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;

		// L left
		rank = king_pos.rank;
		file = king_pos.file;

		rank -= 1;
		file -= 2;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;
		rank += 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;

		// L right
		file += 2 + 2;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;
		rank -= 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_WHITE_KNIGHT)
			return true;
	}
	// by queen -> covered on bishop and rook cases

	return false;
}


static bool white_in_check(Board* board, s8 king_rank, s8 king_file) {
	Position king_pos;
	if (king_rank != -1 && king_file != -1) {
		king_pos.rank = king_rank;
		king_pos.file = king_file;
	} else {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				Piece p = board->square[i][j];
				if (p == PIECE_WHITE_KING) {
					king_pos.file = j;
					king_pos.rank = i;
					goto found_king;
				}
			}
		}
	}
found_king:

	// by pawn
	if (king_pos.rank < 6) {
		// can be attacked by pawn
		if (king_pos.file > 0 && board->square[king_pos.rank + 1][king_pos.file - 1] == PIECE_BLACK_PAWN)
			return true;
		if (king_pos.file < 7 && board->square[king_pos.rank + 1][king_pos.file + 1] == PIECE_BLACK_PAWN)
			return true;
	}

	// by bishop or queen diagonal
	{
		// top left
		for (s32 i = king_pos.rank + 1, j = king_pos.file - 1; i < 8 && j >= 0; ++i, --j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_BISHOP || p == PIECE_BLACK_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// top right
		for (s32 i = king_pos.rank + 1, j = king_pos.file + 1; i < 8 && j < 8; ++i, ++j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_BISHOP || p == PIECE_BLACK_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// bot left
		for (s32 i = king_pos.rank - 1, j = king_pos.file - 1; i >= 0 && j >= 0; --i, --j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_BISHOP || p == PIECE_BLACK_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// bot right
		for (s32 i = king_pos.rank - 1, j = king_pos.file + 1; i >= 0, j < 8; --i, ++j) {
			Piece p = board->square[i][j];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_BISHOP || p == PIECE_BLACK_QUEEN)) {
				// is black bishop or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}
	}

	// by rook or queen aligned
	{
		// up
		for (s32 i = king_pos.rank + 1; i < 8; ++i) {
			Piece p = board->square[i][king_pos.file];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_ROOK || p == PIECE_BLACK_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// down
		for (s32 i = king_pos.rank - 1; i >= 0; --i) {
			Piece p = board->square[i][king_pos.file];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_ROOK || p == PIECE_BLACK_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// left
		for (s32 i = king_pos.file - 1; i >= 0; --i) {
			Piece p = board->square[king_pos.rank][i];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_ROOK || p == PIECE_BLACK_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}

		// right
		for (s32 i = king_pos.file + 1; i < 8; ++i) {
			Piece p = board->square[king_pos.rank][i];
			if (p == 0)
				continue;
			if (PIECE_BLACK(p) && (p == PIECE_BLACK_ROOK || p == PIECE_BLACK_QUEEN)) {
				// is black rook or queen aligned with the king
				return true;
			} else {
				// is white piece, hence is blocking check
				break;
			}
		}
	}

	// by knight
	{
		s8 rank = king_pos.rank;
		s8 file = king_pos.file;

		// L top
		rank += 2;
		file -= 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;
		file += 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;

		// L bot
		rank -= 2 + 2;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;
		file -= 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;

		// L left
		rank = king_pos.rank;
		file = king_pos.file;

		rank -= 1;
		file -= 2;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;
		rank += 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;

		// L right
		file += 2 + 2;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;
		rank -= 1 + 1;
		if (rank < 8 && file >= 0 && board->square[rank][file] == PIECE_BLACK_KNIGHT)
			return true;
	}
	// by queen -> covered on bishop and rook cases

	return false;
}

static bool bishop_valid_move(Board* board, Move move, Game_State* state) {
	s8 src_rank = move.src_rank;
	s8 src_file = move.src_file;
	s8 dst_rank = move.dst_rank;
	s8 dst_file = move.dst_file;

	if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8)
		return false;

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

	// @TODO(psv): check pinned piece

	return true;
}

static bool rook_valid_move(Board* board, Move move, Game_State* state) {
	s8 src_rank = move.src_rank;
	s8 src_file = move.src_file;
	s8 dst_rank = move.dst_rank;
	s8 dst_file = move.dst_file;

	if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8)
		return false;

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

	// @TODO(psv): check pinned piece

	return true;
}

static bool queen_valid_move(Board* board, Move move, Game_State* state) {
	s8 src_rank = move.src_rank;
	s8 src_file = move.src_file;
	s8 dst_rank = move.dst_rank;
	s8 dst_file = move.dst_file;

	if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8)
		return false;

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

	// @TODO(psv): check pinned piece

	return true;
}

static bool knight_valid_move(Board* board, Move move, Game_State* state) {
	s8 src_rank = move.src_rank;
	s8 src_file = move.src_file;
	s8 dst_rank = move.dst_rank;
	s8 dst_file = move.dst_file;

	if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8)
		return false;

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

	// @TODO(psv): check pinned piece

	return false;
}

//
// Note(psv): This requires little endianess
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

static bool king_valid_move(Board* board, Move move, Game_State* state) {
	s8 src_rank = move.src_rank;
	s8 src_file = move.src_file;
	s8 dst_rank = move.dst_rank;
	s8 dst_file = move.dst_file;

	if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8)
		return false;

	s32 rank_diff = dst_rank - src_rank;
	s32 file_diff = dst_file - src_file;

	if (rank_diff == 0 && file_diff == 0)
		return false;

	Piece p = board->square[src_rank][src_file];
	// if normal move (1 square distance)
	if (rank_diff >= -1 && rank_diff <= 1 && file_diff >= -1 && file_diff <= 1) {
		Piece checked = board->square[dst_rank][dst_file];
		bool in_check = false;
		if (PIECE_WHITE(p)) {
			board->square[src_rank][src_file] = PIECE_NONE;
			in_check = white_in_check(board, dst_rank, dst_file);
			board->square[src_rank][src_file] = p;
		} else if (PIECE_BLACK(p)) {
			board->square[src_rank][src_file] = PIECE_NONE;
			in_check = black_in_check(board, dst_rank, dst_file);
			board->square[src_rank][src_file] = p;
		}
		if (checked == PIECE_NONE) {
			return !in_check;
		}
		return (!(is_same_color_piece(p, checked)) && !in_check);
	}

	// otherwise try castle

	// if in check can't castle
	bool in_check = false;
	if (PIECE_WHITE(p)) {
		in_check = white_in_check(board, dst_rank, dst_file);
	} else if (PIECE_BLACK(p)) {
		in_check = black_in_check(board, dst_rank, dst_file);
	}
	if (in_check) return false;

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

static bool pawn_valid_move(Board* board, Move move, Game_State* state) {
	s8 src_rank = move.src_rank;
	s8 src_file = move.src_file;
	s8 dst_rank = move.dst_rank;
	s8 dst_file = move.dst_file;

	if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8)
		return false;

	Piece p = board->square[src_rank][src_file];
	Piece dst = board->square[dst_rank][dst_file];

	if (dst && is_same_color_piece(p, dst))
		return false;

	if (src_file - dst_file < -1 || src_file - dst_file > 1)
		return false;

	if (PIECE_WHITE(p)) {
		if (dst_rank - src_rank == 1) {
			if (src_file == dst_file) {
				// advance once
				if (dst == PIECE_NONE) {
					// promoting
					if (dst_rank == 7) {
						if (move.promoting_piece >= PIECE_WHITE_BISHOP && move.promoting_piece <= PIECE_WHITE_QUEEN)
							return true;
						return false;
					}
					return true;
				}
			} else {
				// capture
				if (PIECE_BLACK(dst)) {
					if (dst_rank == 7) {
						if (move.promoting_piece >= PIECE_WHITE_BISHOP && move.promoting_piece <= PIECE_WHITE_QUEEN)
							return true;
						return false;
					}
					return true;
				} else if (src_rank == 4 && state->en_passant_file == dst_file) {
					return true;
				}
			}
		} else if (src_rank == 1 && dst_rank == 3) {
			// advance twice
			if (src_file == dst_file && dst == PIECE_NONE && board->square[dst_rank - 1][dst_file] == PIECE_NONE)
				return true;
		}
	} else if (PIECE_BLACK(p)) {
		if (src_rank - dst_rank == 1) {
			if (src_file == dst_file) {
				// advance once
				if (dst == PIECE_NONE) {
					// promoting
					if (dst_rank == 0) {
						if (move.promoting_piece >= PIECE_BLACK_BISHOP && move.promoting_piece <= PIECE_BLACK_QUEEN)
							return true;
						return false;
					}
					return true;
				}
			} else {
				// capture
				if (PIECE_WHITE(dst)) {
					if (dst_rank == 0) {
						if (move.promoting_piece >= PIECE_BLACK_BISHOP && move.promoting_piece <= PIECE_BLACK_QUEEN)
							return true;
						return false;
					}
					return true;
				} else if (src_rank == 3 && state->en_passant_file == dst_file) {
					return true;
				}
			}
		} else if (src_rank == 7 && dst_rank == 5) {
			// advance twice
			if (src_file == dst_file && dst == PIECE_NONE && board->square[dst_rank + 1][dst_file] == PIECE_NONE)
				return true;
		}
	}
	return false;
}

#define PUSH_MOVE(M) moves[(*num_moves)++] = M

// Generate moves
static void generate_pawn_moves(Position pos, Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	Move mv;
	mv.src_file = pos.file;
	mv.src_rank = pos.rank;

	s8 move_direction = 1;
	s8 black_piece_flag = 0;
	s8 seventh_rank = 6;
	if (game_state->flags & GAME_STATE_BLACK_TO_MOVE) {
		move_direction = -1;
		black_piece_flag = 8;
		seventh_rank = 2;
	}

	// advance once
	{
		mv.dst_file = pos.file;
		mv.dst_rank = pos.rank + move_direction;

		// advance once promote
		if (pos.rank == seventh_rank) {
			mv.promoting_piece = PIECE_WHITE_QUEEN | black_piece_flag;
			if (pawn_valid_move(board, mv, game_state)) {
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_ROOK | black_piece_flag;
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_KNIGHT | black_piece_flag;
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_BISHOP | black_piece_flag;
				PUSH_MOVE(mv);
			}
		} else {
			if (pawn_valid_move(board, mv, game_state)) {
				PUSH_MOVE(mv);
			}
		}
	}

	// advance twice
	{
		mv.promoting_piece = PIECE_NONE;
		mv.dst_file = pos.file;
		mv.dst_rank = pos.rank + 2 * move_direction;
		if (pawn_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
	}

	// capture normal - en passant
	{
		mv.dst_rank = pos.rank + move_direction;
		if (pos.rank == seventh_rank) {
			// capture promote
			mv.dst_file = pos.file + 1;
			mv.promoting_piece = PIECE_WHITE_QUEEN | black_piece_flag;
			if (pawn_valid_move(board, mv, game_state)) {
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_ROOK | black_piece_flag;
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_KNIGHT | black_piece_flag;
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_BISHOP | black_piece_flag;
				PUSH_MOVE(mv);
			}

			mv.dst_file = pos.file - 1;
			mv.promoting_piece = PIECE_WHITE_QUEEN | black_piece_flag;
			if (pawn_valid_move(board, mv, game_state)) {
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_ROOK | black_piece_flag;
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_KNIGHT | black_piece_flag;
				PUSH_MOVE(mv);
				mv.promoting_piece = PIECE_WHITE_BISHOP | black_piece_flag;
				PUSH_MOVE(mv);
			}
		} else {
			mv.promoting_piece = PIECE_NONE;
			mv.dst_file = pos.file + 1;
			if (pawn_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);

			mv.dst_file = pos.file - 1;
			if (pawn_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
		}
	}
}
static void generate_bishop_moves(Position pos, Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	Move mv;
	mv.src_file = pos.file;
	mv.src_rank = pos.rank;

	// top left
	for (s32 i = pos.rank + 1, j = pos.file -1 ; i < 8 && j >= 0; ++i, --j) {
		mv.dst_file = j;
		mv.dst_rank = i;
		if (bishop_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}

	// top right
	for (s32 i = pos.rank + 1, j = pos.file + 1; i < 8 && j < 8; ++i, ++j) {
		mv.dst_file = j;
		mv.dst_rank = i;
		if (bishop_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}

	// bot left
	for (s32 i = pos.rank - 1, j = pos.file - 1; i >= 0 && j >= 0; --i, --j) {
		mv.dst_file = j;
		mv.dst_rank = i;
		if (bishop_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}

	// bot right
	for (s32 i = pos.rank - 1, j = pos.file + 1; i >= 0, j < 8; --i, ++j) {
		mv.dst_file = j;
		mv.dst_rank = i;
		if (bishop_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}
}
static void generate_knight_moves(Position pos, Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	Move mv;
	mv.src_file = pos.file;
	mv.src_rank = pos.rank;

	// L up left
	mv.dst_file = pos.file - 1;
	mv.dst_rank = pos.rank + 2;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L up right
	mv.dst_file = pos.file + 1;
	mv.dst_rank = pos.rank + 2;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L down left
	mv.dst_file = pos.file - 1;
	mv.dst_rank = pos.rank - 2;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L down right
	mv.dst_file = pos.file + 1;
	mv.dst_rank = pos.rank - 2;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L right up
	mv.dst_file = pos.file + 2;
	mv.dst_rank = pos.rank + 1;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L right down
	mv.dst_file = pos.file + 2;
	mv.dst_rank = pos.rank - 1;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L left up
	mv.dst_file = pos.file - 2;
	mv.dst_rank = pos.rank + 1;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);

	// L left down
	mv.dst_file = pos.file - 2;
	mv.dst_rank = pos.rank - 1;
	if (knight_valid_move(board, mv, game_state))
		PUSH_MOVE(mv);
}
static void generate_rook_moves(Position pos, Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	Move mv;
	mv.src_file = pos.file;
	mv.src_rank = pos.rank;

	// up
	mv.dst_file = pos.file;
	for (s32 i = pos.rank + 1; i < 8; ++i) {
		mv.dst_rank = i;
		if (rook_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}

	// down
	for (s32 i = pos.rank - 1; i >= 0; --i) {
		mv.dst_rank = i;
		if (rook_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}

	// left
	mv.dst_rank = pos.rank;
	for (s32 i = pos.file - 1; i >= 0; --i) {
		mv.dst_file = i;
		if (rook_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}

	// right
	for (s32 i = pos.file + 1; i < 8; ++i) {
		mv.dst_file = i;
		if (rook_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		else
			break;
	}
}
static void generate_queen_moves(Position pos, Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	generate_rook_moves(pos, game_state, board, moves, num_moves);
	generate_bishop_moves(pos, game_state, board, moves, num_moves);
}
static void generate_king_moves(Position pos, Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	Move mv;
	mv.src_file = pos.file;
	mv.src_rank = pos.rank;

	{
		mv.dst_rank = pos.rank;

		mv.dst_file = pos.file - 1;
		if (king_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		mv.dst_file = pos.file + 1;
		if (king_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);

		if (pos.rank < 7) {
			// up
			mv.dst_rank = pos.rank + 1;
			mv.dst_file = pos.file;
			if (king_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
			mv.dst_file = pos.file - 1;
			if (king_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
			mv.dst_file = pos.file + 1;
			if (king_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
		}

		if (pos.rank > 0) {
			// down
			mv.dst_rank = pos.rank - 1;
			mv.dst_file = pos.file;
			if (king_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
			mv.dst_file = pos.file - 1;
			if (king_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
			mv.dst_file = pos.file + 1;
			if (king_valid_move(board, mv, game_state))
				PUSH_MOVE(mv);
		}
	}

	if ((game_state->flags & GAME_STATE_WHITE_TO_MOVE) && pos.file == 4 && pos.rank == 0) {
		mv.dst_rank = pos.rank;
		mv.dst_file = pos.file - 2;
		if (king_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		mv.dst_file = pos.file + 2;
		if (king_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
	} else if(pos.file == 4 && pos.rank == 7) {
		mv.dst_rank = pos.rank;
		mv.dst_file = pos.file - 2;
		if (king_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
		mv.dst_file = pos.file + 2;
		if (king_valid_move(board, mv, game_state))
			PUSH_MOVE(mv);
	}
}

static void generate_possible_moves(Game_State* game_state, Board* board, Move* moves, u32* num_moves) {
	for (s32 i = 0; i < 8; ++i) {
		for (s32 j = 0; j < 8; ++j) {
			Piece p = board->square[i][j];
			Position pos = { i, j };
			switch (p) {
				case PIECE_BLACK_PAWN:
				case PIECE_WHITE_PAWN:		generate_pawn_moves(pos, game_state, board, moves, num_moves); break;
				case PIECE_BLACK_BISHOP:
				case PIECE_WHITE_BISHOP:	generate_bishop_moves(pos, game_state, board, moves, num_moves); break;
				case PIECE_BLACK_KNIGHT:
				case PIECE_WHITE_KNIGHT:	generate_knight_moves(pos, game_state, board, moves, num_moves); break;
				case PIECE_BLACK_ROOK:
				case PIECE_WHITE_ROOK:		generate_rook_moves(pos, game_state, board, moves, num_moves); break;
				case PIECE_BLACK_QUEEN:
				case PIECE_WHITE_QUEEN:		generate_queen_moves(pos, game_state, board, moves, num_moves); break;
				case PIECE_BLACK_KING:
				case PIECE_WHITE_KING:		generate_king_moves(pos, game_state, board, moves, num_moves); break;
				default: break;
			}
		}
	}
}

static bool is_number(s8 c) {
	if (c >= '0' && c <= '9')
		return true;
	return false;
}

static bool is_whitespace(s8 c) {
	return (c == ' ' || c == '\n' || c == '\v' || c == '\t' || c == '\r');
}

static s32 str_to_s32(char* text, int length)
{
	s32 result = 0;
	s32 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result;
}

enum Fen_Format {
	FEN_BOARD,
	FEN_TO_MOVE,
	FEN_CASTLING,
	FEN_EN_PASSANT,
	FEN_HALFMOVE,
	FEN_FULLMOVE,
	FEN_END,
};

static s32 parse_fen(s8* fen, Board* board, Game_State* state) {
	// startpos
	// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

	memset(board->square, PIECE_NONE, sizeof(board->square));
	state->flags = 0;
	state->en_passant_file = -1;
	state->full_move_count = 0;
	state->half_move_clock = 0;

	Fen_Format parsing_state = FEN_BOARD;

	s32 rank = 7;
	s32 file = 0;
	while (*fen != 0) {
		s8 c = *fen;
		if (is_whitespace(c)) {
			fen++;
			continue;
		}

		switch (parsing_state) 
		{
			case FEN_BOARD: {
				switch (c) 
				{
					case 'r':	board->square[rank][file] = PIECE_BLACK_ROOK; break;
					case 'n':	board->square[rank][file] = PIECE_BLACK_KNIGHT; break;
					case 'b':	board->square[rank][file] = PIECE_BLACK_BISHOP; break;
					case 'q':	board->square[rank][file] = PIECE_BLACK_QUEEN; break;
					case 'k':	board->square[rank][file] = PIECE_BLACK_KING; break;
					case 'p':	board->square[rank][file] = PIECE_BLACK_PAWN; break;

					case 'R':	board->square[rank][file] = PIECE_WHITE_ROOK; break;
					case 'N':	board->square[rank][file] = PIECE_WHITE_KNIGHT; break;
					case 'B':	board->square[rank][file] = PIECE_WHITE_BISHOP; break;
					case 'Q':	board->square[rank][file] = PIECE_WHITE_QUEEN; break;
					case 'K':	board->square[rank][file] = PIECE_WHITE_KING; break;
					case 'P':	board->square[rank][file] = PIECE_WHITE_PAWN; break;

					case '/': {
						rank -= 1;
						file = -1;
					} break;

					default: {
						if (is_number(c)) {
							s32 start = 0;
							while (start < (c - 0x30) - 1) {
								board->square[rank][file] = PIECE_NONE;
								start++;
								file++;
							}
						}
					}break;
				}

				if (file >= 7 && rank == 0) {
					parsing_state = FEN_TO_MOVE;
				} else {
					file += 1;
				}
			} break;

			case FEN_TO_MOVE: {
				if (c == 'w')
					state->flags |= GAME_STATE_WHITE_TO_MOVE;
				else if (c == 'b')
					state->flags |= GAME_STATE_BLACK_TO_MOVE;
				else
					return -1;
				parsing_state = FEN_CASTLING;
			}break;

			case FEN_CASTLING: {
				while (!is_whitespace(*fen)) {
					s8 c = *fen;
					if (c == '-')
						break;
					switch (c) {
						case 'K': state->flags |= GAME_STATE_WHITE_CASTLE_KINGSIDE; break;
						case 'Q': state->flags |= GAME_STATE_WHITE_CASTLE_QUEENSIDE; break;
						case 'k': state->flags |= GAME_STATE_BLACK_CASTLE_KINGSIDE; break;
						case 'q': state->flags |= GAME_STATE_BLACK_CASTLE_QUEENSIDE; break;
						default: return -1;
					}
					++fen;
				}
				parsing_state = FEN_EN_PASSANT;
			}break;

			case FEN_EN_PASSANT: {
				if (c == '-')
					parsing_state = FEN_HALFMOVE;
				else {
					if (c >= 'a' && c <= 'h') {
						state->en_passant_file = c - 0x61;
					}
					fen++;	// skip rank
					parsing_state = FEN_HALFMOVE;
				}
			}break;

			case FEN_HALFMOVE: {
				s32 length = 0;
				while (!is_whitespace(*(fen + length))) length++;
				state->half_move_clock = (s8)str_to_s32(fen, length);
				parsing_state = FEN_FULLMOVE;
			}break;

			case FEN_FULLMOVE: {
				s32 length = 0;
				while (!is_whitespace(*(fen + length)) && *(fen + length)) length++;
				state->full_move_count = (s8)str_to_s32(fen, length);
				parsing_state = FEN_END;
			}break;

			case FEN_END: return 0;
		}
		
		++fen;
	}

	return 0;
}