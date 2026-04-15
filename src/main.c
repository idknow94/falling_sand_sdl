#define SDL_MAIN_USE_CALLBACKS 1  
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include <stddef.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define TILE_SIZE 5
#define WIDTH 100
#define HEIGHT 100
#define LENGTH (WIDTH * HEIGHT)
#define IDX(x, y) ((x) + WIDTH * (y))

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

int current[LENGTH];
int next[LENGTH];

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  
    }
    return SDL_APP_CONTINUE;  
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, SDL_ALPHA_OPAQUE_FLOAT);  
    
    SDL_RenderClear(renderer);
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if ((x + y) % 2 == 0) {
                SDL_SetRenderDrawColorFloat(renderer, 1.0, 1.0, 1.0, SDL_ALPHA_OPAQUE_FLOAT);
                SDL_RenderRect(renderer, &(SDL_FRect){x, y, 1.0, 1.0});
            }
        }
    }
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{}

