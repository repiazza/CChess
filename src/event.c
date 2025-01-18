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

    if (iRow < 0 || iRow >= ROW_SQUARE_COUNT || iCol < 0 || iCol >= COLUMN_SQUARE_COUNT) return;

    if (iSelectedRow == -1 || iSelectedCol == -1) {
        // Sem seleção prévia: selecionar e destacar
        if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) != 0) {
            iSelectedRow = iRow;
            iSelectedCol = iCol;
            vHighlightMoves(pBoard, iRow, iCol);
        }
    } else {
        // Com seleção prévia: mover ou capturar
        if (pBoard[iRow][iCol].bHighlighted) {
            if (strcmp(pBoard[iRow][iCol].pszType, SQUARE_TYPE_BLANK) == 0) {
                // Movimento simples
                vMovePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol);
            } else {
                // Captura
                vCapturePiece(pBoard, iSelectedRow, iSelectedCol, iRow, iCol);
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
            vHighlightMoves(pBoard, iRow, iCol);
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
