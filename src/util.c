/**
 * util.c
 *
 * Written by Gustavo Bacagine <gustavo.bacagine@protonmail.com>
 *  
 * My util file
 *
 * Date: 10/11/2023
 */

#include <util.h>

char *szTokenName[] = {
  "TRACE_FILE",
  "DEBUG_LEVEL",
  "WINDOW_RESOLUTION",
  NULL
};

/******************************************************************************
 *                                                                            *
 *                               FILE FUNCTIONS                               *
 *                                                                            *
 ******************************************************************************/

#ifdef _WIN32
  // Return 0 if error
  int iDIR_MkDir(char *szDir)
  {
    return CreateDirectory(szDir, NULL);
  }
  int iDIR_SplitFilename(char* szFilename, char* szPath, char* szName, char* szExt)
  {
    char szDrive[_MAX_DRIVE];
    char szDir[_MAX_DIR];
    _splitpath(
      szFilename,
      szDrive,
      szDir,
      szName,
      szExt
      );
    strcpy(szPath, szDrive);
    strcat(szPath, szDir);
    return 0;
  }
  int iDIR_IsDir(char* szDir)
  {
    HANDLE hArquivo;
    WIN32_FIND_DATA wfdArquivo;
    hArquivo = FindFirstFile(szDir, &wfdArquivo);
    if ( hArquivo == INVALID_HANDLE_VALUE )
      return 0;
    FindClose(hArquivo);
    if ( wfdArquivo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      return 1;
    return 0;
  }
#else
  /*
   * Is directory: 1
   * Exists, not directory: 0
   * Does not exist: -1
   * **/ 
  int iDIR_IsDir(char* szDir)
  {
    struct stat stStat;
    if ( stat(szDir, &stStat) != 0 )
      return 0;
    if ( S_ISDIR(stStat.st_mode) )
      return 1;
    return 0;
  }
  int iDIR_SplitFilename(char* szFilename, char* szPath, char* szName, char* szExt)
  {
    char szWrk[_MAX_PATH];
    char szBase[_MAX_PATH];
    char* pszBase;
    int ii;
    strcpy(szWrk, szFilename);
    pszBase = dirname(szWrk);
    if ( pszBase == NULL ) {
      szPath[0] = 0;
    }
    else {
      strcpy(szPath, pszBase);
    }
    strcpy(szWrk, szFilename);
    pszBase = basename(szWrk);
    strcpy(szBase, pszBase);
    for ( ii = strlen(szBase)-1;  ii && szBase[ii] != '.';  ii-- ) {
      if ( szBase[ii] == '\n' || szBase[ii] == '\r' )
        szBase[ii] = 0;
    }
    if ( ii > 0 ) { // found dot
      strcpy(szExt, &szBase[ii]);
      szBase[ii] = 0;
      strcpy(szName, szBase);
    }
    else {
      strcpy(szName, szBase);
      *szExt = 0;
    }
    if ( DEBUG_MORE_MSGS ) {
      char szDbg[1024];
      sprintf(szDbg, "iDIR_SplitFilename('%s', ...) szPath=[%s] szName=[%s] szExt=[%s]",
        szFilename,
        szPath,
        szName,
        szExt
      );
      vTraceMsg(szDbg);
    }
    return 0;
  }
    // Return 0 if error
  int iDIR_MkDir(char *szDir) //linux
  {
    int iRsl;
    if ( DEBUG_MSGS )
      vTraceVarArgs("iDIR_MkDir(%s)", szDir);
    iRsl = mkdir(szDir,
      S_IRUSR | S_IWUSR | S_IXUSR |
      S_IRGRP | S_IWGRP | S_IXGRP |
      S_IROTH
    );
    return (iRsl == 0);
  }

#endif

int bOpenFile( FILE **fppFile, const char *kpszFileName, const char *kpszMode ) {
  if ( ( *fppFile = fopen( kpszFileName, kpszMode ) ) == NULL ) {
    return FALSE;
  }

  return TRUE;
} /* bOpenFile */

int bCloseFile( FILE **fppFile ) {
  if ( *fppFile != NULL ) {
    fclose( *fppFile );
    *fppFile = NULL;

    return TRUE;
  }

  return FALSE;
} /* bCloseFile */

int bFileExist(const char *kpszFileName) {
  FILE *fpFile = NULL;
  
  if ( !bOpenFile( &fpFile, kpszFileName, "r" ) ) {
    return FALSE;
  }
  
  bCloseFile( &fpFile );

  return TRUE;
} /* bFileExist */

/******************************************************************************
 *                                                                            *
 *                              STRING FUNCTIONS                              *
 *                                                                            *
 ******************************************************************************/
int bStrIsEmpty( const char *kpszStr ) {
  if (kpszStr == NULL || !strcmp( kpszStr, "" ) || !strcmp( kpszStr, "\n" ) ) {
    return TRUE;
  }

  return FALSE;
} /* bStrIsEmpty */

/******************************************************************************
 *                                                                            *
 *                              Chess FUNCTIONS                              *
 *                                                                            *
 ******************************************************************************/

int iValidToken( char *pTokSearch ) {
  ENUM_CCHESS_PRM eCChessPrm = 0;

  while ( eCChessPrm != END_PRM ) {
    if ( memcmp( pTokSearch, szTokenName[eCChessPrm], strlen( szTokenName[eCChessPrm] ) ) == 0 ) {
      return eCChessPrm;
    }
    
    eCChessPrm++;
  } /* while */
  
  return TOKEN_MISMATCH;
} /* iValidToken */

int iParseCfgLine( char *pszLineContents ) {
  char *pTok     = NULL;
  char *pCh      = NULL;
  char *pToken   = NULL;
  char *pDestVar = NULL;
  int iTokenRsl  =    0;
  
  if ( ( pTok = strtok( pszLineContents, "\n" ) ) == NULL ) return 0;
  
  pCh = pTok;
  
  if ( *pCh == '#' ) return 0;
  
  /**
   * Ignore spaces
   */
  while ( isspace( *pCh ) ) {
    if ( *pCh == EOF ) return 0;

    pCh++;
  } /* while */

  if ( bStrIsEmpty( pCh ) ) return 0;
  
  switch ( ( iTokenRsl = iValidToken( pCh ) ) ) {
    case TRACE_FILE       : 
        memset( gstCmdLine.szTraceFile, 0x00, sizeof( gstCmdLine.szTraceFile ));
        pDestVar = gstCmdLine.szTraceFile ;
        break;
    case DEBUG_LEVEL      : 
        memset( gstCmdLine.szDebugLevel, 0x00, sizeof( gstCmdLine.szDebugLevel ));
        pDestVar = gstCmdLine.szDebugLevel; 
        break;
    case WINDOW_RESOLUTION: 
        memset( gstCmdLine.szWinWidth, 0x00, sizeof( gstCmdLine.szWinWidth ));
        memset( gstCmdLine.szWinHeight, 0x00, sizeof( gstCmdLine.szWinHeight ));
        break;
    default               : return 0;
  } /* switch */
  
  pToken = strchr( pCh, '=' );
  pToken++;
  
  /**
   * Ignore spaces
   */
  while ( isspace( *pToken ) ) pToken++;

  if ( bStrIsEmpty( pToken ) ) return 0;
  
  if ( iTokenRsl == WINDOW_RESOLUTION ) {
    strcpy( gstCmdLine.szWinWidth, strtok( pToken, "x" ) );
    strcpy( gstCmdLine.szWinHeight, strtok (NULL, "\n" ) );
  }
  else {
    snprintf( &(*pDestVar), strlen( pToken ) + 1, "%s", pToken );
  }

  return 0;
} /* iParseCfgLine */

int iGetDebugLevel( const char *kpszConfFile ) {
  FILE *fpConfFile = NULL;
  int iDebugLevel = 0;
  char szLine[256];
  int iLineLen = sizeof(szLine);
  int bFoundDebugLevel = FALSE;
  char *pTok = NULL;
  int ii = 0;
  char szLevel[2] = "";

  memset( szLine , 0, iLineLen          );
  memset( szLevel, 0, sizeof( szLevel ) );
  
  if ( ( fpConfFile = fopen( kpszConfFile, "r" ) ) == NULL ) {
    fprintf( stderr, "E: %s %s", kpszConfFile, strerror( errno ) );

    return 0;
  }
  
  /**
   * bacagine - 2023-11-10 - Reading the .conf file
   */
  while ( fgets( szLine, iLineLen, fpConfFile ) != NULL ) {
    /**
     * Ignore commented lines
     */
    if ( strstr( szLine, "#" ) ) continue;

    pTok = strtok(szLine, " = ");

    if ( strstr(szLine, "DEBUG_LEVEL" ) ) {
      bFoundDebugLevel = TRUE;
      
      /**
       * bacagine - 2023-11-10 - Getting the value after '='
       * symbom and converting form string to char.
       */
      while ( pTok != NULL ) {
        if ( ii == 1 ) {
          /**
           * bacagine - 2023-11-10 - Verify if have value after '=' symbol.
           */
          if ( bStrIsEmpty( pTok ) ) {
            fprintf(stderr, "E: DEBUG_LEVEL is empty in file %s!\n", kpszConfFile);
              
            fclose( fpConfFile );
            fpConfFile = NULL;
            
            return 0;
          }

          snprintf( szLevel, sizeof( szLevel ), "%s", pTok );
          iDebugLevel = atoi( szLevel );

          break;
        }

        pTok = strtok( NULL, " = " );

        ii++;
      } /* while pTok */

      break;
    }
    else {
      continue;
    }
  } /* while fgets */
  
  /**
   * If don't found the DEBUG_LEVEL 
   * variable in file print a error 
   * message to user
   */
  if ( bFoundDebugLevel == FALSE ) {
    fprintf(stderr, "E: Not found variable DEBUG_LEVEL in file %s!\n", kpszConfFile);
        
    fclose( fpConfFile );
    fpConfFile = NULL;

    return 0;
  }
  
  /**
   * If DEBUG_LEVEL in file is incorret
   */
  if ( iDebugLevel < 0 || iDebugLevel > 9 ) {
    fprintf(stderr, "E: Invalid value of log level in file %s!\n", kpszConfFile);
        
    fclose( fpConfFile );
    fpConfFile = NULL;

    return 0;
  }

  fclose( fpConfFile );
  fpConfFile = NULL;

  return iDebugLevel;
} /* iGetDebugLevel */

void vSetLogFile( void ) {
#ifdef _WIN32
  int ii = 0;
#endif /* _WIN32 */

  if(!bStrIsEmpty( gstCmdLine.szTraceFile ) ) {
    snprintf( gstTracePrm.szTraceFile, (size_t) sizeof( gstTracePrm.szTraceFile ) - 8, "%s", gstCmdLine.szTraceFile );
    return;
  }
#ifdef LINUX
  else {
    snprintf( gstTracePrm.szTraceFile, sizeof( gstTracePrm.szTraceFile ), "%s.log", gkpszProgramName );
    return;
  }
#else
  else {
    while ( gkpszProgramName[ii] != '.' ) {
      gstTracePrm.szTraceFile[ii] = gkpszProgramName[ii];
      ii++;
    }
    strcat( gstTracePrm.szTraceFile, ".log" );
  }
#endif /* LINUX */
} /* vSetLogFile */

int iCheckCfgPrm( void ) {
  if ( bStrIsEmpty( gstCmdLine.szTraceFile ) ) {
    return -1;
  }

  if ( bStrIsEmpty( gstCmdLine.szDebugLevel ) ) {
    strcpy( gstCmdLine.szDebugLevel, "0" );
  }

  if ( bStrIsEmpty( gstCmdLine.szWinHeight ) ) {
    strcpy ( gstCmdLine.szWinHeight, "800" );
  }

  if ( bStrIsEmpty( gstCmdLine.szWinWidth ) ) {
    strcpy(gstCmdLine.szWinWidth, "800");
  }

  return 0;
} /* iCheckCfgPrm */

/**
 * Load the Chess's parameters file.
 */
int bLoadCfgFile( const char *kpszFileName ) {
  FILE *fpFile = NULL;
  char szLine[2048];
  char szFullPath[256];

  memset( szLine, 0x00, sizeof( szLine ) );
  memset( szFullPath, 0x00, sizeof( szFullPath ) );

  sprintf(szFullPath, "%s/%s", ROOT_PATH_FROM_BIN, kpszFileName);
  
  if(!bOpenFile( &fpFile, kpszFileName, "r" ) ) {
    return FALSE;
  }
  
  while ( fgets( szLine, sizeof( szLine ), fpFile ) ) {
    if ( iParseCfgLine( szLine ) == -1 ) {
      fprintf(stderr, "E: impossible parse .conf file\n");
      
      exit(EXIT_FAILURE);
    }

    memset( szLine, 0x00, sizeof( szLine ) );
  } // while

  if ( iCheckCfgPrm() == -1 ) {
    fprintf(stderr, "E: Impossible check .conf parameters!\n");

    exit(EXIT_FAILURE);
  }

  if ( !bCloseFile( &fpFile ) ) {
    return FALSE;
  }

  return TRUE;
} /* bLoadCfgFile */

int bTerminalSupportColors( void ) {
  char *szTerm = getenv("TERM");

  if (bStrIsEmpty( szTerm ) || !strcmp( szTerm, "dumb" ) ) {
    return FALSE;
  }

  return TRUE;
} /* bTerminalSupportColors */

void vPrintWarningMessage( const char* kpszFmt, ... ) {
  va_list args;
  char *pszMsg = NULL;
  int iReallocBuffer = 0;

  va_start(args, kpszFmt);

  /**
   * Check if the terminal suport colors
   */
  if ( bTerminalSupportColors() == FALSE ) {
    iReallocBuffer = snprintf(NULL, 0, "W: %s\n", kpszFmt);

    pszMsg = ( char * ) malloc( sizeof( char ) * ( iReallocBuffer + 1 ) );

    snprintf( pszMsg, iReallocBuffer + 1, "W: %s\n", kpszFmt );

    vfprintf( stderr, pszMsg, args );

    free( pszMsg );
    pszMsg = NULL;

    va_end( args );

    return;
  }

  iReallocBuffer = snprintf( NULL, 0, "\033[1;35mW:\033[m %s\n", kpszFmt );

  pszMsg = (char *) malloc(sizeof(char) * (iReallocBuffer+1));

  snprintf( pszMsg, iReallocBuffer + 1, "\033[1;35mW:\033[m %s\n", kpszFmt );

  vfprintf( stderr, pszMsg, args );

  free( pszMsg );
  pszMsg = NULL;

  va_end( args );
} /* vPrintWarningMessage */

void vRemoveChar(char *pszStr, size_t lStrSize, char chCh) {
  char szAux[lStrSize];
  int ii = 0;
  int jj = 0;
  memset(szAux, 0x00, lStrSize);
  for ( ii = 0; pszStr[ii]; ii++ ) {
    if ( pszStr[ii] != chCh ) {
      szAux[jj] = pszStr[ii];
      jj++;
    }
  }
  szAux[jj] = '\0';
  snprintf(pszStr, lStrSize, "%s", szAux);
}

void vInvertStr(char *pszStr) {
  size_t lStrSize = strlen(pszStr);
  char szAux[lStrSize+1];
  int ii = 0;
  int jj = 0;
  memset(szAux, 0x00, sizeof(szAux));
  for ( ii = lStrSize-1; ii >= 0; ii-- ) {
    szAux[jj] = pszStr[ii];
    jj++;
  }
  szAux[jj] = '\0';
  snprintf(pszStr, lStrSize+1, "%s", szAux);
}
