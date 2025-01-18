#ifndef EVENT_H
#define EVENT_H

#include <SDL2/SDL.h>

// Prototipação das funções de eventos
void vHandleMouseEvent(SDL_Event *pSDL_Event_Event);
void vHandleQuitEvent(SDL_Event *pSDL_Event_Event);

#endif // EVENT_H