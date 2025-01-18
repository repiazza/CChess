#ifndef COLORS_H
#define COLORS_H

#include <SDL2/SDL.h>

// Funções para obter cores padrão (agora retornam instâncias diretas de SDL_Color)
SDL_Color SDL_COLOR_GetDarkSquareColor();
SDL_Color SDL_COLOR_GetLightSquareColor();

// Função para definir uma cor customizada
void vSetCustomColor(SDL_Color *pSDL_COLOR_MyColor, Uint8 ui8_R, Uint8 ui8_G, Uint8 ui8_B, Uint8 ui8_A);

#endif // COLORS_H
