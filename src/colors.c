#include <colors.h>

/* Retorna uma cor para os quadrados escuros */
SDL_Color SDL_COLOR_GetDarkSquareColor() {
  return (SDL_Color){139, 69, 19, 255}; /* Marrom escuro */
}

/* Retorna uma cor para os quadrados claros */
SDL_Color SDL_COLOR_GetLightSquareColor() {
  return (SDL_Color){222, 184, 135, 255}; /* Bege */
}

/* Define uma cor customizada em um SDL_Color existente */
void vSetCustomColor(SDL_Color *pSDL_COLOR_MyColor, Uint8 ui8_R, Uint8 ui8_G, Uint8 ui8_B, Uint8 ui8_A) {
  if (pSDL_COLOR_MyColor) {
    pSDL_COLOR_MyColor->r = ui8_R;
    pSDL_COLOR_MyColor->g = ui8_G;
    pSDL_COLOR_MyColor->b = ui8_B;
    pSDL_COLOR_MyColor->a = ui8_A;
  }
}
