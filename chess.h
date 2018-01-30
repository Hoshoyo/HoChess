#pragma once
#include "common.h"
#include <string.h>

enum Player {
	PLAYER_WHITE,
	PLAYER_BLACK,
	PLAYER_NUMBER,
};

enum Piece {
	PIECE_NONE = 0,
	
	// White
	PIECE_WHITE_PAWN,
	PIECE_WHITE_KING,
	PIECE_WHITE_QUEEN,
	PIECE_WHITE_ROOK,
	PIECE_WHITE_BISHOP,
	PIECE_WHITE_KNIGHT,

	// Black
	PIECE_BLACK_PAWN,
	PIECE_BLACK_KING,
	PIECE_BLACK_QUEEN,
	PIECE_BLACK_ROOK,
	PIECE_BLACK_BISHOP,
	PIECE_BLACK_KNIGHT,

	NUM_PIECES,
};

struct Board {
	// rank file
	Piece piece[8][8];
	bool  selected[8][8];
};

struct King_State {
	s32 rank;
	s32 file;
	bool in_check;
	bool can_castle_kingside;
	bool can_castle_queenside;
};

struct Game_State {
	Player turn;
	s32 move_count = 0;
	s32 move_draw = 0;			// number of moves without advancing a pawn or capturing a piece
	s32 en_passant_file = -1;
	King_State king_state[PLAYER_NUMBER];
	Board board;
};

void init_game_state(Game_State* game_state);

void board_clear(Game_State* game_state);
void board_init(Game_State* game_state);
void board_put(Game_State* game_state, Piece piece, s32 line, s32 column);

s32 is_valid(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);
bool interpret_move(Game_State* state, s8* buffer, u32 length);
void switch_turn(Game_State* state);
bool is_inside_board(s32 rank, s32 file);

Piece get_piece_from_character(s8 color, s8 piece);
void fill_text_move(s8* buffer, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);

bool is_white_piece(Piece p);
bool is_black_piece(Piece p);

// Move Pieces
void pawn_push_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);
Piece pawn_capture_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);
Piece pawn_capture_en_passant_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);
void undo_pawn_push_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);
void undo_pawn_capture_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Piece p);
void undo_pawn_capture_en_passant_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file, Piece p);


void bishop_move_unchecked(Game_State* state, s32 src_rank, s32 src_file, s32 dst_rank, s32 dst_file);

const s32 MOVE_INVALID = 0;
const s32 MOVE_VALID = 1;
const s32 MOVE_VALID_EN_PASSANT = 2;
const s32 MOVE_VALID_CASTLE_KINGSIDE = 3;
const s32 MOVE_VALID_CASTLE_QUEENSIDE = 4;