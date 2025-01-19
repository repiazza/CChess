#include <board.h>
#include <movement.h>
#include <util.h>
#include <stdlib.h>
#include <stdio.h>

int bProcessSquareHighlight(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    if (pBoard[iRow][iCol].ui8Side == FRIENDLY_SIDE) {
        return 0; // Bloqueia se for peça amiga
    }

    pBoard[iRow][iCol].bHighlighted = TRUE;

    // Se encontrar uma peça inimiga, permite captura, mas interrompe o movimento
    return (pBoard[iRow][iCol].ui8Side == ENEMY_SIDE) ? 0 : 1;
}



void vHighlightRookMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    for (int i = iRow - 1; i >= 0; i--) { // Norte
        if (!bProcessSquareHighlight(pBoard, i, iCol)) break;
    }
    for (int i = iRow + 1; i < ROW_SQUARE_COUNT; i++) { // Sul
        if (!bProcessSquareHighlight(pBoard, i, iCol)) break;
    }
    for (int j = iCol - 1; j >= 0; j--) { // Oeste
        if (!bProcessSquareHighlight(pBoard, iRow, j)) break;
    }
    for (int j = iCol + 1; j < COLUMN_SQUARE_COUNT; j++) { // Leste
        if (!bProcessSquareHighlight(pBoard, iRow, j)) break;
    }
}

void vHighlightKingMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    int aiOffsets[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (int i = 0; i < 8; i++) {
        int iNewRow = iRow + aiOffsets[i][0];
        int iNewCol = iCol + aiOffsets[i][1];

        if (iNewRow >= 0 && iNewRow < ROW_SQUARE_COUNT && iNewCol >= 0 && iNewCol < COLUMN_SQUARE_COUNT) {
            if (pBoard[iNewRow][iNewCol].ui8Side != pBoard[iRow][iCol].ui8Side) // Evitar aliados
                pBoard[iNewRow][iNewCol].bHighlighted = TRUE;
        }
    }
}

void vHighlightPawnMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol, uint8_t ui8Side) {
    int iDirection = (ui8Side == FRIENDLY_SIDE) ? -1 : 1; // Direção baseada no lado
    int iStartRow = (ui8Side == FRIENDLY_SIDE) ? ROW_SQUARE_COUNT - 2 : 1;

    // Movimento normal
    if (iRow + iDirection >= 0 && iRow + iDirection < ROW_SQUARE_COUNT && 
        pBoard[iRow + iDirection][iCol].ui8Side == BLANK_SIDE) {
        pBoard[iRow + iDirection][iCol].bHighlighted = TRUE;

        // Movimento inicial de duas casas
        if (iRow == iStartRow && 
            pBoard[iRow + 2 * iDirection][iCol].ui8Side == BLANK_SIDE) {
            pBoard[iRow + 2 * iDirection][iCol].bHighlighted = TRUE;
        }
    }

    // Captura diagonal
    for (int iOffset = -1; iOffset <= 1; iOffset += 2) {
        int iNewCol = iCol + iOffset;
        if (iNewCol >= 0 && iNewCol < COLUMN_SQUARE_COUNT &&
            pBoard[iRow + iDirection][iNewCol].ui8Side != BLANK_SIDE &&
            pBoard[iRow + iDirection][iNewCol].ui8Side != ui8Side) {
            pBoard[iRow + iDirection][iNewCol].bHighlighted = TRUE;
        }
    }
}


void vHighlightQueenMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    vHighlightRookMoves(pBoard, iRow, iCol);  // Movimentos como torre
    vHighlightBishopMoves(pBoard, iRow, iCol); // Movimentos como bispo
}


void vHighlightKnightMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    int aiOffsets[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

    for (int i = 0; i < 8; i++) {
        int iNewRow = iRow + aiOffsets[i][0];
        int iNewCol = iCol + aiOffsets[i][1];

        if (iNewRow >= 0 && iNewRow < ROW_SQUARE_COUNT && iNewCol >= 0 && iNewCol < COLUMN_SQUARE_COUNT) {
            if (pBoard[iNewRow][iNewCol].ui8Side != pBoard[iRow][iCol].ui8Side) // Evitar aliados
                pBoard[iNewRow][iNewCol].bHighlighted = TRUE;
        }
    }
}


void vHighlightBishopMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    int aiDirections[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}; // NW, NE, SW, SE

    for (int i = 0; i < 4; i++) {
        int iNewRow = iRow, iNewCol = iCol;

        while (1) {
            iNewRow += aiDirections[i][0];
            iNewCol += aiDirections[i][1];

            if (iNewRow < 0 || iNewRow >= ROW_SQUARE_COUNT || iNewCol < 0 || iNewCol >= COLUMN_SQUARE_COUNT)
                break;

            if (pBoard[iNewRow][iNewCol].ui8Side == pBoard[iRow][iCol].ui8Side) // Peça do mesmo lado
                break;

            pBoard[iNewRow][iNewCol].bHighlighted = TRUE;

            if (pBoard[iNewRow][iNewCol].ui8Side != BLANK_SIDE) // Obstáculo encontrado
                break;
        }
    }
}

