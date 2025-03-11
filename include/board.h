#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <consts.h>
#include <colors.h>
/* Lados do tabuleiro */
#define NEUTRAL_SIDE  0
#define FRIENDLY_SIDE 1
#define ENEMY_SIDE    2

/* Total de peças */
#define TOTAL_PIECE_COUNT 32
#define PLAYER_PIECE_COUNT (TOTAL_PIECE_COUNT / 2)

/* Dimensões do tabuleiro */
#define ROW_SQUARE_COUNT 8
#define COLUMN_SQUARE_COUNT 8

/* Cores das casas */
#define DARK_BGCOLOR  0
#define LIGHT_BGCOLOR 1

#define BLANK_SQUARE_COLOR 0

#define SQUARE_SIZE 80


/* Tipos de peças */
#define SQUARE_TYPE_PAWN_PIECE       "PAWNPIECE"
#define SQUARE_TYPE_HIGHVALUE_PIECE  "NOTPAWNPIECE"
#define SQUARE_TYPE_KNIGHT_PIECE     "KNIGHTPIECE"
#define SQUARE_TYPE_BISHOP_PIECE     "BISHOPPIECE"
#define SQUARE_TYPE_QUEEN_PIECE      "QUEENPIECE"
#define SQUARE_TYPE_KING_PIECE       "KINGPIECE"
#define SQUARE_TYPE_ROOK_PIECE       "ROOKPIECE"
#define SQUARE_TYPE_BLANK            "BLANK"

#define SQUARE_THREAT_NONE     0x00
#define SQUARE_THREAT_FRIENDLY 0x01
#define SQUARE_THREAT_ENEMY    0x02
#define SQUARE_THREAT_BOTH (SQUARE_THREAT_FRIENDLY | SQUARE_THREAT_ENEMY)

/* Estruturas para o tabuleiro */
typedef struct STRUCT_SQUARE {
  char *pszType;  /* Adicionado const para strings constantes */
  uint8_t ui8Color;
  uint8_t ui8Side;
  int bHighlighted;
  int bSelected;
  int bHasMoved;
  uint8_t ui8Threat;
} STRUCT_SQUARE, *PSTRUCT_SQUARE, **PPSTRUCT_SQUARE;

extern const char *gkpszThreatType[];

void vClearHighlights(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
PSTRUCT_SQUARE pCreateSquare(const char *pszType, uint8_t ui8Color, uint8_t ui8Side);
void vSetSquare(STRUCT_SQUARE *pSquare, char *pszType, uint8_t ui8Color, uint8_t ui8Side);
void vInitializeBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
void vPrintBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
// void vHighlightSquare(PSTRUCT_SQUARE pSquare,   int bHighlighted);
void vHighlightSquare(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vConvertBoard2String(char *pszOutput, size_t lOutputSize, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);

#endif /* BOARD_H */
