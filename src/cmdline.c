/**
 * cmdline.h
 *
 * Written by Gustavo Bacagine <gustavo.bacagine@protonmail.com> 
 *  
 * My cmdline file
 *
 * Date: 21/10/2023
 */

#include <cmdline.h>
#include <trace.h>

/**
 * Long            Short                 Required                   DataType                         Argument
 *  Set             Default               Data                       DataLength
 * Help
 */

STRUCT_CMDLINE astCmdOpt[] = {
/* 00 */
  { "help",        "h",                  CMDDATA_NODATA,            CMDTYPE_NULL,                    "",
     FALSE,         "",                   NULL,                      0,
    "Show this message and exit"
  },
/* 01 */
  { "version",     "v",                  CMDDATA_NODATA,            CMDTYPE_NULL,                    "",
     FALSE,         "",                   NULL,                      0,
    "Show the version and exit."
  },
/* 02 */
  { "trace",       "t",                  CMDDATA_REQUIRED,          CMDTYPE_STR,                     "<file>",
     FALSE,         "log/cchess.log",       gstCmdLine.szTraceFile,    sizeof(gstCmdLine.szTraceFile),
    "<file> is the path of the debug file"
  },
/* 03 */
  { "debug-level", "d",                  CMDDATA_REQUIRED,          CMDTYPE_INT,                     "<number>",
     FALSE,         "0",                  gstCmdLine.szDebugLevel,   sizeof(gstCmdLine.szDebugLevel),
    "<number> is the level of debug level"
  },
/* 04 */
  { "conf-file",   "C",                  CMDDATA_REQUIRED,          CMDTYPE_INT,                     "<number>",
     FALSE,         "0",                  gstCmdLine.szConfFile,     sizeof(gstCmdLine.szConfFile),
    "<file> is the path of the configuration file"
  },
/* 05 */
  { "win-heigth",  "H",                  CMDDATA_REQUIRED,          CMDTYPE_INT,                     "<number>",
     FALSE,         "0",                  gstCmdLine.szWinHeight,    sizeof(gstCmdLine.szWinHeight),
    "<number> is the Window Height"
  },
/* 06 */
  { "win-width",   "W",                  CMDDATA_REQUIRED,          CMDTYPE_INT,                     "<number>",
     FALSE,         "0",                  gstCmdLine.szWinWidth,     sizeof(gstCmdLine.szWinWidth),
    "<number> is the Window Width"
  },
/* 06 */
  { "clock",       "c",                  CMDDATA_REQUIRED,          CMDTYPE_INT,                     "<number>",
     FALSE,         "1",                  gstCmdLine.szClock,        sizeof(gstCmdLine.szClock),
    "<number> define if play with clock (default is 1)"
  },
  /* NOTE: NULL indicates the end of the array, NEVER REMOVE IT! */
  { NULL,         NULL,                   CMDDATA_NULL,             CMDTYPE_NULL,                    "",
     FALSE,        "",                     NULL,                     0,
    NULL
  }
};

STRUCT_COMMAND_LINE gstCmdLine;

char *szGetProgramName( const char *szPathName ) {
  char cDlm = '/';

#ifdef _WIN32
  cDlm = '\\';
#endif /* WIN32 */

  return strrchr(szPathName, cDlm) == NULL 
          ? (char *) szPathName
          : (strrchr(szPathName, cDlm) + 1); 
} /* szGetProgramName */

void vShowOptions( PSTRUCT_CMDLINE astCmdOpt ) {
  int ii = 0;

  for ( ii = 0; astCmdOpt[ii].iRequired != CMDDATA_NULL; ii++ ) {
    if ( astCmdOpt[ii].iRequired == CMDDATA_NODATA ) {
      if ( astCmdOpt[ii].pszShort == NULL || !strcmp( astCmdOpt[ii].pszShort, "" ) ) {
        printf("  --%s\n", astCmdOpt[ii].pszLong);
      }
      else if ( astCmdOpt[ii].pszLong == NULL || !strcmp( astCmdOpt[ii].pszLong, "" ) ) {
        printf("  -%s\n", astCmdOpt[ii].pszShort);
      }
      else {
        printf("  --%s, -%s\n", astCmdOpt[ii].pszLong, astCmdOpt[ii].pszShort);
      }
    }
    else {
      if ( astCmdOpt[ii].pszShort == NULL || !strcmp( astCmdOpt[ii].pszShort, "" ) ) {
        printf("  --%s=%s\n", astCmdOpt[ii].pszLong, astCmdOpt[ii].pszArgument);
      }
      else if ( astCmdOpt[ii].pszLong == NULL || !strcmp( astCmdOpt[ii].pszLong, "" ) ) {
        printf("  -%s%s\n", astCmdOpt[ii].pszShort, astCmdOpt[ii].pszArgument);
      }
      else {
        printf("  --%s=%s, -%s%s\n", astCmdOpt[ii].pszLong, astCmdOpt[ii].pszArgument,
                                     astCmdOpt[ii].pszShort, astCmdOpt[ii].pszArgument);
      }
    }
    printf("    %s\n\n", astCmdOpt[ii].pszHelp);
  } /* for*/
} /* vShowOptions */

