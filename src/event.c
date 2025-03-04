#include <event.h>
#include <trace.h>
#include <board.h>
#include <movement.h>

static int iCurrentTurn = FRIENDLY_SIDE; // Turno inicial: peças brancas

void vHandleMouseClickEvent(SDL_Event *pEvent, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT])
{
  static int iSelectedRow = -1, iSelectedCol = -1;

  if (pEvent->type != SDL_MOUSEBUTTONDOWN)
    return;

  int iMouseX = pEvent->button.x;
  int iMouseY = pEvent->button.y;
  int iRow = (ROW_SQUARE_COUNT - 1) - (iMouseY / SQUARE_SIZE);
  int iCol = iMouseX / SQUARE_SIZE;

  if (iRow < 0 || iRow >= ROW_SQUARE_COUNT || iCol < 0 || iCol >= COLUMN_SQUARE_COUNT)
    return;

  if (iSelectedRow == -1 || iSelectedCol == -1)
  {
    if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) != 0 && pBoard[iRow][iCol].ui8Side == iCurrentTurn)
    {
      iSelectedRow = iRow;
      iSelectedCol = iCol;
      vHighlightPieceMoves(pBoard, iRow, iCol);
    }
  }
  else
  {
    if (pBoard[iRow][iCol].bHighlighted)
    {
      if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) == 0)
      {
        vMovePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol);
      }
      else if (pBoard[iRow][iCol].ui8Side != iCurrentTurn)
      {
        vCapturePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol);
      }

      vClearHighlights(pBoard);
      iSelectedRow = -1;
      iSelectedCol = -1;

      // Alterna o turno
      iCurrentTurn = (iCurrentTurn == FRIENDLY_SIDE) ? ENEMY_SIDE : FRIENDLY_SIDE;
    }
    else
    {
      iSelectedRow = iRow;
      iSelectedCol = iCol;
      vClearHighlights(pBoard);
      vHighlightPieceMoves(pBoard, iRow, iCol);
    }
  }
}

void vCapturePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iSrcRow, int iSrcCol, int iDestRow, int iDestCol)
{
  vTraceVarArgs("Takes %s in position (%d, %d)\n", pBoard[iDestRow][iDestCol].pszType, iDestRow, iDestCol);
  // Liberar a memória da peça capturada
  free(pBoard[iDestRow][iDestCol].pszType);

  // Mover a peça atacante para a nova posição
  pBoard[iDestRow][iDestCol].pszType = strdup(pBoard[iSrcRow][iSrcCol].pszType);
  pBoard[iDestRow][iDestCol].ui8Side = pBoard[iSrcRow][iSrcCol].ui8Side;
  pBoard[iDestRow][iDestCol].ui8Color = pBoard[iSrcRow][iSrcCol].ui8Color;

  // Marcar a posição antiga como vazia
  pBoard[iSrcRow][iSrcCol].pszType = strdup(SQUARE_TYPE_BLANK);
  pBoard[iSrcRow][iSrcCol].ui8Side = BLANK_SIDE;
}

void vHighlightPieceMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol)
{
  const char *pszType = pBoard[iRow][iCol].pszType;
  uint8_t ui8Side = pBoard[iRow][iCol].ui8Side;

  vTraceVarArgs("Testing highlighting possibilities for square piece: %s at (%d, %d), Side: %d", pszType, iRow, iCol, ui8Side);

  if (strcmp(pszType, SQUARE_TYPE_BLANK) == 0)
  {
    vTraceVarArgs("No moves to highlight for an empty square.");
    return;
  }

  // Testar os lados para verificar coerência com o layout
  if (ui8Side == FRIENDLY_SIDE)
  {
    vTraceVarArgs("Piece is FRIENDLY_SIDE.");
  }
  else if (ui8Side == ENEMY_SIDE)
  {
    vTraceVarArgs("Piece is ENEMY_SIDE.");
  }
  else
  {
    vTraceVarArgs("Piece has an invalid or blank side.");
  }

  // Destacar movimentos baseados no tipo da peça
  if (strcmp(pszType, SQUARE_TYPE_PAWN_PIECE) == 0)
  {
    vHighlightPawnMoves(pBoard, iRow, iCol, ui8Side);
  }
  else if (strcmp(pszType, SQUARE_TYPE_ROOK_PIECE) == 0)
  {
    vHighlightRookMoves(pBoard, iRow, iCol, ui8Side);
  }
  else if (strcmp(pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0)
  {
    vHighlightKnightMoves(pBoard, iRow, iCol);
  }
  else if (strcmp(pszType, SQUARE_TYPE_BISHOP_PIECE) == 0)
  {
    vHighlightBishopMoves(pBoard, iRow, iCol);
  }
  else if (strcmp(pszType, SQUARE_TYPE_QUEEN_PIECE) == 0)
  {
    vHighlightQueenMoves(pBoard, iRow, iCol, ui8Side);
  }
  else if (strcmp(pszType, SQUARE_TYPE_KING_PIECE) == 0)
  {
    vHighlightKingMoves(pBoard, iRow, iCol);
  }
  else
  {
    vTraceVarArgs("Unknown piece type: %s", pszType);
  }
}

// Gerencia eventos de clique do mouse
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event)
{
  if (pSDL_Event_Event->type == SDL_MOUSEBUTTONDOWN)
  {
    int iMouseX = pSDL_Event_Event->button.x;
    int iMouseY = pSDL_Event_Event->button.y;

    int iSquareX = iMouseX / SQUARE_SIZE; // Determina a coluna
    int iSquareY = iMouseY / SQUARE_SIZE; // Determina a linha

    vTraceVarArgs("Mouse Click: (%d, %d) -> Square (%d, %d)",
                  iMouseX, iMouseY, iSquareX, iSquareY);
  }
}

// Lida com eventos de saída
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event)
{
  if (pSDL_Event_Event->type == SDL_QUIT)
  {
    vTraceMsg("Quit event. Aborting...");
  }
}
