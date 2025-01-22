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
#include <movement.h>

// Inicializações de fontes
#define FONT_PATH "fonts/FiraCode.ttf" // Substitua pelo caminho correto
#define FONT_SIZE 24

/* Nome do programa */
const char *gkpszProgramName;

/**
 * Renderiza texto na tela.
 */
void vRenderText(SDL_Renderer *pRenderer, const char *pszText, int x, int y, SDL_Color color, TTF_Font *pFont) {
    if (!pszText || !pFont) {
        fprintf(stderr, "Erro: Texto ou fonte inválidos. Texto: %s\n", pszText ? pszText : "NULL");
        return;
    }

    SDL_Surface *pSurface = TTF_RenderText_Solid(pFont, pszText, color);
    if (!pSurface) {
        fprintf(stderr, "Erro ao renderizar texto: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    if (!pTexture) {
        fprintf(stderr, "Erro ao criar textura de texto: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurface);
        return;
    }

    SDL_Rect rect = {x, y, pSurface->w, pSurface->h};
    SDL_RenderCopy(pRenderer, pTexture, NULL, &rect);

    SDL_FreeSurface(pSurface);
    SDL_DestroyTexture(pTexture);
}

/**
 * Renderiza o tabuleiro e as peças.
 */
void vDrawBoard(SDL_Renderer *pRenderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
    SDL_Color SDL_COLOR_Dark = SDL_COLOR_GetDarkSquareColor();
    SDL_Color SDL_COLOR_Light = SDL_COLOR_GetLightSquareColor();
    SDL_Color SDL_COLOR_Highlight = {255, 255, 0, 128}; // Amarelo semi-transparente

    for (int iRow = 0; iRow < ROW_SQUARE_COUNT; iRow++) {
        for (int iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++) {
            // Determinar a cor da casa
            SDL_Rect SDL_Rect_Square = {iCol * SQUARE_SIZE, iRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_Color SDL_COLOR_Current = (iRow + iCol) % 2 == 0 ? SDL_COLOR_Light : SDL_COLOR_Dark;

            // Desenhar o fundo da casa
            SDL_SetRenderDrawColor(pRenderer, SDL_COLOR_Current.r, SDL_COLOR_Current.g, SDL_COLOR_Current.b, SDL_COLOR_Current.a);
            SDL_RenderFillRect(pRenderer, &SDL_Rect_Square);

            // Destacar casas disponíveis
            if (pBoard[iRow][iCol].bHighlighted) {
                SDL_SetRenderDrawColor(pRenderer, SDL_COLOR_Highlight.r, SDL_COLOR_Highlight.g, SDL_COLOR_Highlight.b, SDL_COLOR_Highlight.a);
                SDL_RenderFillRect(pRenderer, &SDL_Rect_Square);
            }

            // Desenhar peça
            const char *pszPieceName = pszGetPieceName(&pBoard[iRow][iCol]);
            if (pszPieceName && strlen(pszPieceName) > 0) {
                SDL_Color SDL_COLOR_PieceText = (pBoard[iRow][iCol].ui8Side == FRIENDLY_SIDE)
                                                    ? (SDL_Color){255, 255, 255, 255}
                                                    : (SDL_Color){0, 0, 0, 255};
                vRenderText(
                    pRenderer,
                    pszPieceName,
                    iCol * SQUARE_SIZE + SQUARE_SIZE / 4,
                    iRow * SQUARE_SIZE + SQUARE_SIZE / 4,
                    SDL_COLOR_PieceText,
                    pFont);
            }
        }
    }
}

/**
 * Função principal do programa.
 */
int SDL_main(int iArgc, char *pszArgv[]) {
    gkpszProgramName = szGetProgramName(pszArgv[0]);
    vInitTraceConfig();

    if (!bLoadCfgFile(gszConfFile)) {
        fprintf(stderr, "Erro: Não foi possível carregar o arquivo de configuração.\n");
        exit(EXIT_FAILURE);
    }

    vTraceBegin();

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        fprintf(stderr, "Erro ao inicializar SDL ou TTF: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *pWindow = SDL_CreateWindow(
        "CSDL Chessboard",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        atoi(gstCmdLine.szWinWidth),
        atoi(gstCmdLine.szWinHeight),
        SDL_WINDOW_SHOWN);
    if (!pWindow) {
        fprintf(stderr, "Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pRenderer) {
        fprintf(stderr, "Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return -1;
    }

    TTF_Font *pFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!pFont) {
        fprintf(stderr, "Erro ao carregar fonte: %s\n", TTF_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return -1;
    }

    STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT];
    vInitializeBoard(pBoard);

    int bRunning = TRUE;
    SDL_Event event;

    while (bRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                bRunning = FALSE;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                vHandleMouseClickEvent(&event, pBoard);
            }
        }

        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pRenderer);

        vDrawBoard(pRenderer, pFont, pBoard);

        SDL_RenderPresent(pRenderer);
    }

    TTF_CloseFont(pFont);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
