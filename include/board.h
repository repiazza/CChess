#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <consts.h>
#include <colors.h>
// Lados do tabuleiro
#define FRIENDLY_SIDE 0
#define ENEMY_SIDE    1
#define BLANK_SIDE    2

// Total de peças
#define TOTAL_PIECE_COUNT 32
#define PLAYER_PIECE_COUNT (TOTAL_PIECE_COUNT / 2)

// Dimensões do tabuleiro
#define ROW_SQUARE_COUNT 8
#define COLUMN_SQUARE_COUNT 8

// Cores das casas
#define DARK_BGCOLOR  0
#define LIGHT_BGCOLOR 1

#define BLANK_SQUARE_COLOR 0

#define SQUARE_SIZE 80


// Tipos de peças
#define SQUARE_TYPE_PAWN_PIECE       "PAWNPIECE"
#define SQUARE_TYPE_HIGHVALUE_PIECE  "NOTPAWNPIECE"
#define SQUARE_TYPE_KNIGHT_PIECE     "KNIGHTPIECE"
#define SQUARE_TYPE_BISHOP_PIECE     "BISHOPPIECE"
#define SQUARE_TYPE_QUEEN_PIECE      "QUEENPIECE"
#define SQUARE_TYPE_KING_PIECE       "KINGPIECE"
#define SQUARE_TYPE_ROOK_PIECE       "ROOKPIECE"
#define SQUARE_TYPE_BLANK            "BLANK"

// Estruturas para o tabuleiro
typedef struct STRUCT_SQUARE {
  const char *pszType;  // Adicionado const para strings constantes
  uint8_t ui8Color;
  uint8_t ui8Side;
  int bHighlighted;
} STRUCT_SQUARE, *PSTRUCT_SQUARE;


void vHandleMouseClickEvent(SDL_Event *pEvent, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
// Inicialização do tabuleiro
void vInitializeBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);

// Funções auxiliares
void vPrintBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
void vHighlightSquare(PSTRUCT_SQUARE pSquare,   int bHighlighted);
void vHighlightMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vClearHighlights(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);


#endif // BOARD_H
