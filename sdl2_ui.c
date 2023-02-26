#include <stdbool.h>
#include <SDL2/SDL.h>

#include <struct.h>
#include <ui.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Event event;

void init_ui()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error starting SDL2: %s", SDL_GetError());
        exit(0);
    }

    if (SDL_CreateWindowAndRenderer(
            COLUMN,
            ROW,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN,
            &window,
            &renderer) < 0)
    {
        fprintf(stderr, "Error starting SDL2: %s", SDL_GetError());
        exit(0);
    }

    SDL_RenderSetScale(renderer, 10, 10);
    SDL_SetWindowSize(window, COLUMN * 10, ROW * 10);
}

void draw_ui(uint8_t display[ROW][BYTE_COLUMN])
{
    clear_ui();

    for (int r = 0; r < ROW; r++)
    {
        for (int c = 0; c < BYTE_COLUMN; c++)
        {
            uint8_t byte = display[r][c];
            for (uint8_t i = 0; i < BYTE_BITS; i++)
            {
                if ((byte << i) & 0x80)
                {
                    SDL_RenderDrawPoint(renderer, c * BYTE_BITS + i, r);
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void clear_ui()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

bool check_ui()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return false;
        }
    }
    return true;
}

void close_ui()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    window = NULL;
    renderer = NULL;
}