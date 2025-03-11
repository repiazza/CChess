#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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

typedef enum ENUM_PIECE_IMAGE_INDEX {
  PAWN_PIECE_INDEX,
  ROOK_PIECE_INDEX,
  KNIGHT_PIECE_INDEX,
  BISHOP_PIECE_INDEX,
  QUEEN_PIECE_INDEX,
  KING_PIECE_INDEX,
  NULL_PIECE_INDEX
} ENUM_PIECE_IMAGE_INDEX, *PENUM_PIECE_IMAGE_INDEX;

#ifdef LINUX
const char *gkpaszWhitePiecesImagePath[] = {
  "img/w_pawn_1x.png",
  "img/w_rook_1x.png",
  "img/w_knight_1x.png",
  "img/w_bishop_1x.png",
  "img/w_queen_1x.png",
  "img/w_king_1x.png",
  NULL
};

const char *gkpaszBlackPiecesImagePath[] = {
  "img/b_pawn_1x.png",
  "img/b_rook_1x.png",
  "img/b_knight_1x.png",
  "img/b_bishop_1x.png",
  "img/b_queen_1x.png",
  "img/b_king_1x.png",
  NULL
};
#endif

#ifdef _WIN32
const char *gkpaszWhitePiecesImagePath[] = {
  "img\\w_pawn_1x.png",
  "img\\w_rook_1x.png",
  "img\\w_knight_1x.png",
  "img\\w_bishop_1x.png",
  "img\\w_queen_1x.png",
  "img\\w_king_1x.png",
  NULL
};

const char *gkpaszBlackPiecesImagePath[] = {
  "img\\b_pawn_1x.png",
  "img\\b_rook_1x.png",
  "img\\b_knight_1x.png",
  "img\\b_bishop_1x.png",
  "img\\b_queen_1x.png",
  "img\\b_king_1x.png",
  NULL
};
#endif

/* Nome do programa */
const char *gkpszProgramName;

/* bacagine - 05/03/2025 - Vamos renderizar a tela apenas quando algum evento ocorrer */
int gbRenderer = TRUE;

char *gpszCurrentWhiteOpegning = NULL;
char *gpszCurrentBlackOpegning = NULL;

// Tempo em segundos
int giWhiteClock = 600;
int giBlackClock = 600;
SDL_mutex *gstClockMutex;

int gbRunning = TRUE;

/**
 * Renderiza texto na tela.
 */
