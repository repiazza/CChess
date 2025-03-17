/**
 * snapshot.c
 * 
 * Written by Renato Fermi <repiazza@gmail.com> in January 2025
 * 
 * Description: Chess written in C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <snapshot.h>

/**
 * Inicializa a estrutura de histórico de snapshots.
 */
PSTRUCT_BOARD_HISTORY pstCreateHistory()
{
  PSTRUCT_BOARD_HISTORY pstHistory = (PSTRUCT_BOARD_HISTORY)malloc(sizeof(STRUCT_BOARD_HISTORY));
  if (!pstHistory)
  {
    vTraceMsg( "Erro ao alocar memória para histórico do tabuleiro.\n");
    exit(EXIT_FAILURE);
  }

  pstHistory->pstFirstSS = NULL;
  pstHistory->pstLastSS = NULL;

  return pstHistory;
}

/**
 * Adiciona um novo snapshot ao histórico.
 */
void vAddSnapshot(PSTRUCT_BOARD_HISTORY pstHistory, STRUCT_SQUARE pstBoard[ROW_SQUARE_COUNT][COLUMN_SQUARE_COUNT])
{
  /* Alocar novo snapshot */
  PSTRUCT_BOARD_SNAPSHOT pstNewSnapshot = (PSTRUCT_BOARD_SNAPSHOT)malloc(sizeof(STRUCT_BOARD_SNAPSHOT));
  if (!pstNewSnapshot)
  {
    vTraceMsg( "Erro ao alocar memória para snapshot.\n");
    exit(EXIT_FAILURE);
  }

  /* Copiar o estado atual do tabuleiro para o novo snapshot */
  memcpy(pstNewSnapshot->stBoardSnapshot, pstBoard, sizeof(STRUCT_SQUARE) * ROW_SQUARE_COUNT * COLUMN_SQUARE_COUNT);
  pstNewSnapshot->pNext = NULL;

  /* Adicionar o novo snapshot ao final da lista */
  if (pstHistory->pstLastSS == NULL)
  {
    /* Lista está vazia: primeiro elemento */
    pstHistory->pstFirstSS = pstNewSnapshot;
    pstHistory->pstLastSS = pstNewSnapshot;
  }
  else
  {
    /* Adicionar ao final da lista */
    pstHistory->pstLastSS->pNext = pstNewSnapshot;
    pstHistory->pstLastSS = pstNewSnapshot;
  }
}

void vSaveHistoryToFile(PSTRUCT_BOARD_HISTORY pstHistory, const char *pszFilename)
{
  int snapshotCount = 0;
  FILE *pfHistory = fopen(pszFilename, "wb");
  PSTRUCT_BOARD_SNAPSHOT pCurrent = pstHistory->pstFirstSS;
  if (!pfHistory){
    vTraceMsg("Erro ao abrir o arquivo %s para gravação.\n", pszFilename);
    return;
  }

  while (pCurrent)
  {
    fwrite(pCurrent->stBoardSnapshot, sizeof(STRUCT_SQUARE), ROW_SQUARE_COUNT * COLUMN_SQUARE_COUNT, pfHistory);
    snapshotCount++;
    pCurrent = pCurrent->pNext;
  }

  printf("Histórico salvo com sucesso com %d snapshots no arquivo %s.\n", snapshotCount, pszFilename);
  fclose(pfHistory);
  pfHistory = NULL;
}

void vFreeHistory(PSTRUCT_BOARD_HISTORY pHistory)
{
  PSTRUCT_BOARD_SNAPSHOT pCurrent = pHistory->pstFirstSS;
  PSTRUCT_BOARD_SNAPSHOT pNext;

  while (pCurrent)
  {
    pNext = pCurrent->pNext;
    free(pCurrent);
    pCurrent = pNext;
  }

  free(pHistory);
}
