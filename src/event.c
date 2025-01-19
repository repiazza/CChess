#include <event.h>
#include <trace.h>
#include <board.h>
#include <movement.h>


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
        // Sem seleção prévia: selecionar e calcular movimentos possíveis
        if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) != 0) {
            iSelectedRow = iRow;
            iSelectedCol = iCol;

            if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_ROOK_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, LINE_OF_SIGHT, MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BISHOP_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, LINE_OF_SIGHT, MOVEMENT_DIRECTION_DIAGONAL);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_QUEEN_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, LINE_OF_SIGHT, MOVEMENT_DIRECTION_ALL);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_KING_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, SQUARE_RANGE, MOVEMENT_DIRECTION_ALL);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, L_RANGE, MOVEMENT_DIRECTION_L);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_PAWN_PIECE) == 0) {
                uint16_t direction = (pBoard[iRow][iCol].ui8Side == FRIENDLY_SIDE) ? NORTH : SOUTH;
                vCalculateAvailableMoves(pBoard, iRow, iCol, SQUARE_RANGE, direction | MOVEMENT_DIRECTION_COLUMN);
            }
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
            // Selecionar outra casa
            vClearHighlights(pBoard);
            iSelectedRow = iRow;
            iSelectedCol = iCol;

            if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_ROOK_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, LINE_OF_SIGHT, MOVEMENT_DIRECTION_COLUMN | MOVEMENT_DIRECTION_LINE);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BISHOP_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, LINE_OF_SIGHT, MOVEMENT_DIRECTION_DIAGONAL);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_QUEEN_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, LINE_OF_SIGHT, MOVEMENT_DIRECTION_ALL);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_KING_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, SQUARE_RANGE, MOVEMENT_DIRECTION_ALL);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_KNIGHT_PIECE) == 0) {
                vCalculateAvailableMoves(pBoard, iRow, iCol, L_RANGE, MOVEMENT_DIRECTION_L);
            } else if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_PAWN_PIECE) == 0) {
                uint16_t direction = (pBoard[iRow][iCol].ui8Side == FRIENDLY_SIDE) ? NORTH : SOUTH;
                vCalculateAvailableMoves(pBoard, iRow, iCol, SQUARE_RANGE, direction | MOVEMENT_DIRECTION_COLUMN);
            }
        }
    }
}




// Lida com eventos de clique do mouse
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event) {
    if (pSDL_Event_Event->type == SDL_MOUSEBUTTONDOWN) {
        int iMouseX = pSDL_Event_Event->button.x;
        int iMouseY = pSDL_Event_Event->button.y;

        int iSquareX = iMouseX / 80; // 80 é o tamanho do quadrado
        int iSquareY = iMouseY / 80;

        vTraceVarArgs("Clique detectado: (%d, %d) -> Casa (%d, %d)", 
                      iMouseX, iMouseY, iSquareX, iSquareY);

        // Aqui você pode adicionar lógica para selecionar peças, validar movimentos, etc.
    }
}

// Lida com eventos de saída
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event) {
    if (pSDL_Event_Event->type == SDL_QUIT) {
        vTraceMsg("Evento de saída detectado. Encerrando o jogo.");
    }
}
