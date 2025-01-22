#ifndef EVENT_H
#define EVENT_H

#include <SDL2/SDL.h>
#include <board.h>

// Prototipação das funções de eventos

void vHighlightPieceMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);
void vHandleMouseClickEvent(SDL_Event *pEvent, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event);
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event);

#endif // EVENT_H