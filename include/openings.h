/**
 * openings.h
 * 
 * Written by Gustavo Bacagine <gustavo.bacagine@protonmail.com>
 * 
 * Description: Check by chess openings
 */

#ifndef _OPENINGS_H_
#define _OPENINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <consts.h>
#include <util.h>

/* Openings */
#define LONDON_OPENING \
"___P_B__"\
"__PBPN__"\
"PP_N_PPP"\
"R__QK__R"

#define ITALIAN_OPENING \
"__B_P___"\
"_____N__"\
"PPPP_PPP"\
"RNBQK__R"

#define RUY_LOPEZ \
"_B__P___"\
"____P___"\
"_____N__"\
"PPPP_PPP"\
"RNBQK__R"

#define KINGS_GAMBIT \
"____PP__"\
"________"\
"PPPP__PP"\
"RNBQKBNR"

#define QUEENS_GAMBIT \
"__PP____"\
"________"\
"PP__PPPP"\
"RNBQKBNR"

/* Defenses */
#define KINGS_INDIAN_DEFENSE \
"________"\
"_PN_____"\
"PBPPPPPP"\
"R__KQBNR"

typedef struct STRUCT_OPENING {
  char *kpszOpeningName;
  const char *kpszOpeningBoard;
} STRUCT_OPENING, *PSTRUCT_OPENING;

extern STRUCT_OPENING gastOpenings[];

char *pszGetOpeningName(char *szBoard);

#ifdef __cplusplus
}
#endif

#endif
