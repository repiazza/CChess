#include <piece.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

// Cria uma nova peça
PSTRUCT_PIECE pCreatePiece(char cType, uint8_t ui8Color) {
  PSTRUCT_PIECE pPiece = (PSTRUCT_PIECE)malloc(sizeof(STRUCT_PIECE));
  if (!pPiece) {
      fprintf(stderr, "Erro: Falha ao alocar memória para a peça.\n");
      return NULL;
  }

  pPiece->cType = cType;
  pPiece->ui8Color = ui8Color;
  pPiece->bHasMoved = FALSE;

  return pPiece;
}

// Libera a memória de uma peça
void vDestroyPiece(PSTRUCT_PIECE pPiece) {
  if (pPiece) {
      free(pPiece);
  }
}

int bValidateRookMove(int iStartRow, int iStartCol, int iEndRow, int iEndCol, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  // Só permite movimento em linha ou coluna
  if (iStartRow != iEndRow && iStartCol != iEndCol) {
    return FALSE;
  }

  // Verifica se a casa final tem uma peça do mesmo lado
  if (pBoard[iEndRow][iEndCol].ui8Side == pBoard[iStartRow][iStartCol].ui8Side) {
    return FALSE;
  }

  return TRUE;
}

int bValidateKnightMove(int iStartRow, int iStartCol, int iEndRow, int iEndCol, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int dx = abs(iEndRow - iStartRow);
  int dy = abs(iEndCol - iStartCol);

  // Movimento em L
  if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
    return FALSE;
  }

  // Verifica se a casa final tem uma peça do mesmo lado
  if (pBoard[iEndRow][iEndCol].ui8Side == pBoard[iStartRow][iStartCol].ui8Side) {
    return FALSE;
  }

  return TRUE;
}

int bValidateBishopMove(int iStartRow, int iStartCol, int iEndRow, int iEndCol, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int dx = abs(iEndRow - iStartRow);
  int dy = abs(iEndCol - iStartCol);

  // Movimento em diagonal
  if (dx != dy) {
    return FALSE;
  }

  // Verifica se a casa final tem uma peça do mesmo lado
  if (pBoard[iEndRow][iEndCol].ui8Side == pBoard[iStartRow][iStartCol].ui8Side) {
    return FALSE;
  }

  return TRUE;
}


// Verifica se uma peça é válida
int bIsPieceValid(PSTRUCT_PIECE pPiece) {
  if (!pPiece) return FALSE;

  switch (pPiece->cType) {
      case PIECE_TYPE_ROOK:
      case PIECE_TYPE_KNIGHT:
      case PIECE_TYPE_BISHOP:
      case PIECE_TYPE_QUEEN:
      case PIECE_TYPE_KING:
      case PIECE_TYPE_PAWN:
          return TRUE;
      default:
          return FALSE;
  }
}

// Verifica se uma peça pode se mover com base nos flags de movimento
int bCanMove(PSTRUCT_PIECE pPiece, uint16_t ui16MovementFlags) {
  if (!bIsPieceValid(pPiece)) return FALSE;

  switch (pPiece->cType) {
    case PIECE_TYPE_ROOK:
      return ui16MovementFlags & (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE);
    case PIECE_TYPE_KNIGHT:
      return ui16MovementFlags & MOVEMENT_DIRECTION_L;
    case PIECE_TYPE_BISHOP:
      return ui16MovementFlags & MOVEMENT_DIRECTION_DIAGONAL;
    case PIECE_TYPE_QUEEN:
      return ui16MovementFlags & (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE | MOVEMENT_DIRECTION_DIAGONAL) ;
    case PIECE_TYPE_KING:
      return ui16MovementFlags & (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE | MOVEMENT_DIRECTION_DIAGONAL | SPECIAL_MOVEMENT_CASTLE);
    case PIECE_TYPE_PAWN:
      return ui16MovementFlags & (MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_DIAGONAL | SPECIAL_MOVEMENT_EN_PASSANT);
    default:
        return FALSE;
  }
}

// Retorna o nome da peça como string
const char *pszGetPieceTypeName(char cType) {
  switch (cType) {
    case PIECE_TYPE_ROOK:
      return "Rook";
    case PIECE_TYPE_KNIGHT:
      return "Knight";
    case PIECE_TYPE_BISHOP:
      return "Bishop";
    case PIECE_TYPE_QUEEN:
      return "Queen";
    case PIECE_TYPE_KING:
      return "King";
    case PIECE_TYPE_PAWN:
      return "Pawn";
    default:
      return "Unknown";
  }
}
const char* pszGetPieceName(PSTRUCT_SQUARE pSquare) {
  if (!pSquare || !pSquare->pszType) {
    return "";
  }

  if (strcmp(pSquare->pszType, SQUARE_TYPE_PAWN_PIECE) == 0) return "P";
  if (strcmp(pSquare->pszType, SQUARE_TYPE_ROOK_PIECE) == 0) return "R";
  if (strcmp(pSquare->pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0) return "N";
  if (strcmp(pSquare->pszType, SQUARE_TYPE_BISHOP_PIECE) == 0) return "B";
  if (strcmp(pSquare->pszType, SQUARE_TYPE_QUEEN_PIECE) == 0) return "Q";
  if (strcmp(pSquare->pszType, SQUARE_TYPE_KING_PIECE) == 0) return "K";

  return "";
}

