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
#include <snapshot.h>
#include <rect.h>
#include <cmdline.h>

/* Inicializações de fontes */
#define FONT_PATH "./fonts/FiraCode.ttf" /* Substitua pelo caminho correto */
#define FONT_SIZE 24

/* Nome do programa */
const char *gkpszProgramName;

/**
 * Renderiza texto na tela.
 */
void vRenderText(SDL_Renderer *pRenderer, const char *pszText, int x, int y, SDL_Color color, TTF_Font *pFont) {
  SDL_Surface *pSurface = NULL;
  SDL_Texture *pTexture = NULL;
  SDL_Rect rect;
  
  if ( !pszText || !pFont ) {
    vTraceError("Erro: Texto ou fonte inválidos. Texto: %s\n", pszText ? pszText : "NULL");
    return;
  }

  pSurface = TTF_RenderText_Solid(pFont, pszText, color);
  if ( !pSurface ) {
    vTraceError("Erro ao renderizar texto: %s\n", TTF_GetError());
    return;
  }

  pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
  if ( !pTexture ) {
    vTraceError("Erro ao criar textura de texto: %s\n", SDL_GetError());
    SDL_FreeSurface(pSurface);
    return;
  }

  iRECT_SetDimensions(&rect, (SDL_Rect){x, y, pSurface->w, pSurface->h});
  SDL_RenderCopy(pRenderer, pTexture, NULL, &rect);

  SDL_FreeSurface(pSurface);
  SDL_DestroyTexture(pTexture);
}

/**
 * Renderiza o tabuleiro e as peças.
 */
void vDrawBoard(SDL_Renderer *pRenderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int iRow = 0;
  int iCol = 0;
  SDL_Color SDL_COLOR_Dark = SDL_COLOR_GetDarkSquareColor();
  SDL_Color SDL_COLOR_Light = SDL_COLOR_GetLightSquareColor();
  SDL_Color SDL_COLOR_Highlight = {255, 255, 0, 128}; /* Amarelo semi-transparente */

  /* Inverte apenas o loop de desenho das linhas */
  for ( iRow = ROW_SQUARE_COUNT - 1; iRow >= 0; iRow-- ) { /* Decrementa as linhas */
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      /* Determinar a cor da casa */
      SDL_Rect SDL_Rect_Square = {iCol * SQUARE_SIZE, (ROW_SQUARE_COUNT - 1 - iRow) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
      SDL_Color SDL_COLOR_Current = (iRow + iCol) % 2 == 0 ? SDL_COLOR_Light : SDL_COLOR_Dark;
      const char *pszPieceName = NULL;
      
      /* Desenhar o fundo da casa */
      SDL_SetRenderDrawColor(pRenderer, SDL_COLOR_Current.r, SDL_COLOR_Current.g, SDL_COLOR_Current.b, SDL_COLOR_Current.a);
      SDL_RenderFillRect(pRenderer, &SDL_Rect_Square);

      /* Destacar casas disponíveis */
      if ( pBoard[iRow][iCol].bHighlighted) {
        SDL_SetRenderDrawColor(pRenderer, SDL_COLOR_Highlight.r, SDL_COLOR_Highlight.g, SDL_COLOR_Highlight.b, SDL_COLOR_Highlight.a);
        SDL_RenderFillRect(pRenderer, &SDL_Rect_Square);
      }

      /* Desenhar peça */
      pszPieceName = pszGetPieceName(&pBoard[iRow][iCol]);
      if ( pszPieceName && strlen(pszPieceName) > 0 ) {
        SDL_Color SDL_COLOR_PieceText = (pBoard[iRow][iCol].ui8Side == FRIENDLY_SIDE)
                                            ? (SDL_Color){255, 255, 255, 255}
                                            : (SDL_Color){0, 0, 0, 255};
        vRenderText(
          pRenderer,
          pszPieceName,
          iCol * SQUARE_SIZE + SQUARE_SIZE / 4,
          (ROW_SQUARE_COUNT - 1 - iRow) * SQUARE_SIZE + SQUARE_SIZE / 4,
          SDL_COLOR_PieceText,
          pFont
        );
      }
    }
  }
}

static void vShowUsage(void) {
  char szMsg[256] = "";
  memset(szMsg, 0x00, sizeof(szMsg));
  
  snprintf(
    szMsg,
    sizeof(szMsg),
    "Usage: %s --parameter=<argument>\n"
    "Where parameter will be the following:\n",
    gkpszProgramName
  );
  
  vShowSyntax(szMsg, astCmdOpt);
}

static void vShowVersion(void) {
  printf(
    "%s [%s %s]\n",
    gkpszProgramName,
    __DATE__,
    __TIME__
  );
}

/**
 * Função principal do programa.
 */
int SDL_main(int iArgc, char *pszArgv[], char *pszEnvp[]) {
  int bRunning = TRUE;
  PSTRUCT_BOARD_HISTORY pstHistory;
  SDL_Event event;
  TTF_Font *pFont = NULL;
  SDL_Window *pWindow;
  SDL_Renderer *pRenderer;
  STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT];
  
  UNUSED(pstHistory);
  
  gkpszProgramName = szGetProgramName(pszArgv[0]);
  if ( !bCommandLineIsOK(iArgc, pszArgv, astCmdOpt) ) {
    vShowUsage();
    return -1;
  }
  
  if ( astCmdOpt[0].bSet ) {
    vShowUsage();
    return 0;
  }
  
  if ( astCmdOpt[1].bSet ) {
    vShowVersion();
    return 0;
  }
  
  vInitTraceConfig();
  if ( DEBUG_MSGS ) {
    vTraceBegin();
    vTraceCommandLine(iArgc, pszArgv);
  }
  if ( DEBUG_MORE_MSGS ) vTraceEnvp(pszEnvp);

  if ( !bLoadCfgFile(gszConfFile) ) {
    if ( DEBUG_MSGS ) vTraceError("Erro: Não foi possível carregar o arquivo de configuração.");
    fprintf(stderr, "Erro: Não foi possível carregar o arquivo de configuração.\n");
    exit(EXIT_FAILURE);
  }


  if ( SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0 ) {
    vTraceError("Erro ao inicializar SDL ou TTF: %s", SDL_GetError());
    return -1;
  }

  pWindow = SDL_CreateWindow(
    "CSDL Chessboard",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    atoi(gstCmdLine.szWinWidth),
    atoi(gstCmdLine.szWinHeight),
    SDL_WINDOW_SHOWN
  );
  if ( !pWindow ) {
    vTraceError("Erro ao criar janela: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
  if ( !pRenderer ) {
    vTraceError("Erro ao criar renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return -1;
  }
  
  pFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  if ( !pFont ) {
    vTraceError("Erro ao carregar fonte: %s\n", TTF_GetError());
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return -1;
  }

  vInitializeBoard(pBoard);
  pstHistory = pstCreateHistory();

  while ( bRunning ) {
    while ( SDL_PollEvent(&event) ) {
      if ( event.type == SDL_QUIT ) {
        bRunning = FALSE;
      }
      else if ( event.type == SDL_MOUSEBUTTONDOWN ) {
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

  vTraceEnd();

  return 0;
}
