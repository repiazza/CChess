/**
 * trace.h
 * 
 * Written by Renato Fermi <repiazza@gmail.com>
 *
 * Trace functions and global variables
 * 
 */

#ifndef _TRACE_H_
  #define _TRACE_H_

/******************************************************************************
 *                                                                            *
 *                                 Includes                                   *
 *                                                                            *
 ******************************************************************************/ 
  #include <time.h>
  #include <errno.h>
  #include <stdarg.h>
  #include <consts.h>
  
  #ifdef _WIN32
    #include <windows.h>
    #include <process.h>
  #elif LINUX
    #include <unistd.h>
    #include <sys/time.h>
  #endif
  
  #include <board.h>
  
/******************************************************************************
 *                                                                            *
 *                             Defines and macros                             *
 *                                                                            *
 ******************************************************************************/
  #define DEBUG_MSGS      gstTracePrm.iDebugLevel > 0
  #define DEBUG_MORE_MSGS gstTracePrm.iDebugLevel > 8

  #define vTraceMsg(FORMAT, ...) vTraceMessage(__FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
  #define vTraceError(FORMAT, ...) vTraceErr(__FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
  #define vTraceVarArgs(FORMAT, ...) _vTraceVarArgs(__FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
  #define vTraceBegin() vTraceVarArgs("%s - begin", __func__)
  #define vTraceEnd() vTraceVarArgs("%s - end", __func__)
  
  #define vTraceSquare vTraceBoardRowCol

/******************************************************************************
 *                                                                            *
 *                     Global variables and constants                         *
 *                                                                            *
 ******************************************************************************/

  typedef struct STRUCT_TRACE_PRM {
    char szTraceFile[_MAX_PATH+_MAX_PATH];
    int iDebugLevel;
  } STRUCT_TRACE_PRM, *PSTRUCT_TRACE_PRM;
  
  extern STRUCT_TRACE_PRM gstTracePrm;
  extern char gszConfFile[_MAX_PATH];

/******************************************************************************
 *                                                                            *
 *                                 Prototypes                                 *
 *                                                                            *
 ******************************************************************************/ 
  /**
   * bacagine - 2023-11-10 - Read a .conf file and
   * get the value of DEBUG_LEVEL.
   * 
   * Return:
   *  If not found DEBUG_LEVEL or can't read .conf file, return 0.
   *  If can read .conf file return the value of DEBUG_LEVEL
   */
  int iGetDebugLevel(const char *kpszConfFile);

  /**
   * bacagine - 2023-11-10 - Set the name of the .conf file
   */
  void vSetConfFile(void);

  /**
   * bacagine - 2023-11-10 - Set the name of the .log file
   */
  void vSetLogFile(void);
  
  /**
   * bacagine - 2023-11-10 - Set the value of gstTracePrm.iDebugLevel
   */
  void vSetDebugLevel(void);
  
  void vTraceErr(const char *kpszModuleName, const int kiLine, const char *kpszFmt, ...);
  void vTraceMessage(const char *kpszModuleName, const int kiLine, const char *kpszFmt, ...);
  void vTracePid(char *szMsg, int iMsgLen);
  void vTraceMsgNoNL(char *szMsg);
  void vInitLogs(void);
  void vInitTraceConfig(void);
  
  /**
   * bacagine - 2023-mm-dd - Advanced trace function
   */
  void _vTraceVarArgs(const char *kpszModuleName,
                       const int kiLine,
                       const char *kpszFmt, ...);
  
  /**
   * bacagine - 2023-mm-dd A detailed and excessive sample of what was passed on the command line
   */
  void vTraceCommandLine(int argc, char **argv);

  /**
   * bacagine - 2023-mm-dd - A detailed and excessive sampling of the system's environment variables
   */
  void vTraceEnvp(char **envp);
  
  /**
   * bacagine - 2025-03-17 - A detailed and excessive sampling of the apple main parameter
   */
  void vTraceApple(char **apple);

  void vTraceBoard(STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
  void vTraceBoardRowCol(const char *kpszMsg, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT], int iRow, int iCol);

#endif /* _TRACE_H */

