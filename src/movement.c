#include <board.h>
#include <movement.h>
#include <util.h>
#include <stdlib.h>
#include <stdio.h>

int bValidateSquareHighlight(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side) {
   /* Bloqueia se for peça do mesmo lado */
  if ( pBoard[iRow][iCol].ui8Side == ui8Side )
    return FALSE;
  if ( iRow < 0 && iRow >= ROW_SQUARE_COUNT && iCol < 0 && iCol >= COLUMN_SQUARE_COUNT )
    return FALSE;
  /* Permite captura se encontrar peça inimiga, mas interrompe o movimento */
  return (pBoard[iRow][iCol].ui8Side != NEUTRAL_SIDE) ? FALSE : TRUE;
}

void vHighlightRookMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side) {
  int ii = 0;
  int jj = 0;
  if ( DEBUG_MSGS ) {
    vTraceVarArgs("Calculating rook moves for (%d, %d), side: %d",
      iRow,
      iCol,
      ui8Side
    );
  }

  for ( ii = iRow - 1; ii >= 0; ii--) { // Norte
    if ( !bValidateSquareHighlight(pBoard, ii, iCol, ui8Side) )
      break;
    if ( DEBUG_MSGS ) vTraceVarArgs("Highlighted move to (%d, %d)", ii, iCol);
    pBoard[ii][iCol].bHighlighted = TRUE;
  }
  for (  ii = iRow + 1; ii < ROW_SQUARE_COUNT; ii++ ) { // Sul
    if (!bValidateSquareHighlight(pBoard, ii, iCol, ui8Side))
      break;
    if ( DEBUG_MSGS ) vTraceVarArgs("Highlighted move to (%d, %d)", ii, iCol);
    pBoard[ii][iCol].bHighlighted = TRUE;
  }
  for (  jj = iCol - 1; jj >= 0; jj-- ) {// Oeste
    if (!bValidateSquareHighlight(pBoard, iRow, jj, ui8Side))
      break;
    if ( DEBUG_MSGS ) vTraceVarArgs("Highlighted move to (%d, %d)", iRow, jj);
    pBoard[iRow][jj].bHighlighted = TRUE;
  }
  
  for ( jj = iCol + 1; jj < COLUMN_SQUARE_COUNT; jj++) { // Leste
    if (!bValidateSquareHighlight(pBoard, iRow, jj, ui8Side))
      break;
    if ( DEBUG_MSGS ) vTraceVarArgs("Highlighted move to (%d, %d)", iRow, jj);
    pBoard[iRow][jj].bHighlighted = TRUE;
  }
}

void vHighlightKingMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side) {
  int aiOffsets[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  for ( int ii = 0; ii < 8; ii++ ) {
    int iNewRow = iRow + aiOffsets[ii][0];
    int iNewCol = iCol + aiOffsets[ii][1];
    // Evitar aliados
    if ( bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side) )
      pBoard[iNewRow][iNewCol].bHighlighted = TRUE;
  }
}

