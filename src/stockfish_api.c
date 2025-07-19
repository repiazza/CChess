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

#ifdef LINUX
pid_t pidStockfish = -1;
int fdToStockfish[2];
int fdFromStockfish[2];
#endif

FILE *fpWrite = NULL;
FILE *fpRead = NULL;

void vSTOCKFISH_UCI(void) {
  if ( !fpWrite || !fpRead ) return;
  fprintf(fpWrite, "uci\n");
  fflush(fpWrite);
  while ( fgets(gstStockfish.szBuffer, sizeof(gstStockfish.szBuffer), fpRead) ) {
    gstStockfish.szBuffer[strcspn(gstStockfish.szBuffer, "\r\n")] = '\0';
    if ( !strcmp(gstStockfish.szBuffer, "uciok") ) break;
  }
}

void vSTOCKFISH_Position(void) {
  STRUCT_MOVE_LIST *mv = NULL;
  char szPositions[2048] = "";
  if ( !fpWrite || !fpRead ) return;
  memset(szPositions, 0x00, sizeof(szPositions));
  for ( mv = gpstMoveList; mv; mv = mv->pstNext ) {
    strcat(szPositions, mv->stMovement.szMovement);
    if ( mv->pstNext )
      strcat(szPositions, " ");
  }
  fprintf(fpWrite, "position startpos moves %s\n", szPositions);
  fflush(fpWrite);
}

void vSTOCKFISH_GoMovetime(void) {
  if ( !fpWrite || !fpRead ) return;
  fprintf(fpWrite, "go movetime %s\n", gstStockfish.szMoveTime);
  fflush(fpWrite);
}

int bSTOCKFISH_IsReady(void) {
  int bRsl = FALSE;
  if ( !fpWrite || !fpRead ) return bRsl;
  fprintf(fpWrite, "isready\n");
  fflush(fpWrite);
  fgets(gstStockfish.szBuffer, sizeof(gstStockfish.szBuffer), fpRead);
  gstStockfish.szBuffer[strcspn(gstStockfish.szBuffer, "\r\n")] = '\0';
  if ( !strcmp(gstStockfish.szBuffer, "readyok") )
    bRsl = TRUE;
  return bRsl;
}

void vSTOCKFISH_Quit(void) {
  if ( !fpWrite || !fpRead ) return;
  fprintf(fpWrite, "quit\n");
  fflush(fpWrite);
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
#ifdef LINUX
  if ( pipe(fdToStockfish) < 0 ) {
    if ( DEBUG_MSGS ) vTraceMsg("pipe to stockfish failed: [%s]", strerror(errno));
    return FALSE;
  }

  if ( pipe(fdFromStockfish) < 0 ) {
    if ( DEBUG_MSGS ) vTraceMsg("pipe to stockfish failed: [%s]", strerror(errno));
    return FALSE;
  }

  pidStockfish = fork();
  if ( pidStockfish < 0 ) {
    if ( DEBUG_MSGS ) vTraceMsg("fork failed: [%s]", strerror(errno));
    return FALSE;
  }

  if ( pidStockfish == 0 ) {
    dup2(fdToStockfish[0], STDIN_FILENO);
    dup2(fdFromStockfish[1], STDOUT_FILENO);

    close(fdToStockfish[0]);
    close(fdToStockfish[1]);
    close(fdFromStockfish[0]);
    close(fdFromStockfish[1]);

    execlp(kpszStockfishPath, kpszStockfishPath, NULL);
    if ( DEBUG_MSGS ) vTraceMsg("exec failed: [%s]", strerror(errno));
    exit(EXIT_FAILURE);
  }
  else {
    close(fdToStockfish[0]);
    close(fdFromStockfish[1]);

    fpWrite = fdopen(fdToStockfish[1], "w");
    fpRead = fdopen(fdFromStockfish[0], "r");

    if ( !fpWrite || !fpRead ) {
      if ( DEBUG_MSGS ) vTraceMsg("fdopen failed: [%s]", strerror(errno));
      return FALSE;
    }
  }
#endif
  gstStockfish.bStockfishOn = TRUE;

  if ( DEBUG_MSGS ) vTraceEnd();

  return TRUE;
}

int bSTOCKFISH_IsStarted(void) {
  return gstStockfish.bStockfishOn;
}

void vSTOCKFISH_End(void) {
  if ( DEBUG_MSGS ) vTraceBegin();

  vSTOCKFISH_Quit();

  if ( fpWrite ) {
    fclose(fpWrite);
    fpWrite = NULL;
  }

  if ( fpRead ) {
    fclose(fpRead);
    fpRead = NULL;
  }
#ifdef LINUX
  if ( pidStockfish > 0 ) {
    int iStatus = 0;
    waitpid(pidStockfish, &iStatus, 0);
    pidStockfish = -1;
  }
#endif
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
  while ( fgets(gstStockfish.szBuffer, sizeof(gstStockfish.szBuffer), fpRead) ) {
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
