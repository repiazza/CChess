#include <board.h>
#include <util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <movement.h>

// Função para destacar todas as casas adjacentes a uma posição
void vHighlightMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  static const int aiOffsets[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    { 0, -1},          { 0, 1},
    { 1, -1}, { 1, 0}, { 1, 1}
  };

  for (int i = 0; i < 8; i++) {
    int iNewRow = iRow + aiOffsets[i][0];
    int iNewCol = iCol + aiOffsets[i][1];

    if (iNewRow >= 0 && iNewRow < ROW_SQUARE_COUNT &&
        iNewCol >= 0 && iNewCol < COLUMN_SQUARE_COUNT) {
      pBoard[iNewRow][iNewCol].bHighlighted = TRUE;
    }
  }
}

// Limpa todos os destaques do tabuleiro
void vClearHighlights(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  for (int i = 0; i < ROW_SQUARE_COUNT; i++) {
    for (int j = 0; j < COLUMN_SQUARE_COUNT; j++) {
      pBoard[i][j].bHighlighted = FALSE;
    }
  }
}

// Função auxiliar para criar uma casa
PSTRUCT_SQUARE pCreateSquare(const char *pszType, uint8_t ui8Color, uint8_t ui8Side) {
  PSTRUCT_SQUARE pSquare = (PSTRUCT_SQUARE)malloc(sizeof(STRUCT_SQUARE));
  if (!pSquare) {
    fprintf(stderr, "Erro: Falha ao alocar memória para uma casa.\n");
    return NULL;
  }

  pSquare->pszType = pszType ? strdup(pszType) : strdup(SQUARE_TYPE_BLANK);
  pSquare->ui8Color = ui8Color;
  pSquare->ui8Side = ui8Side;
  pSquare->bHighlighted = FALSE;

  return pSquare;
}

// Função auxiliar para configurar uma casa
void vSetSquare(STRUCT_SQUARE *pSquare, const char *pszType, uint8_t ui8Color, uint8_t ui8Side) {
  pSquare->pszType = pszType;
  pSquare->ui8Color = ui8Color;
  pSquare->ui8Side = ui8Side;
  pSquare->bHighlighted = FALSE;
}

// Inicializa o tabuleiro com peças e cores padrão
void vInitializeBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  for (int i = 0; i < ROW_SQUARE_COUNT; i++) {
    for (int j = 0; j < COLUMN_SQUARE_COUNT; j++) {
      vSetSquare(&pBoard[i][j], SQUARE_TYPE_BLANK, BLANK_SQUARE_COLOR, BLANK_SIDE);
    }
  }

  // Configuração inicial das peças
  const char *aszInitialPieces[] = {
    SQUARE_TYPE_ROOK_PIECE, SQUARE_TYPE_KNIGHT_PIECE, SQUARE_TYPE_BISHOP_PIECE,
    SQUARE_TYPE_QUEEN_PIECE, SQUARE_TYPE_KING_PIECE, SQUARE_TYPE_BISHOP_PIECE,
    SQUARE_TYPE_KNIGHT_PIECE, SQUARE_TYPE_ROOK_PIECE
  };

  // Linhas de peças principais
  for (int j = 0; j < COLUMN_SQUARE_COUNT; j++) {
    vSetSquare(&pBoard[0][j], aszInitialPieces[j], LIGHT_BGCOLOR, FRIENDLY_SIDE);
    vSetSquare(&pBoard[7][j], aszInitialPieces[j], DARK_BGCOLOR, ENEMY_SIDE);
  }

  // Linhas de peões
  for (int j = 0; j < COLUMN_SQUARE_COUNT; j++) {
    vSetSquare(&pBoard[1][j], SQUARE_TYPE_PAWN_PIECE, LIGHT_BGCOLOR, FRIENDLY_SIDE);
    vSetSquare(&pBoard[6][j], SQUARE_TYPE_PAWN_PIECE, DARK_BGCOLOR, ENEMY_SIDE);
  }
}

// Imprime o tabuleiro para depuração
void vPrintBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  for (int iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++) {
    for (int iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++) {
      printf("[%s%s] ",
        pBoard[iRow][iCol].pszType,
        pBoard[iRow][iCol].bHighlighted ? "*" : ""
      );
    }
    printf("\n");
  }
}

// Destaca ou remove o destaque de uma casa
void vHighlightSquare(STRUCT_SQUARE *pSquare, int bHighlighted) {
  if (pSquare) {
    pSquare->bHighlighted = bHighlighted;
  }
}