void vHighlightPawnMoves(
  STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
  int iRow,
  int iCol,
  uint8_t ui8Side) {
  // Ajustar direção para alinhar com a inversão visual do tabuleiro
  int iDirection = (ui8Side == FRIENDLY_SIDE) ? 1 : -1; // Brancas para cima (+1), pretas para baixo (-1)
  int iStartRow = (ui8Side == FRIENDLY_SIDE) ? 1 : 6;   // Linha inicial de movimento duplo
  int iOffset = 0;
  
  vTraceVarArgs("Calculating pawn moves for (%d, %d), side: %d, direction: %d", iRow, iCol, ui8Side, iDirection);

  // Movimento normal de 1 casa para frente
  if ( iRow + iDirection >= 0 && iRow + iDirection < ROW_SQUARE_COUNT ) {
    vTraceVarArgs("Checking single move to (%d, %d)", iRow + iDirection, iCol);

    if ( pBoard[iRow + iDirection][iCol].ui8Side == NEUTRAL_SIDE) {
      pBoard[iRow + iDirection][iCol].bHighlighted = TRUE;
      vTraceVarArgs("Highlighted single move to (%d, %d)", iRow + iDirection, iCol);

      // Movimento inicial de 2 casas para frente
      if (iRow == iStartRow && pBoard[iRow + 2 * iDirection][iCol].ui8Side == NEUTRAL_SIDE) {
        pBoard[iRow + 2 * iDirection][iCol].bHighlighted = TRUE;
        vTraceVarArgs("Highlighted double move to (%d, %d)", iRow + 2 * iDirection, iCol);
      }
    } else {
      vTraceVarArgs("Single move blocked at (%d, %d)", iRow + iDirection, iCol);
    }
  }

  // Captura na diagonal
  for ( iOffset = -1; iOffset <= 1; iOffset += 2) { // -1 = oeste, +1 = leste
    int iNewCol = iCol + iOffset;
    if (iNewCol >= 0 && iNewCol < COLUMN_SQUARE_COUNT &&
      iRow + iDirection >= 0 && iRow + iDirection < ROW_SQUARE_COUNT) {
      vTraceVarArgs("Checking diagonal capture at (%d, %d)", iRow + iDirection, iNewCol);

      if ( pBoard[iRow + iDirection][iNewCol].ui8Side != NEUTRAL_SIDE &&
            pBoard[iRow + iDirection][iNewCol].ui8Side != ui8Side) {
        pBoard[iRow + iDirection][iNewCol].bHighlighted = TRUE;
        vTraceVarArgs("Highlighted diagonal capture at (%d, %d)", iRow + iDirection, iNewCol);
      }
      else {
        vTraceVarArgs("No capture possible at (%d, %d)", iRow + iDirection, iNewCol);
      }
    }
  }
}

/* Valida movimento e alcance de uma peça */
int bValidatePieceMovementAndRange(
    STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
    int iStartRow,
    int iStartCol,
    int iTargetRow,
    int iTargetCol) {
  /* Verifica se há uma peça na posição inicial */
  const char *pszPieceType = pBoard[iStartRow][iStartCol].pszType;
  uint8_t ui8Side = pBoard[iStartRow][iStartCol].ui8Side;
  uint16_t ui16MovementType;
  int iMovementRange;
  int iRowDiff = 0;
  int iColDiff = 0;

  if ( strcmp(pszPieceType, SQUARE_TYPE_BLANK) == 0 )
    return FALSE; /* Não há peça para mover */

  /* Verifica se o destino é uma peça do mesmo lado */
  if ( pBoard[iTargetRow][iTargetCol].ui8Side == ui8Side )
    return FALSE;

  // Determina o tipo de movimento e alcance
  vGetPieceMovementAttributes(
    pszPieceType,
    pBoard[iStartRow][iStartCol].ui8Side,
    &iMovementRange,
    &ui16MovementType
  );

  // Calcula as diferenças de linha e coluna
  iRowDiff = iTargetRow - iStartRow;
  iColDiff = iTargetCol - iStartCol;

  // Movimentos especiais
  if ( ui16MovementType & SPECIAL_MOVEMENT_ALL ) {
    if ( ui16MovementType & SPECIAL_MOVEMENT_CASTLE )
      if (bValidateCastle(pBoard, iStartRow, iStartCol, iTargetCol))
        return TRUE; // Roque válido
    if ( ui16MovementType & SPECIAL_MOVEMENT_EN_PASSANT )
      if ( bValidateEnPassant(pBoard, iStartRow, iStartCol, iTargetRow, iTargetCol) )
        return TRUE; // En passant válido
  }

  // Movimentos diagonais
  if ( ui16MovementType & MOVEMENT_DIRECTION_DIAGONAL )
    if ( abs(iRowDiff) == abs(iColDiff) && abs(iRowDiff) <= iMovementRange )
      return bPathIsClear(pBoard, iStartRow, iStartCol, iTargetRow, iTargetCol);

  /* Movimentos verticais */
  if (ui16MovementType & MOVEMENT_DIRECTION_COLUMN)
    if (iColDiff == 0 && abs(iRowDiff) <= iMovementRange)
      return bPathIsClear(pBoard, iStartRow, iStartCol, iTargetRow, iTargetCol);

  /* Movimentos horizontais */
  if (ui16MovementType & MOVEMENT_DIRECTION_LINE)
    if (iRowDiff == 0 && abs(iColDiff) <= iMovementRange)
      return bPathIsClear(pBoard, iStartRow, iStartCol, iTargetRow, iTargetCol);

  /* Movimentos em L (cavalo) */
  if (ui16MovementType & MOVEMENT_DIRECTION_L)
    if ((abs(iRowDiff) == 2 && abs(iColDiff) == 1) || (abs(iRowDiff) == 1 && abs(iColDiff) == 2))
      return TRUE;

  /* Se nenhum movimento foi válido, retorna falso */
  return FALSE;
}

