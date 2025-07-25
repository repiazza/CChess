#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>
#include <piece.h>
#include <board.h>


/* Direções básicas */
#define NORTH 0x01
#define SOUTH 0x02
#define WEST  0x04
#define EAST  0x08

/* Direções compostas */
#define NORTH_WEST (NORTH | WEST)
#define NORTH_EAST (NORTH | EAST)
#define SOUTH_WEST (SOUTH | WEST)
#define SOUTH_EAST (SOUTH | EAST)

/* Alcances e tipos de movimento */
#define SQUARE_RANGE        1
#define DOUBLE_SQUARE_RANGE 2
#define L_RANGE             4
#define LINE_OF_SIGHT       8
#define RANGE_TYPE_NONE     0

#define MOVEMENT_TYPE_NONE 0

/* Direções de movimento */
#define MOVEMENT_DIRECTION_COLUMN   0x01
#define MOVEMENT_DIRECTION_LINE     0x02
#define MOVEMENT_DIRECTION_DIAGONAL 0x04
#define MOVEMENT_DIRECTION_L        0x08

/* Subtipos de movimento */
#define SUBTYPE_DIAG_MAIN_BEGIN     0x10
#define SUBTYPE_DIAG_MAIN_END       0x20
#define SUBTYPE_DIAG_OPPOSITE_BEGIN 0x40
#define SUBTYPE_DIAG_OPPOSITE_END   0x80

#define SUBTYPE_COLUMN_NORTH 0x400
#define SUBTYPE_COLUMN_SOUTH 0x800
#define SUBTYPE_LINE_WEST    0x1000
#define SUBTYPE_LINE_EAST    0x2000

/* Movimentos especiais */
#define SPECIAL_MOVEMENT_CASTLE     0x4000
#define SPECIAL_MOVEMENT_EN_PASSANT 0x8000
#define SPECIAL_MOVEMENT_PROMOTE    0x10000

/* Combinações de movimentos */
#define MOVEMENT_DIRECTION_ALL \
  (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE | MOVEMENT_DIRECTION_DIAGONAL | MOVEMENT_DIRECTION_L)
  
#define MOVEMENT_DIRECTION_STAR \
  (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE | MOVEMENT_DIRECTION_DIAGONAL )

#define SPECIAL_MOVEMENT_ALL \
  (SPECIAL_MOVEMENT_CASTLE | SPECIAL_MOVEMENT_EN_PASSANT | SPECIAL_MOVEMENT_PROMOTE)

#define MOVEMENT_TYPE_ALL \
  (MOVEMENT_DIRECTION_ALL | SPECIAL_MOVEMENT_ALL)

/* Estrutura de Movimento */
typedef struct STRUCT_MOVEMENT {
  int iStartX;
  int iStartY;
  int iEndX;
  int iEndY;
  uint16_t ui16MovementFlags;
  char szMovement[32];
} STRUCT_MOVEMENT, *PSTRUCT_MOVEMENT;

typedef struct STRUCT_MOVE_LIST {
  STRUCT_MOVEMENT stMovement;
  struct STRUCT_MOVE_LIST *pstNext;
} STRUCT_MOVE_LIST, *PSTRUCT_MOVE_LIST;

extern PSTRUCT_MOVE_LIST gpstMoveList;

int iValidateSquareHighlight(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
void vHighlightRookMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
void vHighlightKingMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
void vHighlightPawnMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
int bValidatePieceMovementAndRange(
    STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
    int iStartRow,
    int iStartCol,
    int iTargetRow,
    int iTargetCol);
int bPathIsClear(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iStartRow, int iStartCol, int iTargetRow, int iTargetCol);
int bValidateCastle(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iStartRow, int iStartCol, int iTargetCol);
int bValidateEnPassant(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iStartRow, int iStartCol, int iTargetRow, int iTargetCol);
void vHighlightQueenMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side);
void vHighlightKnightMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vHighlightBishopMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vGetPieceMovementAttributes(const char *pszType, uint8_t ui8Side, int *piMaxRange, uint16_t *pui16Directions);
// void vCalculateAvailableMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, int iMaxRange, uint16_t ui16Directions, uint8_t ui8Side);
int iValidateMove(PSTRUCT_MOVEMENT pMovement);
void vCalculateDirections(int iX, int iY, int *aiDirections);
void vPrintDirections(const int *aiDirections);

/**
 * @brief Loga todos os moviementos jogados na partida ate o momento atual
 */
void vTraceMoveList(void);

/**
 * @brief Inicializa a lista global de movimentos
 *
 * @return int
 */
int bCreateMoveList(void);

/**
 * @brief Verifica se a lista de movimentos esta vazia
 *
 * @return TRUE Lista vazia
 * @return FALSE Lista nao esta vazia
 */
int bMoveListIsEmpty(void);

/**
 * @brief Adiciona um movimento na lista de movimentos
 *
 * @param pszMovement Movimento a ser adicionado na lista
 *
 * @return TRUE
 */
int bAddMoveToList(STRUCT_MOVEMENT *pstMovement);

void vMovePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iFromRow, int iFromCol, int iToRow, int iToCol);

#endif /* MOVEMENT_H */
