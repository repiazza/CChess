#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>
#include <piece.h>
#include <board.h>

void vHighlightRookMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
void vHighlightKnightMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vHighlightBishopMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vHighlightQueenMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
void vHighlightKingMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vHighlightPawnMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);

// Declarações das funções
void vGetPieceMovementAttributes(const char *pszType, uint8_t ui8Side, int *piMaxRange, uint16_t *pui16Directions);
int bPathIsClear(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iStartRow, int iStartCol, int iTargetRow, int iTargetCol);
int bValidateCastle(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iStartRow, int iStartCol, int iTargetRow, int iTargetCol);
int bValidateEnPassant(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iStartRow, int iStartCol, int iTargetRow, int iTargetCol);


// Direções básicas
#define NORTH 0x01
#define SOUTH 0x02
#define WEST  0x04
#define EAST  0x08

// Direções compostas
#define NORTH_WEST  ( NORTH | WEST )
#define NORTH_EAST  ( NORTH | EAST )
#define SOUTH_WEST  ( SOUTH | WEST )
#define SOUTH_EAST  ( SOUTH | EAST )

// Alcances e tipos de movimento
#define SQUARE_RANGE        1
#define DOUBLE_SQUARE_RANGE 2
#define L_RANGE             4
#define LINE_OF_SIGHT       8
#define RANGE_TYPE_NONE     0

#define MOVEMENT_TYPE_NONE  0

// Direções de movimento
#define MOVEMENT_DIRECTION_COLUMN    0x01
#define MOVEMENT_DIRECTION_LINE      0x02
#define MOVEMENT_DIRECTION_DIAGONAL  0x04
#define MOVEMENT_DIRECTION_L         0x08

// Subtipos de movimento
#define SUBTYPE_DIAG_MAIN_BEGIN      0x10
#define SUBTYPE_DIAG_MAIN_END        0x20
#define SUBTYPE_DIAG_OPPOSITE_BEGIN  0x40
#define SUBTYPE_DIAG_OPPOSITE_END    0x80

#define SUBTYPE_COLUMN_NORTH         0x400
#define SUBTYPE_COLUMN_SOUTH         0x800
#define SUBTYPE_LINE_WEST            0x1000
#define SUBTYPE_LINE_EAST            0x2000

// Movimentos especiais
#define SPECIAL_MOVEMENT_CASTLE      0x4000
#define SPECIAL_MOVEMENT_EN_PASSANT  0x8000
#define SPECIAL_MOVEMENT_PROMOTE     0x10000

// Combinações de movimentos
#define MOVEMENT_DIRECTION_ALL  \
    (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE | MOVEMENT_DIRECTION_DIAGONAL | MOVEMENT_DIRECTION_L)

#define SPECIAL_MOVEMENT_ALL \
    (SPECIAL_MOVEMENT_CASTLE | SPECIAL_MOVEMENT_EN_PASSANT | SPECIAL_MOVEMENT_PROMOTE)

#define MOVEMENT_TYPE_ALL \
    (MOVEMENT_DIRECTION_ALL | SPECIAL_MOVEMENT_ALL)

// Estrutura de Movimento
typedef struct STRUCT_MOVEMENT {
    int iStartX;
    int iStartY;
    int iEndX;
    int iEndY;
    uint16_t ui16MovementFlags;
} STRUCT_MOVEMENT, *PSTRUCT_MOVEMENT;

// Prototipação de funções
int iValidateMove(PSTRUCT_MOVEMENT pMovement);
void vCalculateDirections(int iX, int iY, int *aiDirections);
void vCapturePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iFromRow, int iFromCol, int iToRow, int iToCol);
void vMovePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iFromRow, int iFromCol, int iToRow, int iToCol);
void vCalculateAvailableMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, int iMaxRange, uint16_t ui16Directions, uint8_t ui8Side);


#endif // MOVEMENT_H
