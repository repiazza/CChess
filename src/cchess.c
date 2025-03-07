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
#include <openings.h>

/* Inicializações de fontes */
#define FONT_PATH "./fonts/FiraCode.ttf" /* Substitua pelo caminho correto */
#define FONT_SIZE 24

/* Nome do programa */
const char *gkpszProgramName;

/* bacagine - 05/03/2025 - Vamos renderizar a tela apenas quando algum evento ocorrer */
int gbRenderer = TRUE;

static int gbTtfStarted = FALSE;

char *gpszCurrentWhiteOpegning = NULL;
char *gpszCurrentBlackOpegning = NULL;

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

void vConvertBoard2String(char *pszOutput, size_t lOutputSize, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  int iRow = 0;
  int iCol = 0;
  
  if ( pszOutput == NULL || lOutputSize <= 0 ) return;
  
  snprintf(
    &pszOutput[strlen(pszOutput)],
    lOutputSize,
    "\n"
  );
  
  /* Inverte apenas o loop de desenho das linhas */
  for ( iRow = ROW_SQUARE_COUNT - 1; iRow >= 0; iRow-- ) { /* Decrementa as linhas */
    for ( iCol = 0; iCol < COLUMN_SQUARE_COUNT; iCol++ ) {
      const char *pszPieceName = NULL;
      
      pszPieceName = pszGetPieceName(&pBoard[iRow][iCol]);
      if ( pszPieceName && strlen(pszPieceName) > 0 ) {
        snprintf(
          &pszOutput[strlen(pszOutput)],
          lOutputSize,
          "%s",
          pszPieceName
        );
      }
      else {
        snprintf(
          &pszOutput[strlen(pszOutput)],
          lOutputSize,
          "_"
        );
      }
    }
    snprintf(
      &pszOutput[strlen(pszOutput)],
      lOutputSize,
      "\n"
    );
  }
}

void vTraceBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  char szDbg[256] = "";
  memset(szDbg, 0x00, sizeof(szDbg));
  vConvertBoard2String(szDbg, sizeof(szDbg), pBoard);
  vTraceMsg(szDbg);
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
      SDL_Rect SDL_Rect_Square = {iCol * SQUARE_SIZE, ((ROW_SQUARE_COUNT - 1 - iRow) * SQUARE_SIZE)+INFO_RECT_HEIGHT, SQUARE_SIZE, SQUARE_SIZE};
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
          (iCol * SQUARE_SIZE + SQUARE_SIZE / 4),
          ((ROW_SQUARE_COUNT - 1 - iRow) * SQUARE_SIZE + SQUARE_SIZE / 4)+50,
          SDL_COLOR_PieceText,
          pFont
        );
      }
    }
  }
  
  if ( DEBUG_MORE_MSGS ) vTraceBoard(pBoard);
}

int bInitSDL(Uint32 ui32SdlFlag) {
  if ( SDL_Init(ui32SdlFlag) < 0 || TTF_Init() < 0 ) {
    vTraceError("Erro ao inicializar SDL ou TTF: %s", SDL_GetError());
    return FALSE;
  }
  return TRUE;
}

void vEndSDL(TTF_Font *pFont, SDL_Renderer *pRenderer, SDL_Window *pWindow) {
  if ( pFont && gbTtfStarted )
    TTF_CloseFont(pFont);
  if ( pRenderer )
    SDL_DestroyRenderer(pRenderer);
  if ( pWindow )
    SDL_DestroyWindow(pWindow);
  if ( gbTtfStarted )
    TTF_Quit();
  SDL_Quit();
}

void vDrawHeaderRect(SDL_Renderer *pRenderer) {
  SDL_Rect stHeaderRect = { 0, 0, 800, INFO_RECT_HEIGHT };
  SDL_SetRenderDrawColor(pRenderer, 128, 128, 128, 255);
  SDL_RenderFillRect(pRenderer, &stHeaderRect);
}

void vDrawFooterRect(SDL_Renderer *pRenderer) {
  SDL_Rect stFooterRect = { 0, 690, 800, INFO_RECT_HEIGHT };
  SDL_SetRenderDrawColor(pRenderer, 128, 128, 128, 255);
  SDL_RenderFillRect(pRenderer, &stFooterRect);
}

