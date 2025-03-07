/**
 * openings.h
 * 
 * Written by Gustavo Bacagine <gustavo.bacagine@protonmail.com>
 * 
 * Description: Check by chess openings
 */

#include <openings.h>

STRUCT_OPENING gastOpenings[] = {
  { "London"               , LONDON_OPENING       },
  { "Italian"              , ITALIAN_OPENING      },
  { "Ruy Lopez"            , RUY_LOPEZ            },
  { "King's Gambit"        , KINGS_GAMBIT          },
  { "Queen's Gambit"       , QUEENS_GAMBIT         },
  { "King's Indian Defense", KINGS_INDIAN_DEFENSE },
  { NULL                   , NULL                 }
};

char *pszGetOpeningName(char *szBoard) {
  char *pszOpeningBoard32 = NULL;
  char *pszOpeningBoard24 = NULL;
  int ii = 0;
  
  if ( bStrIsEmpty(szBoard) ) return NULL;
  
  pszOpeningBoard32 = &szBoard[32];
  pszOpeningBoard24 = &szBoard[24];
  
  for ( ii = 0; gastOpenings[ii].kpszOpeningName; ii++ ) {
    if ( !strcmp(pszOpeningBoard32, gastOpenings[ii].kpszOpeningBoard) ||
          !strcmp(pszOpeningBoard24, gastOpenings[ii].kpszOpeningBoard) )
      return gastOpenings[ii].kpszOpeningName;
  }
  
  return NULL;
}
