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

#define LONDON_OPENING \
"___P_B__"\
"__PBPN__"\
"PP_N_PPP"\
"R__QK__R"

#define KINGS_INDIAN_DEFENSE \
"________"\
"_PN_____"\
"PBPPPPPP"\
"R__QKBNR"

typedef struct STRUCT_OPENING {
  const char *kpszOpeningName;
  const char *kpszOpeningBoard;
} STRUCT_OPENING, *PSTRUCT_OPENING;

extern STRUCT_OPENING gastOpenings[];

const char *kpszGetOpeningName(char *szBoard);

#ifdef __cplusplus
}
#endif

#endif