void vRenderText(SDL_Renderer *pRenderer, const char *pszText, int x, int y, SDL_Color color, const char *kpszFontPath, const int kiFontSize) {
  SDL_Surface *pSurface = NULL;
  SDL_Texture *pTexture = NULL;
  SDL_Rect rect;
  TTF_Font *pFont = NULL;
  
  if ( !pszText ) {
    vTraceError("Erro: Texto inválido. Texto: %s\n", pszText ? pszText : "NULL");
    return;
  }
  
  if ( !bOpenFont(&pFont, kpszFontPath, kiFontSize) ) {
    vTraceError("Erro ao abrir a fonte [%s]: %s", kpszFontPath, TTF_GetError());
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
  
  vCloseFont(&pFont);
}

void vRenderImage(SDL_Renderer *pRenderer, const char *kpszImagePath, SDL_Rect stRect) {
  SDL_Texture *pstTexture = NULL;
  SDL_Surface *pstSurface = NULL;
  int iTextureWidth = 0;
  int iTextureHeight = 0;
  
  pstSurface = IMG_Load(kpszImagePath);
  if ( !pstSurface ) {
    vTraceError("Erro ao carregar a imagem [%s]: %s", kpszImagePath, IMG_GetError());
    return;
  }
  pstTexture = SDL_CreateTextureFromSurface(pRenderer, pstSurface);
  SDL_FreeSurface(pstSurface);
  if ( !pstTexture ) {
    vTraceError("Erro ao criar textura para a imagem [%s]: %s", kpszImagePath, IMG_GetError());
    SDL_DestroyTexture(pstTexture);
    return;
  }
  SDL_QueryTexture(pstTexture, NULL, NULL, &iTextureWidth, &iTextureHeight);
  SDL_RenderCopy(pRenderer, pstTexture, NULL, &stRect);
  SDL_DestroyTexture(pstTexture);
}

ENUM_PIECE_IMAGE_INDEX eGetPieceIndexFromPieceName(const char *kpszPieceName) {
  ENUM_PIECE_IMAGE_INDEX ePieceIndex = NULL_PIECE_INDEX;
  if ( strcmp(kpszPieceName, SQUARE_TYPE_PAWN_PIECE) == 0 )
    ePieceIndex = PAWN_PIECE_INDEX;
  if ( strcmp(kpszPieceName, SQUARE_TYPE_ROOK_PIECE) == 0 )
    ePieceIndex = ROOK_PIECE_INDEX;
  if ( strcmp(kpszPieceName, SQUARE_TYPE_KNIGHT_PIECE) == 0 )
    ePieceIndex = KNIGHT_PIECE_INDEX;
  if ( strcmp(kpszPieceName, SQUARE_TYPE_BISHOP_PIECE) == 0 )
    ePieceIndex = BISHOP_PIECE_INDEX;
  if ( strcmp(kpszPieceName, SQUARE_TYPE_QUEEN_PIECE) == 0 )
    ePieceIndex = QUEEN_PIECE_INDEX;
  if ( strcmp(kpszPieceName, SQUARE_TYPE_KING_PIECE) == 0 )
    ePieceIndex = KING_PIECE_INDEX;
  return ePieceIndex;
}

void vDrawPieces(SDL_Renderer *pRenderer, PSTRUCT_SQUARE pstBoard, const char *pszPieceName, int iRow, int iCol, const char *kpszFontPath, const int kiFontSize) {
  SDL_Color SDL_COLOR_PieceText = (pstBoard->ui8Side == FRIENDLY_SIDE)
                                      ? (SDL_Color){255, 255, 255, 255}
                                      : (SDL_Color){0, 0, 0, 255};
  SDL_Rect stRect;
  // Text x y
  int iX = (iCol * SQUARE_SIZE + SQUARE_SIZE / 4);
  int iY = ((ROW_SQUARE_COUNT - 1 - iRow) * SQUARE_SIZE + SQUARE_SIZE / 4)+50;
  ENUM_PIECE_IMAGE_INDEX ePieceIndex = NULL_PIECE_INDEX;
  
  vRenderText(
    pRenderer,
    pszPieceName,
    iX,
    iY,
    SDL_COLOR_PieceText,
    kpszFontPath,
    kiFontSize
  );
  
  // Image x y
  iX = (iCol * SQUARE_SIZE + SQUARE_SIZE / 4)-20;
  iY = ((ROW_SQUARE_COUNT - 1 - iRow) * SQUARE_SIZE + SQUARE_SIZE / 4)+30;
  iRECT_SetDimensions(&stRect, (SDL_Rect){iX, iY, SQUARE_SIZE, SQUARE_SIZE});
  
  if ( (ePieceIndex = eGetPieceIndexFromPieceName(pstBoard->pszType)) != NULL_PIECE_INDEX ) {
    if ( pstBoard->ui8Side == FRIENDLY_SIDE ) {
      vRenderImage(
        pRenderer,
        gkpaszWhitePiecesImagePath[ePieceIndex],
        stRect
      );
    }
    else {
      vRenderImage(
        pRenderer,
        gkpaszBlackPiecesImagePath[ePieceIndex],
        stRect
      );
    }
  }
}

/**
 * Renderiza o tabuleiro e as peças.
 */
void vDrawBoard(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
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
        vDrawPieces(pRenderer, &pBoard[iRow][iCol], pszPieceName, iRow, iCol, kpszFontPath, kiFontSize);
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
  
  if ( !IMG_Init(IMG_INIT_PNG) ) {
    printf("Erro ao inicializar SDL_image: %s\n", IMG_GetError());
    return FALSE;
  }
  
  return TRUE;
}

void vEndSDL(SDL_Thread *pstThread, SDL_Renderer *pRenderer, SDL_Window *pWindow) {
  if ( pstThread ) {
    SDL_WaitThread(pstThread, NULL);
    SDL_DestroyMutex(gstClockMutex);
  }
  if ( pRenderer )
    SDL_DestroyRenderer(pRenderer);
  if ( pWindow )
    SDL_DestroyWindow(pWindow);
  IMG_Quit();
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

void vDrawWhiteOpenings(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  char szBoard[124] = "";
  char szText[512] = "";
  char *pszOpeningName = NULL;
  SDL_Color stBlackColor = { 0, 0, 0, 255 };
  int iX = 10;
  int iY = 700;
  TTF_Font *pFont = NULL;
  
  memset(szBoard, 0x00, sizeof(szBoard));
  memset(szText , 0x00, sizeof(szText ));
  
  if ( !bOpenFont(&pFont, kpszFontPath, kiFontSize) ) {
    vTraceError("Erro ao abrir a fonte [%s]: %s", kpszFontPath, TTF_GetError());
    return;
  }
  
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
  
  vCloseFont(&pFont);
  
  vRenderText(
    pRenderer,
    szText,
    iX,
    iY,
    stBlackColor,
    kpszFontPath,
    kiFontSize
  );
}

void vDrawBlackOpenings(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
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
    kpszFontPath,
    kiFontSize
  );
}

void vDrawOpenings(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  vDrawWhiteOpenings(pRenderer, kpszFontPath, kiFontSize, pBoard);
  vDrawBlackOpenings(pRenderer, kpszFontPath, kiFontSize, pBoard);
}

int iClockThread(void *vpArg) {
  UNUSED(vpArg);
  while ( gbRunning ) {
    SDL_Delay(1000);
    
    SDL_LockMutex(gstClockMutex);
    
    if ( giCurrentTurn == FRIENDLY_SIDE ) {
      if ( giWhiteClock > 0 ) giWhiteClock--;
    }
    else {
      if ( giBlackClock > 0 ) giBlackClock--;
    }
    
    SDL_UnlockMutex(gstClockMutex);
  }
  return 0;
}

void vDrawWhiteClock(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize) {
  SDL_Rect stWhiteClockRect = { 520, 695, 100, 40 };
  SDL_Color stBlackColor = { 0, 0, 0, 255 };
  int iMinutes = giWhiteClock / 60;
  int iSeconds = giWhiteClock % 60;
  char szTime[32] = "";
  
  memset(szTime, 0x00, sizeof(szTime));
  
  SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
  SDL_RenderFillRect(pRenderer, &stWhiteClockRect);
  
  snprintf(
    szTime,
    sizeof(szTime),
    "%02d:%02d",
    iMinutes,
    iSeconds
  );
  
  vRenderText(
    pRenderer,
    szTime,
    540,
    705,
    stBlackColor,
    kpszFontPath,
    kiFontSize
  );
}

void vDrawBlackClock(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize) {
  SDL_Rect stBlacClockRect = { 520, 5, 100, 40 };
  SDL_Color stBlackColor = { 0, 0, 0, 255 };
  int iMinutes = giBlackClock / 60;
  int iSeconds = giBlackClock % 60;
  char szTime[32] = "";
  
  memset(szTime, 0x00, sizeof(szTime));
  
  SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
  SDL_RenderFillRect(pRenderer, &stBlacClockRect);
  
  snprintf(
    szTime,
    sizeof(szTime),
    "%02d:%02d",
    iMinutes,
    iSeconds
  );
  
  vRenderText(
    pRenderer,
    szTime,
    540,
    15,
    stBlackColor,
    kpszFontPath,
    kiFontSize
  );
}

void vDrawClocks(SDL_Renderer *pRenderer, const char *kpszFontPath, const int kiFontSize) {
  vDrawWhiteClock(pRenderer, kpszFontPath, kiFontSize);
  vDrawBlackClock(pRenderer, kpszFontPath, kiFontSize);
}

void vRenderer(SDL_Renderer *pRenderer, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  if ( gbRenderer /*|| atoi(gstCmdLine.szClock)*/ ) {
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);
    
    vDrawHeaderRect(pRenderer);
    vDrawBoard(pRenderer, FONT_PATH, FONT_SIZE, pBoard);
    vDrawFooterRect(pRenderer);
    
    vDrawOpenings(pRenderer, FONT_PATH, 18, pBoard);
    // if ( atoi(gstCmdLine.szClock) ) vDrawClocks(pRenderer, FONT_PATH, 18);
    
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
  PSTRUCT_BOARD_HISTORY pstHistory = NULL;
  SDL_Event event;
  SDL_Window *pWindow = NULL;
  SDL_Renderer *pRenderer = NULL;
  STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT];
  SDL_Thread *pstThread = NULL;
  
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
    vEndSDL(pstThread, pRenderer, pWindow);
    return -1;
  }

  pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
  if ( !pRenderer ) {
    vTraceError("Erro ao criar renderer: %s\n", SDL_GetError());
    vEndSDL(pstThread, pRenderer, pWindow);
    return -1;
  }
  
  vInitializeBoard(pBoard);
  pstHistory = pstCreateHistory();
  
  pstThread = SDL_CreateThread(iClockThread, "ClockThread", NULL);
  if ( !pstThread ) {
    vTraceError("Erro ao criar thread: %s\n", SDL_GetError());
    vEndSDL(pstThread, pRenderer, pWindow);
    return -1;
  }
  
  // main loop
  while ( gbRunning ) {
    while ( SDL_PollEvent(&event) ) {
      if ( event.type == SDL_QUIT ) {
        gbRunning = FALSE;
      }
      else if ( event.type == SDL_MOUSEBUTTONDOWN ) {
        vHandleMouseClickEvent(&event, pBoard);
      }
    }
    vRenderer(pRenderer, pBoard);
  }
  
  vEndSDL(pstThread, pRenderer, pWindow);

  vTraceEnd();

  return 0;
}
