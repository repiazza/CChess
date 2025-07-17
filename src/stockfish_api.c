/**
 * @file stockfish_api.c
 *
 * @brief Integrates CChess with stockfish
 *
 * @author Gustavo Bacagine <gustavo.bacagine@protonmail.com>
 *
 * @date Jul 2025
 */

#include <stockfish_api.h>

STRUCT_STOCKFISH gstStockfish;

pid_t pidStockfish;
int fdToStockfish[2];
int fdFromStockfish[2];

void vSTOCKFISH_UCI(void) {
  fprintf(stdin, "uci\n");
  fflush(stdin);
}

void vSTOCKFISH_Position(void) {
  STRUCT_MOVE_LIST *mv = NULL;
  char szPositions[2048] = "";
  memset(szPositions, 0x00, sizeof(szPositions));
  for ( mv = gpstMoveList; mv; mv = mv->pstNext ) {
    strcat(szPositions, mv->stMovement.szMovement);
    if ( mv->pstNext )
      strcat(szPositions, " ");
  }
  fprintf(stdin, "position startpos moves %s\n", szPositions);
  fflush(stdin);
}

void vSTOCKFISH_GoMovetime(void) {
  fprintf(stdin, "go movetime %s\n", gstStockfish.szMoveTime);
  fflush(stdin);
}

int bSTOCKFISH_IsReady(void) {
  int bRsl = FALSE;
  fprintf(stdin, "isready\n");
  fflush(stdin);
  fgets(gstStockfish.szBuffer, sizeof(gstStockfish.szBuffer), stdin);
  gstStockfish.szBuffer[strcspn(gstStockfish.szBuffer, "\r\n")] = '\0';
  if ( !strcmp(gstStockfish.szBuffer, "readyok") )
    bRsl = TRUE;
  return bRsl;
}

void vSTOCKFISH_Quit(void) {
  fprintf(stdin, "quit\n");
  fflush(stdin);
}

int bSTOCKFISH_Init(const char *kpszStockfishPath, const char *kpszMoveTime) {
  if ( DEBUG_MSGS ) {
    vTraceBegin();
    vTraceMsg("Starting stockfish API");
    vTraceMsg("API Version [%s]", __STOCKFISH_API_VERSION__);
  }

  memset(&gstStockfish, 0x00, sizeof(gstStockfish));

  if ( !kpszStockfishPath ) return FALSE;

  if ( !kpszMoveTime )
    snprintf(gstStockfish.szMoveTime, sizeof(gstStockfish.szMoveTime), "1000");
  else
    snprintf(gstStockfish.szMoveTime, sizeof(gstStockfish.szMoveTime), "%s", kpszMoveTime);

  if ( DEBUG_MSGS ) {
    vTraceMsg("Stockfish Path: [%s]", kpszStockfishPath);
    vTraceMsg("Move Time: [%s]", gstStockfish.szMoveTime);
  }

  if ( pipe(fdToStockfish) < 0 ) {
    perror("pipe to stockfish failed");
    return FALSE;
  }

  if ( pipe(fdFromStockfish) < 0 ) {
    perror("pipe to stockfish failed");
    return FALSE;
  }

  pidStockfish = fork();
  if ( pidStockfish < 0 ) {
    perror("fork failed");
    return FALSE;
  }

  /* bacagine - 17/07/2025 - TODO: Continuar o desenvolvimento da API a partir desse ponto */

  if ( DEBUG_MSGS ) vTraceEnd();

  return TRUE;
}

int bSTOCKFISH_IsStarted(void) {
  return !(pidStockfish < 0);
}

void vSTOCKFISH_End(void) {
  if ( DEBUG_MSGS ) vTraceBegin();
  vSTOCKFISH_Quit();
  pclose(stdin);
  memset(&gstStockfish, 0x00, sizeof(gstStockfish));
  if ( DEBUG_MSGS ) vTraceEnd();
}

int bSTOCKFISH_GetBestMovement(void) {
  char *pTok = NULL;
  char szBuffer[2048] = "";

  if ( DEBUG_MORE_MSGS ) vTraceBegin();

  memset(szBuffer, 0x00, sizeof(szBuffer));

  if ( !bSTOCKFISH_IsStarted() ) {
    if ( DEBUG_MSGS ) vTraceMsg("E: Stockfish API was not started!");
    return FALSE;
  }

  vSTOCKFISH_UCI();
  if  ( !bSTOCKFISH_IsReady() ) {
    if ( DEBUG_MSGS ) vTraceMsg("E: Stockfish is not ready!");
    return FALSE;
  }

  vSTOCKFISH_Position();
  vSTOCKFISH_GoMovetime();
  while ( fgets(gstStockfish.szBuffer, sizeof(gstStockfish.szBuffer), stdin) ) {
    if ( strstr(gstStockfish.szBuffer, "bestmove") ) {
      break;
    }
  }

  snprintf(szBuffer, sizeof(szBuffer), "%s", gstStockfish.szBuffer);
  pTok = strtok(szBuffer, " ");
  pTok = strtok(NULL, " ");
  if ( pTok )
    snprintf(gstStockfish.szBestMove, sizeof(gstStockfish.szBestMove), "%s", pTok);
  else
    gstStockfish.szBestMove[0] = '\0';

  if ( DEBUG_MORE_MSGS ) vTraceEnd();

  return TRUE;
}