// Função auxiliar para validar se o caminho está livre
int bPathIsClear(
  STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
  int iStartRow,
  int iStartCol,
  int iTargetRow,
  int iTargetCol) {
  int iRowStep = (iTargetRow > iStartRow) ? 1 : (iTargetRow < iStartRow) ? -1 : 0;
  int iColStep = (iTargetCol > iStartCol) ? 1 : (iTargetCol < iStartCol) ? -1 : 0;
  int iRow = iStartRow + iRowStep;
  int iCol = iStartCol + iColStep;
  while ( iRow != iTargetRow || iCol != iTargetCol ) {
    if ( pBoard[iRow][iCol].ui8Side != NEUTRAL_SIDE )
      return FALSE; // Caminho obstruído
    iRow += iRowStep;
    iCol += iColStep;
  }
  return TRUE;
}

// Valida o roque
int bValidateCastle(
    STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
    int iStartRow,
    int iStartCol,
    int iTargetCol) {
  // Determina se é um roque curto ou longo
  int iColStep = (iTargetCol > iStartCol) ? 1 : -1;

  // Verifica se a peça é o rei
  if (strcmp(pBoard[iStartRow][iStartCol].pszType, SQUARE_TYPE_KING_PIECE) != 0)
    return FALSE;

  // Verifica se todas as casas entre o rei e a torre estão livres
  for ( int iCol = iStartCol + iColStep; iCol != iTargetCol; iCol += iColStep ) {
    if (pBoard[iStartRow][iCol].ui8Side != NEUTRAL_SIDE)
      return FALSE; // Caminho bloqueado
  }

  // Adicione lógica para verificar se o rei passa ou termina em xeque
  return TRUE; // Roque válido
}

// Valida en passant
int bValidateEnPassant(
  STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
  int iStartRow,
  int iStartCol,
  int iTargetRow,
  int iTargetCol) {
  // Verifica se a peça é um peão
  if (strcmp(pBoard[iStartRow][iStartCol].pszType, SQUARE_TYPE_PAWN_PIECE) != 0)
    return FALSE;
  // Verifica se o movimento é diagonal e a casa alvo está vazia
  if (abs(iTargetCol - iStartCol) == 1 && pBoard[iTargetRow][iTargetCol].ui8Side == NEUTRAL_SIDE)
    // Lógica para verificar se "en passant" é válido (último movimento do oponente)
    // Você pode armazenar informações do último movimento em uma estrutura global
    return TRUE;
  return FALSE;
}

void vHighlightQueenMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side) {
  vHighlightRookMoves(pBoard, iRow, iCol, ui8Side); // Movimentos como torre
  vHighlightBishopMoves(pBoard, iRow, iCol);        // Movimentos como bispo
}

void vHighlightKnightMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  int aiOffsets[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
  char szLogMsg[128];
  int ii = 0;
  
  snprintf(szLogMsg, sizeof(szLogMsg), "Calculating knight moves for (%d, %d)", iRow, iCol);
  vTraceMsg(szLogMsg);

  for ( ii = 0; ii < 8; ii++) {
    int iNewRow = iRow + aiOffsets[ii][0];
    int iNewCol = iCol + aiOffsets[ii][1];

    if (iNewRow >= 0 && iNewRow < ROW_SQUARE_COUNT && iNewCol >= 0 && iNewCol < COLUMN_SQUARE_COUNT)
    {
      if (pBoard[iNewRow][iNewCol].ui8Side != pBoard[iRow][iCol].ui8Side) // Evitar aliados
      {
          pBoard[iNewRow][iNewCol].bHighlighted = TRUE;
          snprintf(szLogMsg, sizeof(szLogMsg), "Highlighted move to (%d, %d)", iNewRow, iNewCol);
          vTraceMsg(szLogMsg);
      }
    }
  }
}


void vHighlightBishopMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  int aiDirections[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}; // NW, NE, SW, SE
  int ii = 0;
  
  for ( ii = 0; ii < 4; ii++ ) {
    int iNewRow = iRow, iNewCol = iCol;

    while ( TRUE ) {
      iNewRow += aiDirections[ii][0];
      iNewCol += aiDirections[ii][1];

      if (iNewRow < 0 || iNewRow >= ROW_SQUARE_COUNT || iNewCol < 0 || iNewCol >= COLUMN_SQUARE_COUNT)
        break;

      if (pBoard[iNewRow][iNewCol].ui8Side == pBoard[iRow][iCol].ui8Side) // Peça do mesmo lado
        break;

      pBoard[iNewRow][iNewCol].bHighlighted = TRUE;

      if (pBoard[iNewRow][iNewCol].ui8Side != NEUTRAL_SIDE) // Obstáculo encontrado
        break;
    }
  }
}

void vGetPieceMovementAttributes(
  const char *pszType,
  uint8_t ui8Side,
  int *piMaxRange,
  uint16_t *pui16Directions) {
  if ( strcmp(pszType, SQUARE_TYPE_ROOK_PIECE) == 0 ) {
    *piMaxRange = LINE_OF_SIGHT;
    *pui16Directions = MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE;
  }
  else if ( strcmp(pszType, SQUARE_TYPE_BISHOP_PIECE) == 0 ) {
    *piMaxRange = LINE_OF_SIGHT;
    *pui16Directions = MOVEMENT_DIRECTION_DIAGONAL;
  }
  else if ( strcmp(pszType, SQUARE_TYPE_QUEEN_PIECE) == 0 ) {
    *piMaxRange = LINE_OF_SIGHT;
    *pui16Directions = MOVEMENT_DIRECTION_STAR;
  } 
  else if ( strcmp(pszType, SQUARE_TYPE_KING_PIECE) == 0) {
    *piMaxRange = SQUARE_RANGE;
    *pui16Directions = MOVEMENT_DIRECTION_STAR;
  }
  else if ( strcmp(pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0 ) {
    *piMaxRange = L_RANGE;
    *pui16Directions = MOVEMENT_DIRECTION_L;
  }
  else if ( strcmp(pszType, SQUARE_TYPE_PAWN_PIECE) == 0 ) {
    *piMaxRange = SQUARE_RANGE;
    *pui16Directions = (ui8Side == FRIENDLY_SIDE) ? NORTH : SOUTH;
  }
  else {
    *piMaxRange = 0;
    *pui16Directions = MOVEMENT_TYPE_NONE;
  }
}

// Função para calcular movimentos possíveis para uma peça
void vCalculateAvailableMoves(
  STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT],
  int iRow,
  int iCol,
  int iMaxRange,
  uint16_t ui16Directions,
  uint8_t ui8Side) {
  int iNewRow, iNewCol;

  // Processar cada direção ativada
  if ( ui16Directions & NORTH ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow - iRange;
      iNewCol = iCol;
      if ( iNewRow < 0 || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side) )
        break;
    }
  }

  if ( ui16Directions & SOUTH ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow + iRange;
      iNewCol = iCol;
      if (iNewRow >= ROW_SQUARE_COUNT || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side))
        break;
    }
  }

  if ( ui16Directions & WEST ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow;
      iNewCol = iCol - iRange;

      if ( iNewCol < 0 || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side) )
        break;
    }
  }

  if ( ui16Directions & EAST ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow;
      iNewCol = iCol + iRange;
      if (iNewCol >= COLUMN_SQUARE_COUNT || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side))
        break;
    }
  }

  if ( ui16Directions & NORTH_WEST ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow - iRange;
      iNewCol = iCol - iRange;
      if (iNewRow < 0 || iNewCol < 0 || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side))
        break;
    }
  }

  if (ui16Directions & NORTH_EAST) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow - iRange;
      iNewCol = iCol + iRange;
      if (iNewRow < 0 || iNewCol >= COLUMN_SQUARE_COUNT || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side))
        break;
    }
  }

  if ( ui16Directions & SOUTH_WEST ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow + iRange;
      iNewCol = iCol - iRange;
      if (iNewRow >= ROW_SQUARE_COUNT || iNewCol < 0 || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side))
        break;
    }
  }

  if ( ui16Directions & SOUTH_EAST ) {
    for ( int iRange = 1; iRange <= iMaxRange; iRange++ ) {
      iNewRow = iRow + iRange;
      iNewCol = iCol + iRange;
      if (iNewRow >= ROW_SQUARE_COUNT || iNewCol >= COLUMN_SQUARE_COUNT || !bValidateSquareHighlight(pBoard, iNewRow, iNewCol, ui8Side))
        break;
    }
  }
}

