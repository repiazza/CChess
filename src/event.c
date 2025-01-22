#include <event.h>
#include <trace.h>
#include <board.h>
#include <movement.h>

// Gerencia o clique do mouse no tabuleiro
void vHandleMouseClickEvent(SDL_Event *pEvent, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
    static int iSelectedRow = -1, iSelectedCol = -1; // Seleção atual

    if (pEvent->type != SDL_MOUSEBUTTONDOWN) return;

    int iMouseX = pEvent->button.x;
    int iMouseY = pEvent->button.y;

    int iRow = iMouseY / SQUARE_SIZE;
    int iCol = iMouseX / SQUARE_SIZE;

    // Verifica se o clique está dentro do tabuleiro
    if (iRow < 0 || iRow >= ROW_SQUARE_COUNT || iCol < 0 || iCol >= COLUMN_SQUARE_COUNT) return;

    if (iSelectedRow == -1 || iSelectedCol == -1) {
        // Nenhuma seleção prévia: selecionar peça e calcular movimentos possíveis
        if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) != 0) {
            iSelectedRow = iRow;
            iSelectedCol = iCol;
            vHighlightPieceMoves(pBoard, iRow, iCol);
        }
    } else {
        // Com seleção prévia: mover, capturar ou redefinir seleção
        if (pBoard[iRow][iCol].bHighlighted) {
            if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) == 0) {
                vMovePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol); // Movimento simples
            } else {
                vCapturePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol); // Captura
            }
            vClearHighlights(pBoard);
            iSelectedRow = -1;
            iSelectedCol = -1;
        } else if (iRow == iSelectedRow && iCol == iSelectedCol) {
            // Clique na mesma casa: cancelar seleção
            vClearHighlights(pBoard);
            iSelectedRow = -1;
            iSelectedCol = -1;
        } else {
            // Selecionar outra peça
            vClearHighlights(pBoard);
            iSelectedRow = iRow;
            iSelectedCol = iCol;
            vHighlightPieceMoves(pBoard, iRow, iCol);
        }
    }
}

void vHighlightPieceMoves(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
    const char *pszType = pBoard[iRow][iCol].pszType;
    uint8_t ui8Side = pBoard[iRow][iCol].ui8Side;

    if (strcmp(pszType, SQUARE_TYPE_ROOK_PIECE) == 0) {
        vHighlightRookMoves(pBoard, iRow, iCol, ui8Side);
    } else if (strcmp(pszType, SQUARE_TYPE_BISHOP_PIECE) == 0) {
        vHighlightBishopMoves(pBoard, iRow, iCol);
    } else if (strcmp(pszType, SQUARE_TYPE_QUEEN_PIECE) == 0) {
        vHighlightQueenMoves(pBoard, iRow, iCol, ui8Side);
    } else if (strcmp(pszType, SQUARE_TYPE_KING_PIECE) == 0) {
        vHighlightKingMoves(pBoard, iRow, iCol);
    } else if (strcmp(pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0) {
        vHighlightKnightMoves(pBoard, iRow, iCol);
    } else if (strcmp(pszType, SQUARE_TYPE_PAWN_PIECE) == 0) {
        vHighlightPawnMoves(pBoard, iRow, iCol, ui8Side); // Certifique-se de que esta linha está presente
    }
}



// Gerencia eventos de clique do mouse
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event) {
    if (pSDL_Event_Event->type == SDL_MOUSEBUTTONDOWN) {
        int iMouseX = pSDL_Event_Event->button.x;
        int iMouseY = pSDL_Event_Event->button.y;

        int iSquareX = iMouseX / SQUARE_SIZE; // Determina a coluna
        int iSquareY = iMouseY / SQUARE_SIZE; // Determina a linha

        vTraceVarArgs("Clique detectado: (%d, %d) -> Casa (%d, %d)", 
                      iMouseX, iMouseY, iSquareX, iSquareY);
    }
}

// Lida com eventos de saída
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event) {
    if (pSDL_Event_Event->type == SDL_QUIT) {
        vTraceMsg("Evento de saída detectado. Encerrando o jogo.");
    }
}