void vGetPieceMovementAttributes(
    const char *pszType,
    uint8_t ui8Side,
    int *piMaxRange,
    uint16_t *pui16Directions
) {
    if (strcmp(pszType, SQUARE_TYPE_ROOK_PIECE) == 0) {
        *piMaxRange = LINE_OF_SIGHT;
        *pui16Directions = MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE;
    } else if (strcmp(pszType, SQUARE_TYPE_BISHOP_PIECE) == 0) {
        *piMaxRange = LINE_OF_SIGHT;
        *pui16Directions = MOVEMENT_DIRECTION_DIAGONAL;
    } else if (strcmp(pszType, SQUARE_TYPE_QUEEN_PIECE) == 0) {
        *piMaxRange = LINE_OF_SIGHT;
        *pui16Directions = MOVEMENT_DIRECTION_ALL;
    } else if (strcmp(pszType, SQUARE_TYPE_KING_PIECE) == 0) {
        *piMaxRange = SQUARE_RANGE;
        *pui16Directions = MOVEMENT_DIRECTION_ALL;
    } else if (strcmp(pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0) {
        *piMaxRange = L_RANGE;
        *pui16Directions = MOVEMENT_DIRECTION_L;
    } else if (strcmp(pszType, SQUARE_TYPE_PAWN_PIECE) == 0) {
        *piMaxRange = SQUARE_RANGE;
        *pui16Directions = (ui8Side == FRIENDLY_SIDE) ? NORTH : SOUTH;
    } else {
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
    uint16_t ui16Directions
) {
    int iNewRow, iNewCol;

    // Processar cada direção ativada
    if (ui16Directions & NORTH) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow - iRange;
            iNewCol = iCol;

            if (iNewRow < 0 || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & SOUTH) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow + iRange;
            iNewCol = iCol;

            if (iNewRow >= ROW_SQUARE_COUNT || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & WEST) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow;
            iNewCol = iCol - iRange;

            if (iNewCol < 0 || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & EAST) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow;
            iNewCol = iCol + iRange;

            if (iNewCol >= COLUMN_SQUARE_COUNT || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & NORTH_WEST) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow - iRange;
            iNewCol = iCol - iRange;

            if (iNewRow < 0 || iNewCol < 0 || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & NORTH_EAST) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow - iRange;
            iNewCol = iCol + iRange;

            if (iNewRow < 0 || iNewCol >= COLUMN_SQUARE_COUNT || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & SOUTH_WEST) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow + iRange;
            iNewCol = iCol - iRange;

            if (iNewRow >= ROW_SQUARE_COUNT || iNewCol < 0 || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }

    if (ui16Directions & SOUTH_EAST) {
        for (int iRange = 1; iRange <= iMaxRange; iRange++) {
            iNewRow = iRow + iRange;
            iNewCol = iCol + iRange;

            if (iNewRow >= ROW_SQUARE_COUNT || iNewCol >= COLUMN_SQUARE_COUNT || !bProcessSquareHighlight(pBoard, iNewRow, iNewCol)) break;
        }
    }
}




// Função para validar um movimento
int iValidateMove(PSTRUCT_MOVEMENT pMovement) {
  if (!pMovement) {
  fprintf(stderr, "Erro: Movimento inválido (ponteiro nulo).\n");
  return 0;
  }

  // Exemplo de validação simples: o movimento deve permanecer dentro do tabuleiro
  if (pMovement->iStartX < 0 || pMovement->iStartX > 7 ||
    pMovement->iStartY < 0 || pMovement->iStartY > 7 ||
    pMovement->iEndX < 0 || pMovement->iEndX > 7 ||
    pMovement->iEndY < 0 || pMovement->iEndY > 7) {
    fprintf(stderr, "Erro: Movimento fora dos limites do tabuleiro.\n");
    return 0;
  }

  // Valida se o movimento respeita os flags fornecidos
  if (pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_COLUMN) {
    if (pMovement->iStartX != pMovement->iEndX) {
      fprintf(stderr, "Erro: Movimento de coluna inválido.\n");
      return 0;
    }
  }

  if (pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_LINE) {
    if (pMovement->iStartY != pMovement->iEndY) {
      fprintf(stderr, "Erro: Movimento de linha inválido.\n");
      return 0;
    }
  }

  if (pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_DIAGONAL) {
    if (abs(pMovement->iStartX - pMovement->iEndX) != abs(pMovement->iStartY - pMovement->iEndY)) {
      fprintf(stderr, "Erro: Movimento diagonal inválido.\n");
      return 0;
    }
  }

  if (pMovement->ui16MovementFlags & MOVEMENT_DIRECTION_L) {
    int dx = abs(pMovement->iStartX - pMovement->iEndX);
    int dy = abs(pMovement->iStartY - pMovement->iEndY);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
      fprintf(stderr, "Erro: Movimento em L inválido.\n");
      return 0;
    }
  }

  return 1; // Movimento válido
}

// Função para calcular direções disponíveis a partir de uma posição
void vCalculateDirections(int iX, int iY, int *aiDirections) {
  if (!aiDirections) return;

  // Inicializa todas as direções como inválidas
  for (int i = 0; i < 4; i++) aiDirections[i] = 0;

  // Norte
  if (iY > 0) aiDirections[NORTH] = 1;
  // Sul
  if (iY < 7) aiDirections[SOUTH] = 1;
  // Oeste
  if (iX > 0) aiDirections[WEST] = 1;
  // Leste
  if (iX < 7) aiDirections[EAST] = 1;
}

// Função para imprimir direções (opcional, útil para debugging)
void vPrintDirections(const int *aiDirections) {
  if (!aiDirections) return;

  printf("Direções disponíveis:\n");
  if (aiDirections[NORTH]) printf("Norte\n");
  if (aiDirections[SOUTH]) printf("Sul\n");
  if (aiDirections[WEST]) printf("Oeste\n");
  if (aiDirections[EAST]) printf("Leste\n");
}
void vMovePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iFromRow, int iFromCol, int iToRow, int iToCol) {
  pBoard[iToRow][iToCol] = pBoard[iFromRow][iFromCol];
  pBoard[iFromRow][iFromCol].pszType = SQUARE_TYPE_BLANK;
  pBoard[iFromRow][iFromCol].ui8Side = BLANK_SIDE;
}
void vCapturePiece(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iFromRow, int iFromCol, int iToRow, int iToCol) {
  // Realiza a captura (pode envolver lógica adicional)
  vMovePiece(pBoard, iFromRow, iFromCol, iToRow, iToCol);
}
