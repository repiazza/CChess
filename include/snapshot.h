#ifndef FOOTPRINT_H
  #define FOOTPRINT_H
  
  #include <board.h> // Para STRUCT_SQUARE

typedef struct STRUCT_BOARD_SNAPSHOT {
  STRUCT_SQUARE stBoardSnapshot[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT];
  struct STRUCT_BOARD_SNAPSHOT *pNext;
} STRUCT_BOARD_SNAPSHOT, *PSTRUCT_BOARD_SNAPSHOT;

typedef struct STRUCT_BOARD_HISTORY {
  PSTRUCT_BOARD_SNAPSHOT pstFirstSS;
  PSTRUCT_BOARD_SNAPSHOT pstLastSS;
} STRUCT_BOARD_HISTORY, *PSTRUCT_BOARD_HISTORY;


  // Funções públicas
  void vInitializeHistory(BOARD_HISTORY *pHistory);
  void vAddSnapshot(BOARD_HISTORY *pHistory, STRUCT_SQUARE pBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT]);
  void vSaveHistoryToFile(BOARD_HISTORY *pHistory, const char *pszFileName);
  void vLoadHistoryFromFile(BOARD_HISTORY *pHistory, const char *pszFileName);
  void vFreeHistory(BOARD_HISTORY *pHistory);

#endif // FOOTPRINT_H
