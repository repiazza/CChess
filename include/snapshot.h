#ifndef FOOTPRINT_H
  #define FOOTPRINT_H
  
  #include <board.h>
  #include <trace.h>
  
typedef struct STRUCT_BOARD_SNAPSHOT {
  STRUCT_SQUARE stBoardSnapshot[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT];
  struct STRUCT_BOARD_SNAPSHOT *pNext;
} STRUCT_BOARD_SNAPSHOT, *PSTRUCT_BOARD_SNAPSHOT;

typedef struct STRUCT_BOARD_HISTORY {
  PSTRUCT_BOARD_SNAPSHOT pstFirstSS;
  PSTRUCT_BOARD_SNAPSHOT pstLastSS;
} STRUCT_BOARD_HISTORY, *PSTRUCT_BOARD_HISTORY;


  /* Funções públicas */
  void vInitializeHistory(STRUCT_BOARD_HISTORY *pHistory);
  void vAddSnapshot(STRUCT_BOARD_HISTORY *pHistory, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
  void vSaveHistoryToFile(STRUCT_BOARD_HISTORY *pHistory, const char *pszFileName);
  void vLoadHistoryFromFile(STRUCT_BOARD_HISTORY *pHistory, const char *pszFileName);
  void vFreeHistory(STRUCT_BOARD_HISTORY *pHistory);
  PSTRUCT_BOARD_HISTORY pstCreateHistory(void);
  
#endif /* FOOTPRINT_H */
