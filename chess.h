#pragma once
#include "common.h"

enum Player {
	PLAYER_WHITE,
	PLAYER_BLACK,
};

enum Piece {
	PIECE_NONE = 0,
	
	// White
	PIECE_WHITE_PAWN_0,
	PIECE_WHITE_PAWN_1,
	PIECE_WHITE_PAWN_2,
	PIECE_WHITE_PAWN_3,
	PIECE_WHITE_PAWN_4,
	PIECE_WHITE_PAWN_5,
	PIECE_WHITE_PAWN_6,
	PIECE_WHITE_PAWN_7,

	PIECE_WHITE_KING,
	PIECE_WHITE_QUEEN,
	PIECE_WHITE_ROOK_0,
	PIECE_WHITE_ROOK_1,
	PIECE_WHITE_BISHOP_0,
	PIECE_WHITE_BISHOP_1,
	PIECE_WHITE_KNIGHT_0,
	PIECE_WHITE_KNIGHT_1,

	// Black
	PIECE_BLACK_PAWN_0,
	PIECE_BLACK_PAWN_1,
	PIECE_BLACK_PAWN_2,
	PIECE_BLACK_PAWN_3,
	PIECE_BLACK_PAWN_4,
	PIECE_BLACK_PAWN_5,
	PIECE_BLACK_PAWN_6,
	PIECE_BLACK_PAWN_7,

	PIECE_BLACK_KING,
	PIECE_BLACK_QUEEN,
	PIECE_BLACK_ROOK_0,
	PIECE_BLACK_ROOK_1,
	PIECE_BLACK_BISHOP_0,
	PIECE_BLACK_BISHOP_1,
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

	Board();
};

struct Game_State {
	Player turn;
	s32 move_count = 0;
	s32 move_draw = 0;		// number of moves without advancing a pawn or capturing a piece
	Piece_State piece_state[NUM_PIECES];
	Board board;
};