void vDrawWhiteOpenings(SDL_Renderer *pRenderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  char szBoard[124] = "";
  char szText[512] = "";
  char *pszOpeningName = NULL;
  SDL_Color stBlackColor = { 0, 0, 0, 255 };
  int iX = 10;
  int iY = 700;
  
  memset(szBoard, 0x00, sizeof(szBoard));
  memset(szText , 0x00, sizeof(szText ));
  
  vConvertBoard2String(szBoard, sizeof(szBoard), pBoard);
  vRemoveChar(szBoard, sizeof(szBoard), '\n');
  
  pszOpeningName = pszGetOpeningName(szBoard);
  
  if ( !bStrIsEmpty(pszOpeningName) )
    gpszCurrentWhiteOpegning = pszOpeningName;
  
  snprintf(
    szText,
    sizeof(szText),
    "Opening: %s",
    bStrIsEmpty(gpszCurrentWhiteOpegning) ? "" : gpszCurrentWhiteOpegning
  );
  
  if ( DEBUG_MSGS ) vTraceMsg(szText);
  
  vRenderText(
    pRenderer,
    szText,
    iX,
    iY,
    stBlackColor,
    pFont
  );
}

void vDrawBlackOpenings(SDL_Renderer *pRenderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  char szBoard[124] = "";
  char szText[512] = "";
  char *pszOpeningName = NULL;
  SDL_Color stBlackColor = { 0, 0, 0, 255 };
  int iX = 10;
  int iY = 10;
  
  memset(szBoard, 0x00, sizeof(szBoard));
  memset(szText , 0x00, sizeof(szText ));
  
  vConvertBoard2String(szBoard, sizeof(szBoard), pBoard);
  vRemoveChar(szBoard, sizeof(szBoard), '\n');
  vInvertStr(szBoard);
  
  pszOpeningName = pszGetOpeningName(szBoard);
  
  if ( !bStrIsEmpty(pszOpeningName) )
    gpszCurrentBlackOpegning = pszOpeningName;
    
  snprintf(
    szText,
    sizeof(szText),
    "Opening: %s",
    bStrIsEmpty(gpszCurrentBlackOpegning) ? "" : gpszCurrentBlackOpegning
  );
  
  if ( DEBUG_MSGS ) vTraceMsg(szText);
  
  vRenderText(
    pRenderer,
    szText,
    iX,
    iY,
    stBlackColor,
    pFont
  );
}

void vDrawOpenings(SDL_Renderer *pRenderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  vDrawWhiteOpenings(pRenderer, pFont, pBoard);
  vDrawBlackOpenings(pRenderer, pFont, pBoard);
}

void vRenderer(SDL_Renderer *pRenderer, TTF_Font *pFont, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  if ( gbRenderer ) {
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);
    
    vDrawHeaderRect(pRenderer);
    vDrawBoard(pRenderer, pFont, pBoard);
    vDrawFooterRect(pRenderer);
    
    vDrawOpenings(pRenderer, pFont, pBoard);
    
    SDL_RenderPresent(pRenderer);
    gbRenderer = FALSE;
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
#ifdef LINUX
int SDL_main(int iArgc, char *pszArgv[], char *pszEnvp[]) {
#else
int SDL_main(int iArgc, char *pszArgv[]) {
#endif
  int bRunning = TRUE;
  PSTRUCT_BOARD_HISTORY pstHistory = NULL;
  SDL_Event event;
  TTF_Font *pFont = NULL;
  SDL_Window *pWindow = NULL;
  SDL_Renderer *pRenderer = NULL;
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
  #ifdef LINUX
  if ( DEBUG_MORE_MSGS ) vTraceEnvp(pszEnvp);
  #endif

  if ( !bLoadCfgFile(gszConfFile) ) {
    if ( DEBUG_MSGS ) vTraceError("Erro: Não foi possível carregar o arquivo de configuração.");
    fprintf(stderr, "Erro: Não foi possível carregar o arquivo de configuração.\n");
    exit(EXIT_FAILURE);
  }
  
  if ( !bInitSDL(SDL_INIT_VIDEO) )
    return -1;

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
    vEndSDL(pFont, pRenderer, pWindow);
    return -1;
  }

  pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
  if ( !pRenderer ) {
    vTraceError("Erro ao criar renderer: %s\n", SDL_GetError());
    vEndSDL(pFont, pRenderer, pWindow);
    return -1;
  }
  
  pFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  if ( !pFont ) {
    vTraceError("Erro ao carregar fonte: %s\n", TTF_GetError());
    vEndSDL(pFont, pRenderer, pWindow);
    return -1;
  }
  gbTtfStarted = TRUE;
  
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
    vRenderer(pRenderer, pFont, pBoard);
  }
  
  vEndSDL(pFont, pRenderer, pWindow);

  vTraceEnd();

  return 0;
}
