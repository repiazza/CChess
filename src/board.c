#include <board.h>
#include <util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <movement.h>

/* Limpa todos os destaques do tabuleiro */
void vClearHighlights(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int ii = 0;
  int jj = 0;
  for ( ii = 0; ii < ROW_SQUARE_COUNT; ii++ ) {
    for ( jj = 0; jj < COLUMN_SQUARE_COUNT; jj++) {
      pBoard[ii][jj].bHighlighted = FALSE;
    }
  }
}

/* Função auxiliar para criar uma casa */
PSTRUCT_SQUARE pCreateSquare(const char *pszType, uint8_t ui8Color, uint8_t ui8Side) {
  PSTRUCT_SQUARE pSquare = (PSTRUCT_SQUARE)malloc(sizeof(STRUCT_SQUARE));
  
  if ( !pSquare ) {
    vTraceError( "Erro: Falha ao alocar memória para uma casa.\n");
    return NULL;
  }
  
  pSquare->pszType = pszType ? strdup(pszType) : strdup(SQUARE_TYPE_BLANK);
  pSquare->ui8Color = ui8Color;
  pSquare->ui8Side = ui8Side;
  pSquare->bHighlighted = FALSE;

  return pSquare;
}

/* Função auxiliar para configurar uma casa */
void vSetSquare(STRUCT_SQUARE *pSquare, char *pszType, uint8_t ui8Color, uint8_t ui8Side) {
  pSquare->pszType = pszType;
  pSquare->ui8Color = ui8Color;
  pSquare->ui8Side = ui8Side;
  pSquare->bHighlighted = FALSE;
}

void vInitializeBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int ii = 0;
  int jj = 0;
  for ( ii = 0; ii < ROW_SQUARE_COUNT; ii++ ) {
    for ( jj = 0; jj < COLUMN_SQUARE_COUNT; jj++ ) {
      pBoard[ii][jj].pszType = strdup(SQUARE_TYPE_BLANK);  /* Tipo inicial */
      pBoard[ii][jj].ui8Side = NEUTRAL_SIDE;               /* Lado inicial */
      pBoard[ii][jj].bHighlighted = FALSE;                 /* Sem destaque */
    }
  }

  /* Configurar peças brancas (linha 0 e 1) */
  pBoard[0][0].pszType = strdup(SQUARE_TYPE_ROOK_PIECE);   pBoard[0][0].ui8Side = FRIENDLY_SIDE;
  pBoard[0][1].pszType = strdup(SQUARE_TYPE_KNIGHT_PIECE); pBoard[0][1].ui8Side = FRIENDLY_SIDE;
  pBoard[0][2].pszType = strdup(SQUARE_TYPE_BISHOP_PIECE); pBoard[0][2].ui8Side = FRIENDLY_SIDE;
  pBoard[0][3].pszType = strdup(SQUARE_TYPE_QUEEN_PIECE);  pBoard[0][3].ui8Side = FRIENDLY_SIDE;
  pBoard[0][4].pszType = strdup(SQUARE_TYPE_KING_PIECE);   pBoard[0][4].ui8Side = FRIENDLY_SIDE;
  pBoard[0][5].pszType = strdup(SQUARE_TYPE_BISHOP_PIECE); pBoard[0][5].ui8Side = FRIENDLY_SIDE;
  pBoard[0][6].pszType = strdup(SQUARE_TYPE_KNIGHT_PIECE); pBoard[0][6].ui8Side = FRIENDLY_SIDE;
  pBoard[0][7].pszType = strdup(SQUARE_TYPE_ROOK_PIECE);   pBoard[0][7].ui8Side = FRIENDLY_SIDE;

  for ( jj = 0; jj < COLUMN_SQUARE_COUNT; jj++ ) {
    pBoard[1][jj].pszType = strdup(SQUARE_TYPE_PAWN_PIECE);
    pBoard[1][jj].ui8Side = FRIENDLY_SIDE;
  }

  /* Configurar peças pretas (linha 6 e 7) */
  pBoard[7][0].pszType = strdup(SQUARE_TYPE_ROOK_PIECE);   pBoard[7][0].ui8Side = ENEMY_SIDE;
  pBoard[7][1].pszType = strdup(SQUARE_TYPE_KNIGHT_PIECE); pBoard[7][1].ui8Side = ENEMY_SIDE;
  pBoard[7][2].pszType = strdup(SQUARE_TYPE_BISHOP_PIECE); pBoard[7][2].ui8Side = ENEMY_SIDE;
  pBoard[7][3].pszType = strdup(SQUARE_TYPE_QUEEN_PIECE);  pBoard[7][3].ui8Side = ENEMY_SIDE;
  pBoard[7][4].pszType = strdup(SQUARE_TYPE_KING_PIECE);   pBoard[7][4].ui8Side = ENEMY_SIDE;
  pBoard[7][5].pszType = strdup(SQUARE_TYPE_BISHOP_PIECE); pBoard[7][5].ui8Side = ENEMY_SIDE;
  pBoard[7][6].pszType = strdup(SQUARE_TYPE_KNIGHT_PIECE); pBoard[7][6].ui8Side = ENEMY_SIDE;
  pBoard[7][7].pszType = strdup(SQUARE_TYPE_ROOK_PIECE);   pBoard[7][7].ui8Side = ENEMY_SIDE;

  for ( jj = 0; jj < COLUMN_SQUARE_COUNT; jj++ ) {
    pBoard[6][jj].pszType = strdup(SQUARE_TYPE_PAWN_PIECE);
    pBoard[6][jj].ui8Side = ENEMY_SIDE;
  }

  printf("Tabuleiro inicializado corretamente com todas as peças na posição inicial.\n");
}

/* Imprime o tabuleiro para depuração */
void vPrintBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int iRow = 0;
  int iCol = 0;
  for ( iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++ ) {
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      printf("[%s%s] ",
        pBoard[iRow][iCol].pszType,
        pBoard[iRow][iCol].bHighlighted ? "*" : ""
      );
    }
    printf("\n");
  }
}

/* Destaca ou remove o destaque de uma casa */
void vHighlightSquare(STRUCT_SQUARE *pSquare, int bHighlighted) {
  if ( pSquare ) {
    pSquare->bHighlighted = bHighlighted;
  }
}
