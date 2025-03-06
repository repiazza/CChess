/**
 * openings.h
 * 
 * Written by Gustavo Bacagine <gustavo.bacagine@protonmail.com>
 * 
 * Description: Check by chess openings
 */

#include <openings.h>

STRUCT_OPENING gastOpenings[] = {
  { "London"              , LONDON_OPENING       },
  { "King's Idian Defense", KINGS_INDIAN_DEFENSE },
  { NULL                  , NULL                 }
};

const char *kpszGetOpeningName(char *szBoard) {
  char *kszOpeningBoard = NULL;
  int ii = 0;
  
  if ( bStrIsEmpty(szBoard) ) return NULL;
  
  kszOpeningBoard = &szBoard[32];
  
  for ( ii = 0; gastOpenings[ii].kpszOpeningName; ii++ ) {
    if ( !strcmp(kszOpeningBoard, gastOpenings[ii].kpszOpeningBoard) )
      return gastOpenings[ii].kpszOpeningName;
  }
  
  return NULL;
}