// Função para validar um movimento
int iValidateMove(PSTRUCT_MOVEMENT pMovement) {
  if ( !pMovement ) {
    vTraceError( "Erro: Movimento inválido (ponteiro nulo).\n");
    return 0;
  }

  // Exemplo de validação simples: o movimento deve permanecer dentro do tabuleiro
  if ( pMovement->iStartX < 0 || pMovement->iStartX > 7 ||
        pMovement->iStartY < 0 || pMovement->iStartY > 7 ||
        pMovement->iEndX < 0 || pMovement->iEndX > 7 ||
        pMovement->iEndY < 0 || pMovement->iEndY > 7 ) {
    vTraceError( "Erro: Movimento fora dos limites do tabuleiro.\n");
    return 0;
  }

  // Valida se o movimento respeita os flags fornecidos
  if ( pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_COLUMN ) {
    if ( pMovement->iStartX != pMovement->iEndX ) {
      vTraceError( "Erro: Movimento de coluna inválido.\n");
      return 0;
    }
  }

  if ( pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_LINE ) {
    if ( pMovement->iStartY != pMovement->iEndY ) {
      vTraceError( "Erro: Movimento de linha inválido.\n");
      return 0;
    }
  }

  if ( pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_DIAGONAL ) {
    if ( abs(pMovement->iStartX - pMovement->iEndX) != abs(pMovement->iStartY - pMovement->iEndY) ) {
      vTraceError( "Erro: Movimento diagonal inválido.\n");
      return 0;
    }
  }

  if ( pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_L ) {
    int dx = abs(pMovement->iStartX - pMovement->iEndX);
    int dy = abs(pMovement->iStartY - pMovement->iEndY);
    if ( !((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) ) {
      vTraceError( "Erro: Movimento em L inválido.\n");
      return 0;
    }
  }

  return 1; // Movimento válido
}

// Função para calcular direções disponíveis a partir de uma posição
void vCalculateDirections(int iX, int iY, int *aiDirections) {
  int ii = 0;
  if (!aiDirections)
    return;
  // Inicializa todas as direções como inválidas
  for ( ii = 0; ii < 4; ii++)
    aiDirections[ii] = 0;
  // Norte
  if (iY > 0)
    aiDirections[NORTH] = 1;
  // Sul
  if (iY < 7)
    aiDirections[SOUTH] = 1;
  // Oeste
  if (iX > 0)
    aiDirections[WEST] = 1;
  // Leste
  if (iX < 7)
    aiDirections[EAST] = 1;
}

// Função para imprimir direções (opcional, útil para debugging)
void vPrintDirections(const int *aiDirections) {
  if (!aiDirections)
    return;
  printf("Direções disponíveis:\n");
  if (aiDirections[NORTH])
    printf("Norte\n");
  if (aiDirections[SOUTH])
    printf("Sul\n");
  if (aiDirections[WEST])
    printf("Oeste\n");
  if (aiDirections[EAST])
    printf("Leste\n");
}
void vMovePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iFromRow, int iFromCol, int iToRow, int iToCol) {
  pBoard[iToRow][iToCol] = pBoard[iFromRow][iFromCol];
  pBoard[iFromRow][iFromCol].pszType = SQUARE_TYPE_BLANK;
  pBoard[iFromRow][iFromCol].ui8Side = NEUTRAL_SIDE;
}
