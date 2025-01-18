#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <trace.h>
#include <colors.h>
#include <cmdline.h>
#include <consts.h>
#include <event.h>
#include <board.h>
#include <util.h>
#include <piece.h>

// Definições do tabuleiro
// #define WINDOW_WIDTH 640
// #define WINDOW_HEIGHT 640

// Inicializações de fontes
#define FONT_PATH "fonts/FiraCode.ttf" // Substitua pelo caminho correto
#define FONT_SIZE 24


/* Receive the name of program */
const char *gkpszProgramName;

// Função auxiliar para desenhar texto em uma posição específica
void vRenderText(SDL_Renderer *pRenderer, const char *pszText, int x, int y, SDL_Color color, TTF_Font *pFont) {
    if (!pszText || !pFont) {
        printf("Erro: Texto ou fonte inválidos. Texto: %s\n", pszText);
        return;
    }
    // SDL_Color textColor = {255, 255, 255, 255}; // Branco
    SDL_Surface *pSurface = TTF_RenderText_Solid(pFont, pszText, color);
    if (!pSurface) {
        printf("Erro ao renderizar texto: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    if (!pTexture) {
        printf("Erro ao criar textura de texto: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurface);
        return;
    }

    SDL_Rect rect = {x, y, pSurface->w, pSurface->h};
    SDL_RenderCopy(pRenderer, pTexture, NULL, &rect);

    SDL_FreeSurface(pSurface);
    SDL_DestroyTexture(pTexture);
}


void vDrawBoard(SDL_Renderer *pSDL_Renderer_Renderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
    SDL_Color SDL_COLOR_Dark = SDL_COLOR_GetDarkSquareColor();
    SDL_Color SDL_COLOR_Light = SDL_COLOR_GetLightSquareColor();
    SDL_Color SDL_COLOR_Highlight = {255, 255, 0, 128}; // Amarelo semi-transparente

    for (int iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++) {
        for (int iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++) {
            // Determinar a cor da casa
            SDL_Rect SDL_Rect_Square = {iCol * SQUARE_SIZE, iRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_Color SDL_COLOR_Current = (iRow + iCol) % 2 == 0 ? SDL_COLOR_Light : SDL_COLOR_Dark;

            // Desenhar o fundo da casa
            SDL_SetRenderDrawColor(pSDL_Renderer_Renderer, SDL_COLOR_Current.r, SDL_COLOR_Current.g, SDL_COLOR_Current.b, SDL_COLOR_Current.a);
            SDL_RenderFillRect(pSDL_Renderer_Renderer, &SDL_Rect_Square);

            // Destacar casa, se aplicável
            if (pBoard[iRow][iCol].bHighlighted) {
                SDL_SetRenderDrawColor(pSDL_Renderer_Renderer, SDL_COLOR_Highlight.r, SDL_COLOR_Highlight.g, SDL_COLOR_Highlight.b, SDL_COLOR_Highlight.a);
                SDL_RenderFillRect(pSDL_Renderer_Renderer, &SDL_Rect_Square);
            }

            // Obter o nome da peça
            const char *pszPieceName = pszGetPieceName(&pBoard[iRow][iCol]);

            if (pszPieceName && strlen(pszPieceName) > 0) {
                vRenderText(
                    pSDL_Renderer_Renderer, 
                    pszPieceName, 
                    iCol * SQUARE_SIZE + SQUARE_SIZE / 4, 
                    iRow * SQUARE_SIZE + SQUARE_SIZE / 4, 
                    SDL_COLOR_Current, 
                    pFont
                );
            }
        }
    }
}



// Função principal
int SDL_main(int iArgc, char *pszArgv[]) {

    gkpszProgramName = szGetProgramName( pszArgv[0] );
    vInitTraceConfig();

    if ( !bLoadCfgFile(gszConfFile) ) {
        fprintf(stderr, "E: impossible load .conf file\n");

        exit( EXIT_FAILURE );
    }

    vTraceBegin();

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        vTraceVarArgs("Erro ao inicializar SDL ou TTF: %s", SDL_GetError());
        return -1;
    }

    SDL_Window *pSDL_Window_Window = SDL_CreateWindow(
        "CSDL Chessboard", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        atoi(gstCmdLine.szWinWidth), 
        atoi(gstCmdLine.szWinHeight), 
        SDL_WINDOW_SHOWN
    );
    if (!pSDL_Window_Window) {
        vTraceVarArgs("Erro ao criar janela: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *pSDL_Renderer_Renderer = SDL_CreateRenderer(
        pSDL_Window_Window, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );

    if (!pSDL_Renderer_Renderer) {
        vTraceVarArgs("Erro ao criar renderer: %s", SDL_GetError());
        SDL_DestroyWindow(pSDL_Window_Window);
        SDL_Quit();
        return -1;
    }

    TTF_Font *pFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!pFont) {
        vTraceVarArgs("Erro ao carregar fonte: %s", TTF_GetError());
        SDL_DestroyRenderer(pSDL_Renderer_Renderer);
        SDL_DestroyWindow(pSDL_Window_Window);
        SDL_Quit();
        return -1;
    }

    // Inicializa tabuleiro
    STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT];
    vInitializeBoard(pBoard);

    // Loop principal
    int bRunning = TRUE;
    SDL_Event SDL_Event_Event;
    while (bRunning) {
        while (SDL_PollEvent(&SDL_Event_Event)) {
            if (SDL_Event_Event.type == SDL_QUIT) {
            bRunning = FALSE;
            } else if (SDL_Event_Event.type == SDL_MOUSEBUTTONDOWN) {
                vHandleMouseClickEvent(&SDL_Event_Event, pBoard);
            }
        }


        SDL_SetRenderDrawColor(pSDL_Renderer_Renderer, 0, 0, 0, 255);
        SDL_RenderClear(pSDL_Renderer_Renderer);

        vDrawBoard(pSDL_Renderer_Renderer, pFont, pBoard);

        SDL_RenderPresent(pSDL_Renderer_Renderer);
    }

    // Finaliza recursos
    TTF_CloseFont(pFont);
    SDL_DestroyRenderer(pSDL_Renderer_Renderer);
    SDL_DestroyWindow(pSDL_Window_Window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
