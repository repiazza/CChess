#ifndef PIECE_H
#define PIECE_H

#include <stdint.h>
#include <board.h>
#include <movement.h>

// Constantes para tipos de peça
#define PIECE_TYPE_ROOK 'R'
#define PIECE_TYPE_KNIGHT 'N'
#define PIECE_TYPE_BISHOP 'B'
#define PIECE_TYPE_QUEEN 'Q'
#define PIECE_TYPE_KING 'K'
#define PIECE_TYPE_PAWN 'P'
#define PIECE_TYPE_NONE 0

// Cores
#define WHITE_COLOR 0
#define BLACK_COLOR 1

// Estrutura para representar uma peça
typedef struct STRUCT_PIECE
{
  char cType;       // Tipo da peça (R, N, B, Q, K, P)
  uint8_t ui8Color; // Cor da peça (WHITE_COLOR ou BLACK_COLOR)
  int bHasMoved;    // Flag indicando se a peça já se moveu
} STRUCT_PIECE, *PSTRUCT_PIECE;

// Funções relacionadas às peças
PSTRUCT_PIECE pCreatePiece(char cType, uint8_t ui8Color);
void vDestroyPiece(PSTRUCT_PIECE pPiece);
int bIsPieceValid(PSTRUCT_PIECE pPiece);
int bCanMove(PSTRUCT_PIECE pPiece, uint16_t ui16MovementFlags);
const char *pszGetPieceTypeName(char cType);
const char *pszGetPieceName(PSTRUCT_SQUARE square);

#endif // PIECE_H
