/**
 * event.c
 * 
 * Written by Renato Fermi <repiazza@gmail.com> in January 2025
 * 
 * Description: Chess written in C
 */

#include <event.h>
#include <trace.h>
#include <board.h>
#include <movement.h>

extern int gbRenderer;

const char *gkpaszTurns[] = {
  "FRIENDLY",
  "ENEMY",
  NULL
};

int giCurrentTurn = FRIENDLY_SIDE; // Turno inicial: peças brancas

void vSetSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  pBoard[iRow][iCol].bSelected = TRUE;
}

void vUnsetSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  pBoard[iRow][iCol].bSelected = FALSE;
}

void vToggleSquareSelection(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  if ( !pBoard[iRow][iCol].bSelected ) {
    vSetSelected(pBoard, iRow, iCol);
    return;
  }
  vUnsetSelected(pBoard, iRow, iCol);
}

bool bSquareSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  return pBoard[iRow][iCol].bSelected;
}

bool bHasAnySelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int iRow = 0;
  int iCol = 0;
  for ( iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++ ) {
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      if ( bSquareSelected(pBoard, iRow, iCol) ) return TRUE;
    }
  }
  vTraceVarArgs("RETORNEMO FARSE");
  return FALSE;
}

PSTRUCT_SQUARE pstGetBoardIfSelected(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int iRow = 0;
  int iCol = 0;
  for ( iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++ ) {
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      if ( bSquareSelected(pBoard, iRow, iCol) ) {
        vTraceVarArgs("iRow [%d] iCol [%d]", iRow, iCol);
        return &pBoard[iRow][iCol];
      }
    }
  }
  return NULL;
}

int iGetRowFromBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], PSTRUCT_SQUARE pstBoard) {
  int iRow = 0;
  int iCol = 0;
  for ( iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++ ) {
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      if ( pstBoard == &pBoard[iRow][iCol] ) {
        return iRow;
      }
    }
  }
  return -1;
}

int iGetColFromBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],  PSTRUCT_SQUARE pstBoard) {
  int iRow = 0;
  int iCol = 0;
  for ( iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++ ) {
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      if ( pstBoard == &pBoard[iRow][iCol] ) {
        return iCol;
      }
    }
  }
  return -1;
}

void vToggleTurn(int *piTurn) {
  if ( piTurn == NULL ) return;
  vTraceVarArgs("Turno encerrado [%s]", gkpaszTurns[*piTurn-1]);
  switch ( *piTurn ) {
    case FRIENDLY_SIDE: *piTurn = ENEMY_SIDE   ; break;
    case ENEMY_SIDE   : *piTurn = FRIENDLY_SIDE; break;
    default:                                     break;
  }
  vTraceVarArgs("Novo turno iniciado [%s]", gkpaszTurns[*piTurn-1]);
}

int bKingIsChecked = FALSE;

void vKingIsChecked(void) {
  if ( !bKingIsChecked ) {
    if ( giCurrentTurn == FRIENDLY_SIDE ) {
      
    }
    else {
    
    }
  }
}

void vHandleMouseClickEvent(SDL_Event *pEvent, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  static int iSelectedRow = -1;
  static int iSelectedCol = -1;
  int iMouseX = 0;
  int iMouseY = 0;
  int iRow = 0;
  int iCol = 0;
  PSTRUCT_SQUARE pstBoard = NULL;
  
  if ( pEvent->type != SDL_MOUSEBUTTONDOWN )
    return;
  
  gbRenderer = TRUE;
  
  iMouseX = pEvent->button.x;
  iMouseY = pEvent->button.y-INFO_RECT_HEIGHT;
  iRow = (ROW_SQUARE_COUNT - 1) - (iMouseY / SQUARE_SIZE);
  iCol = iMouseX / SQUARE_SIZE;

  if (iRow < 0 || iRow >= ROW_SQUARE_COUNT || iCol < 0 || iCol >= COLUMN_SQUARE_COUNT)
    return;
  vTraceMsg("pBoard[iRow][iCol].ui8Side [%d]", pBoard[iRow][iCol].ui8Side);
  vTraceMsg("giCurrentTurn [%d]", giCurrentTurn);
  
  if ( !bHasAnySelected(pBoard) ) {
    if ( pBoard[iRow][iCol].ui8Side == giCurrentTurn ) {
      vClearHighlights(pBoard);
      vToggleSquareSelection(pBoard, iRow, iCol);
      vHighlightPieceMoves(pBoard, iRow, iCol);
    }
  }
  else {
    pstBoard = pstGetBoardIfSelected(pBoard);
    
    if ( pstBoard ) {
      iSelectedRow = iGetRowFromBoard(pBoard, pstBoard);
      iSelectedCol = iGetColFromBoard(pBoard, pstBoard);
    }
    
    //  Clicamos duas vezes na mesma?
    if ( iSelectedRow == iRow && iSelectedCol == iCol ) {
      if ( pstBoard->ui8Side == giCurrentTurn ) {
        vClearHighlights(pBoard);
        vToggleSquareSelection(pBoard, iRow, iCol);
        return;
      }
    }
    // Clicamos em uma outra peca aliada?
    else if ( pBoard[iRow][iCol].ui8Side == giCurrentTurn ) {
      vToggleSquareSelection(pBoard, iSelectedRow, iSelectedCol);
      vClearHighlights(pBoard);
      vToggleSquareSelection(pBoard, iRow, iCol);
      vHighlightPieceMoves(pBoard, iRow, iCol);
      return;
    }
    
    // nao, vamos andar ou comer?
    if ( pBoard[iRow][iCol].bHighlighted ) {
      if ( strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) == 0 ) {
        vToggleSquareSelection(pBoard, iSelectedRow, iSelectedCol);
        vMovePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol);
      }
      else if ( pBoard[iRow][iCol].ui8Side != giCurrentTurn ) {
        vToggleSquareSelection(pBoard, iSelectedRow, iSelectedCol);
        vCapturePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol);
      }
      
      vToggleTurn(&giCurrentTurn);
      vClearHighlights(pBoard);

      vKingIsChecked();

      iSelectedRow = -1;
      iSelectedCol = -1;
    }
    else { // Desfazer selecao
      vClearHighlights(pBoard);
      vToggleSquareSelection(pBoard, iSelectedRow, iSelectedCol);
    }
  }
}

void vCapturePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iSrcRow, int iSrcCol, int iDestRow, int iDestCol) {
  vTraceBegin();
  if ( DEBUG_MSGS ) vTraceVarArgs("Takes %s in position (%d, %d)\n", pBoard[iDestRow][iDestCol].pszType, iDestRow, iDestCol);
  vTraceBoardRowCol("DEST BOARD", pBoard, iDestRow, iDestCol);
  
  // Liberar a memória da peça capturada
  vTraceMsg("Liberar a memória da peça capturada");
  free(pBoard[iDestRow][iDestCol].pszType);
  pBoard[iDestRow][iDestCol].pszType = NULL;

  // Mover a peça atacante para a nova posição
  vTraceMsg("Mover a peça atacante para a nova posição");
  pBoard[iDestRow][iDestCol].pszType = strdup(pBoard[iSrcRow][iSrcCol].pszType);
  pBoard[iDestRow][iDestCol].ui8Side = pBoard[iSrcRow][iSrcCol].ui8Side;
  pBoard[iDestRow][iDestCol].ui8Color = pBoard[iSrcRow][iSrcCol].ui8Color;
  pBoard[iDestRow][iDestCol].bHasMoved = TRUE;

  vTraceMsg("Marcar a posição antiga como vazia");
  // Marcar a posição antiga como vazia
  pBoard[iSrcRow][iSrcCol].pszType = strdup(SQUARE_TYPE_BLANK);
  pBoard[iSrcRow][iSrcCol].ui8Side = NEUTRAL_SIDE;
  vTraceBoardRowCol("SRC BOARD DEPOIS", pBoard, iSrcRow, iSrcCol);
  vTraceEnd();
}

void vHighlightPieceMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  const char *pszType = pBoard[iRow][iCol].pszType;
  uint8_t ui8Side = pBoard[iRow][iCol].ui8Side;

  if ( DEBUG_MSGS ) vTraceVarArgs("Testing highlighting possibilities for square piece: %s at (%d, %d), Side: %d", pszType, iRow, iCol, ui8Side);

  if ( strcmp(pszType, SQUARE_TYPE_BLANK) == 0 ) {
    if ( DEBUG_MSGS ) vTraceVarArgs("No moves to highlight for an empty square.");
    return;
  }

  // Testar os lados para verificar coerência com o layout
  if ( ui8Side == FRIENDLY_SIDE ) {
    if ( DEBUG_MSGS ) vTraceVarArgs("Piece is FRIENDLY_SIDE.");
  }
  else if ( ui8Side == ENEMY_SIDE ) {
    if ( DEBUG_MSGS ) vTraceVarArgs("Piece is ENEMY_SIDE.");
  }
  else {
    if ( DEBUG_MSGS ) vTraceVarArgs("Piece has an invalid or blank side.");
    return;
  }

  // Destacar movimentos baseados no tipo da peça
  if ( strcmp(pszType, SQUARE_TYPE_PAWN_PIECE) == 0 )
    vHighlightPawnMoves(pBoard, iRow, iCol, ui8Side);
  else if ( strcmp(pszType, SQUARE_TYPE_ROOK_PIECE) == 0 )
    vHighlightRookMoves(pBoard, iRow, iCol, ui8Side);
  else if ( strcmp(pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0 )
    vHighlightKnightMoves(pBoard, iRow, iCol);
  else if ( strcmp(pszType, SQUARE_TYPE_BISHOP_PIECE) == 0 )
    vHighlightBishopMoves(pBoard, iRow, iCol);
  else if ( strcmp(pszType, SQUARE_TYPE_QUEEN_PIECE) == 0 )
    vHighlightQueenMoves(pBoard, iRow, iCol, ui8Side);
  else if ( strcmp(pszType, SQUARE_TYPE_KING_PIECE) == 0 )
    vHighlightKingMoves(pBoard, iRow, iCol, ui8Side);
  else
    if ( DEBUG_MSGS ) vTraceVarArgs("Unknown piece type: %s", pszType);
}

// Gerencia eventos de clique do mouse
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event) {
  if ( pSDL_Event_Event->type == SDL_MOUSEBUTTONDOWN ) {
    int iMouseX = pSDL_Event_Event->button.x;
    int iMouseY = pSDL_Event_Event->button.y;
    int iSquareX = iMouseX / SQUARE_SIZE; // Determina a coluna
    int iSquareY = iMouseY / SQUARE_SIZE; // Determina a linha
    if ( DEBUG_MSGS ) {
      vTraceVarArgs(
        "Mouse Click: (%d, %d) -> Square (%d, %d)",
        iMouseX,
        iMouseY,
        iSquareX,
        iSquareY
      );
    }
  }
}

// Lida com eventos de saída
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event) {
  if ( pSDL_Event_Event->type == SDL_QUIT ) {
    if ( DEBUG_MSGS ) vTraceMsg("Quit event. Aborting...");
  }
}
