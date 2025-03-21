/**
 * trace.c
 * 
 * Written by Renato Fermi <repiazza@gmail.com> in January 2025
 * 
 * Description: Chess written in C
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>
#include <consts.h>
#include <cmdline.h>
#include <util.h>

#if defined(LINUX) || defined(APPLE)
  #include <sys/time.h>
  #include <sys/types.h>  
  #include <unistd.h>  
#endif /* LINUX */

STRUCT_TRACE_PRM gstTracePrm;
char gszConfFile[_MAX_PATH];

int giNoNL = FALSE;

void vSetDebugLevel(void) {
  if( !bStrIsEmpty( gstCmdLine.szDebugLevel ) ) {
    gstTracePrm.iDebugLevel = atoi( gstCmdLine.szDebugLevel );
  }
  else {
    gstTracePrm.iDebugLevel = iGetDebugLevel( gszConfFile );
  }
} /* vSetDebugLevel */

// Função para obter o timestamp atual
void vGetCurrentTimestamp(char *szTimestamp, size_t size) {
  struct tm *st_tm_Now;
  struct timeval tv;
  time_t lTime;

  time(&lTime);
  st_tm_Now = localtime(&lTime);
  gettimeofday(&tv, NULL);

  snprintf(szTimestamp, size,
    "[%02d/%02d/%04d %02d:%02d:%02d.%03ld] ",
    (int) st_tm_Now->tm_mday, 
    (int) st_tm_Now->tm_mon + 1, 
    (int) st_tm_Now->tm_year + 1900,
    (int) st_tm_Now->tm_hour,
    (int) st_tm_Now->tm_min,
    (int) st_tm_Now->tm_sec,
    (long) tv.tv_usec / 1000
  );
}

// Função genérica para inicializar configuração de trace
void vInitTraceConfig(void) {
  memset(gstTracePrm.szTraceFile, 0, sizeof(gstTracePrm.szTraceFile));
  memset(gszConfFile, 0, sizeof(gszConfFile));
  
  vSetConfFile();
  vSetLogFile();
  vSetDebugLevel();
}

void vTraceMsgNoNL(char *szMsg) {
  giNoNL = TRUE;
  vTraceMsg(szMsg);
  giNoNL = FALSE;
}

void vSetConfFile( void ) {
#ifdef _WIN32
  int ii = 0;
#endif /* _WIN32 */

  /**
   * bacagine - 2023-11-10 - Setting the name of the configuration file
   */

  if ( !bStrIsEmpty( gstCmdLine.szConfFile ) ) {
    snprintf( gszConfFile, sizeof( gszConfFile ), "%s", gstCmdLine.szConfFile );
    return;
  }
#if defined(LINUX) || defined(APPLE)
  else if ( bFileExist("/etc/cchess.conf") ) {
    snprintf( gszConfFile, sizeof( gszConfFile ), "/etc/cchess.conf" );
    return;
  }
  else if ( bFileExist("~/.cchess.conf") ) {
    snprintf( gszConfFile, sizeof( gszConfFile ), "~/.cchess.conf" );
    return;
  }
  else {
    snprintf( gszConfFile, sizeof( gszConfFile ), "%s.conf", gkpszProgramName );
    return;
  }
#else
  else {
    while ( gkpszProgramName[ii] != '.' ) {
      gszConfFile[ii] = gkpszProgramName[ii];
      ii++;
    }
  }
#endif /* LINUX */
  if ( !bStrIsEmpty(gszConfFile) && !strstr(gszConfFile, "prm") ) {
    char szWrk[_MAX_PATH];
    memset(szWrk, 0x00, sizeof(szWrk));
    strcpy(szWrk, "prm/");
    strcat(szWrk, gszConfFile);
    memset(gszConfFile, 0x00, sizeof(gszConfFile));
    strcpy(gszConfFile, szWrk);
    strcat(gszConfFile, ".conf");
  }
} /* vSetConfFile */

