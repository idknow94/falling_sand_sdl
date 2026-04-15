#define SDL_MAIN_USE_CALLBACKS 1
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stddef.h>
#include <stdint.h>

#define TILE_SIZE 5
#define WIDTH 200
#define HEIGHT 150
#define LENGTH (WIDTH * HEIGHT)
#define IDX(x, y) ((x) + WIDTH * (y))

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

typedef enum { EMPTY = 0, SAND = 1, WATER = 2 } CellType;

typedef struct {
    CellType type;
    unsigned int age;
} Cell;

bool mouse_pressed = false;

Cell current[LENGTH];
Cell next[LENGTH];
uint32_t pixels[LENGTH];

unsigned int counter = 1;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("sand", WIDTH * TILE_SIZE,
                                     HEIGHT * TILE_SIZE, 0, &window,
                                     &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                                SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        mouse_pressed = true;
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        mouse_pressed = false;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    float mx_f, my_f;
    SDL_GetMouseState(&mx_f, &my_f);

    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    int mx = (int)(mx_f * WIDTH / window_w);
    int my = (int)(my_f * HEIGHT / window_h);

    if (mouse_pressed && my >= 0 && my < HEIGHT && mx >= 0 && mx < WIDTH &&
        current[IDX(mx, my)].type == EMPTY) {
        current[IDX(mx, my)] = (Cell){.type = SAND, .age = ++counter};
    }

    for (int i = 0; i < LENGTH; i++) {
        next[i] = (Cell){.type = EMPTY, .age = 0};
    }

    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            int i = IDX(x, y);

            if (current[i].type == SAND) {
                if (y + 1 < HEIGHT && current[IDX(x, y + 1)].type == EMPTY) {
                    next[IDX(x, y + 1)] = current[i];
                } else if (x > 0 && y + 1 < HEIGHT &&
                           current[IDX(x - 1, y + 1)].type == EMPTY) {
                    next[IDX(x - 1, y + 1)] = current[i];
                } else if (x < WIDTH - 1 && y + 1 < HEIGHT &&
                           current[IDX(x + 1, y + 1)].type == EMPTY) {
                    next[IDX(x + 1, y + 1)] = current[i];
                } else {
                    next[i] = current[i];
                }
            }
        }
    }

    for (int i = 0; i < LENGTH; i++) {
        current[i] = next[i];
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int i = IDX(x, y);

            if (current[i].type == SAND) {
                float t = current[i].age / (float)counter;
                if (t < 0.3f)
                    t = 0.3f;

                uint8_t r = (uint8_t)(200 * t);
                uint8_t g = (uint8_t)(170 * t);
                uint8_t b = (uint8_t)(80 * t);

                pixels[i] = (255u << 24) | (b << 16) | (g << 8) | r;
            } else {
                float t = y / (float)HEIGHT;

                uint8_t r = (uint8_t)(50 * t);
                uint8_t g = (uint8_t)(20 * t);
                uint8_t b = (uint8_t)(120 * t);

                pixels[i] = (255u << 24) | (b << 16) | (g << 8) | r;
            }
        }
    }

    SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
