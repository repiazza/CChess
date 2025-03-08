#ifndef EVENT_H
#define EVENT_H

#include <SDL2/SDL.h>
#include <board.h>

extern const char *gkpaszTurns[];

void vSetSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vUnsetSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vToggleSquareSelection(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
bool bSquareSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
bool bHasAnySelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
PSTRUCT_SQUARE pstGetBoardIfSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
int iGetRowFromBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], PSTRUCT_SQUARE pstBoard);
int iGetColFromBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],  PSTRUCT_SQUARE pstBoard);
void vToggleTurn(int *piTurn);

/* Prototipação das funções de eventos */
void vHandleMouseClickEvent(SDL_Event *pEvent, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
void vCapturePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iSrcRow, int iSrcCol, int iDestRow, int iDestCol);
void vHighlightPieceMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event);
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event);

#endif /* EVENT_H */