void vTraceErr(const char *kpszModuleName, const int kiLine, const char *kpszFmt, ...) {
  va_list args;
  FILE *pfLog = NULL;
  char szTimestamp[128] = {0};
  char szDbg[2048] = {0};

  vGetCurrentTimestamp(szTimestamp, sizeof(szTimestamp));

  snprintf(szDbg, sizeof(szDbg), "%s[ERROR] %s:%d ", szTimestamp, kpszModuleName, kiLine);

  if ((pfLog = fopen(gstTracePrm.szTraceFile, "a+")) == NULL)
      return;

  va_start(args, kpszFmt);
  vfprintf(pfLog, strcat(szDbg, kpszFmt), args);
  va_end(args);

  fprintf(pfLog, "\n");
  fclose(pfLog);
}

void vTraceMessage(const char *kpszModuleName, const int kiLine, const char *kpszFmt, ...) {
  va_list args;
  FILE *pfLog = NULL;
  char szTimestamp[128] = {0};
  char szDbg[2048] = {0};

  vGetCurrentTimestamp(szTimestamp, sizeof(szTimestamp));

  snprintf(szDbg, sizeof(szDbg), "%s %s:%d ", szTimestamp, kpszModuleName, kiLine);

  if ((pfLog = fopen(gstTracePrm.szTraceFile, "a+")) == NULL)
      return;

  va_start(args, kpszFmt);
  vfprintf(pfLog, strcat(szDbg, kpszFmt), args);
  va_end(args);

  fprintf(pfLog, "\n");
  fclose(pfLog);
}

void _vTraceVarArgs(const char *kpszModuleName, const int kiLine, const char *kpszFmt, ...) {
  va_list args;
  FILE *pfLog = NULL;
  char szTimestamp[128] = {0};
  char szDbg[2048] = {0};

  vGetCurrentTimestamp(szTimestamp, sizeof(szTimestamp));

  snprintf(szDbg, sizeof(szDbg), "%s%s:%d ", szTimestamp, kpszModuleName, kiLine);

  if ((pfLog = fopen(gstTracePrm.szTraceFile, "a+")) == NULL)
      return;

  va_start(args, kpszFmt);
  vfprintf(pfLog, strcat(szDbg, kpszFmt), args);
  va_end(args);

  fprintf(pfLog, "\n");
  fclose(pfLog);
}

void vTraceCommandLine(int argc, char **argv) {
  int ii = 0;
  vTraceVarArgs("argc [%d]", argc);
  for ( ii = 0; ii < argc; ii++ ) {
    vTraceVarArgs("argv %d: [%s]", ii, argv[ii]);
  }
}

void vTraceEnvp(char **envp) {
  int ii = 0;
  for ( ii = 0; envp[ii]; ii++ ) {
    vTraceVarArgs("envp %d: [%s]", ii, envp[ii]);
  }
}

void vTraceApple(char **apple) {
  int ii = 0;
  for ( ii = 0; apple[ii]; ii++ ) {
    vTraceVarArgs("apple %d: [%s]", ii, apple[ii]);
  }
}

void vTraceBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]) {
  char szDbg[256] = "";
  memset(szDbg, 0x00, sizeof(szDbg));
  vConvertBoard2String(szDbg, sizeof(szDbg), pBoard);
  vTraceMsg(szDbg);
}

void vTraceBoardRowCol(const char *kpszMsg, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol) {
  vTraceVarArgs(
    "\n=================%s================\n"
    "TYPE.......: [%s]\n"
    "COLOR......: [%d]\n"
    "SIDE.......: [%d]\n"
    "Highlighted: [%d]\n"
    "SELECTED...: [%d]\n"
    "TRHEAT.....: [%s]\n"
    "===================================\n",
    bStrIsEmpty(kpszMsg) ? "=" : kpszMsg,
    pBoard[iRow][iCol].pszType,
    pBoard[iRow][iCol].ui8Color,
    pBoard[iRow][iCol].ui8Side,
    pBoard[iRow][iCol].bHighlighted,
    pBoard[iRow][iCol].bSelected,
    gkpszThreatType[pBoard[iRow][iCol].ui8Threat]
  );
}