void vShowSyntax( const char *pszMsg, PSTRUCT_CMDLINE astCmdOpt ) {
  printf("%s\n", pszMsg);
  vShowOptions( astCmdOpt );
} /* vShowSyntax */

int bCommandLineIsOK( int argc, char **argv, PSTRUCT_CMDLINE astCmdOpt ) {
  int ii = 0;
  int jj = 0;
  char *pszArgument = NULL;
  char *pszParameter = NULL;
  bool bLongCmd = FALSE;
  bool bShortCmd = FALSE;

  for(ii = 1; ii < argc; ii++)
  {
    int iReallocBuffer = snprintf( NULL, 0, "%s", argv[ii] );
    char *pszArgv = ( char * ) malloc(sizeof( char ) * ( iReallocBuffer + 1 ) );

    snprintf(pszArgv, iReallocBuffer+1, "%s", argv[ii]);

    pszArgument = strtok( pszArgv, "=" );

    if ( argv[ii][0] == '-' && argv[ii][1] == '-' ) {
      bLongCmd = TRUE;

      pszParameter = strtok(NULL, "=");

      pszArgument = &pszArgument[2];
    }
    else if ( argv[ii][0] == '-' ) {
      bShortCmd = TRUE;
      pszParameter = ( char * ) malloc( sizeof( char ) * strlen( pszArgument ) - 1 );
      strcpy( pszParameter, &pszArgument[2] );

      snprintf( pszArgument, strlen( pszArgument ), "%c", pszArgument[1] );
    }
    else {
      free( pszArgv );
      pszArgv = NULL;

      return FALSE;
    }
    
    for ( jj = 0; astCmdOpt[jj].iRequired != CMDDATA_NULL; jj++ ) {
      if ( bLongCmd ) {
        if ( !strcmp ( pszArgument, astCmdOpt[jj].pszLong ) ) {
          astCmdOpt[jj].bSet = TRUE;

          if ( astCmdOpt[jj].iRequired == CMDDATA_REQUIRED ) {
            if ( pszParameter == NULL) return FALSE;
            snprintf( astCmdOpt[jj].pszData, astCmdOpt[jj].lDataLength, "%s", pszParameter );
          }

          if ( astCmdOpt[jj].iRequired == CMDDATA_OPTIONAL ) {
            if ( pszParameter == NULL ) {
              bLongCmd = FALSE;

              continue;
            }
            snprintf( astCmdOpt[jj].pszData, astCmdOpt[jj].lDataLength, "%s", pszParameter );
          }
        }
      } /* bLongCmd */
      else if ( bShortCmd ) {
        if ( !strcmp (pszArgument, astCmdOpt[jj].pszShort ) ) {
          astCmdOpt[jj].bSet = TRUE;

          if ( astCmdOpt[jj].iRequired == CMDDATA_NODATA ) {
            bShortCmd = FALSE;

            free( pszParameter );
            pszParameter = NULL;

            continue;
          }
          if ( astCmdOpt[jj].iRequired == CMDDATA_REQUIRED ) {
            if ( pszParameter[0] == 0 ) {
              bShortCmd = FALSE;

              free(pszParameter);
              pszParameter = NULL;

              free(pszArgv);
              pszArgv = NULL;

              return FALSE;
            }
            snprintf(astCmdOpt[jj].pszData, astCmdOpt[jj].lDataLength, "%s", pszParameter);
          }

          if ( astCmdOpt[jj].iRequired == CMDDATA_OPTIONAL ) {
            if (pszParameter[0] == 0 ) {
              bShortCmd = FALSE;

              free(pszParameter);
              pszParameter = NULL;

              continue;
            }

            snprintf(astCmdOpt[jj].pszData, astCmdOpt[jj].lDataLength, "%s", pszParameter);
          }
        }
      } /* bShortCmd */
      else {
        bLongCmd = FALSE;
        bShortCmd = FALSE;

        free(pszArgv);
        pszArgv = NULL;
      }
    } /* for */

    if ( bShortCmd ) {
      free(pszParameter);
      pszParameter = NULL;

      bShortCmd = FALSE;
    }

    if ( bLongCmd ) {
      bLongCmd = FALSE;
    }

    free( pszArgv );
    pszArgv = NULL;
  } /* for */

  return TRUE;
} /* bCommandLineIsOK */
