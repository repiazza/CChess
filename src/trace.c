#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>
#include <consts.h>
#include <cmdline.h>
#include <util.h>

#ifdef LINUX
  #include <sys/time.h>
  #include <sys/types.h>  
  #include <unistd.h>  
#endif /* LINUX */

char gszTraceFile[_MAX_PATH+_MAX_PATH];
int giDebugLevel = 0;
char gszConfFile[_MAX_PATH];

int giNoNL = FALSE;

void vSetDebugLevel( void ) {
  if( !bStrIsEmpty( gstCmdLine.szDebugLevel ) ) {
    giDebugLevel = atoi( gstCmdLine.szDebugLevel );
  }
  else {
    giDebugLevel = iGetDebugLevel( gszConfFile );
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
  memset(gszTraceFile, 0, sizeof(gszTraceFile));
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
#ifdef LINUX
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
  if ( !bStrIsEmpty(gszConfFile) && !strstr(gszConfFile, "prm") ){
    char szWrk[_MAX_PATH];
    memset(szWrk, 0x00, sizeof(szWrk));
    strcpy(szWrk, "prm/");
    strcat(szWrk, gszConfFile);
    memset(gszConfFile, 0x00, sizeof(gszConfFile));
    strcpy(gszConfFile, szWrk);
    strcat(gszConfFile, ".conf");
  }
} /* vSetConfFile */

void vTraceMsg(char *szMsg) {
  FILE *pfLog;
  char szTimestamp[128] = {0};

  vGetCurrentTimestamp(szTimestamp, sizeof(szTimestamp));

  if ((pfLog = fopen(gszTraceFile, "a+")) == NULL)
    return;

  if (giNoNL)
    fprintf(pfLog, "%s", szMsg);
  else 
    fprintf(pfLog, "%s%s\n", szTimestamp, szMsg);

  fclose(pfLog);
}

void _vTraceVarArgs(const char *kpszModuleName, const int kiLine, const char *kpszFmt, ...) {
  va_list args;
  FILE *pfLog = NULL;
  char szTimestamp[128] = {0};
  char szDbg[2048] = {0};

  vGetCurrentTimestamp(szTimestamp, sizeof(szTimestamp));

  snprintf(szDbg, sizeof(szDbg), "%s%s:%d ", szTimestamp, kpszModuleName, kiLine);

  if ((pfLog = fopen(gszTraceFile, "a")) == NULL)
      return;

  va_start(args, kpszFmt);
  vfprintf(pfLog, strcat(szDbg, kpszFmt), args);
  va_end(args);

  fprintf(pfLog, "\n");
  fclose(pfLog);
}

