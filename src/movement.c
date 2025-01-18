#include <stdlib.h>
#include <stdio.h>
#include <movement.h>

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
