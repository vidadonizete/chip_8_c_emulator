#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <SDL2/SDL.h>

static uint8_t font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

#define FONT_OFFSET 0x50
#define ROM_OFFSET 0x200
#define ROW 32
#define COLUMN 64

struct chip_8
{
    uint8_t memory[4 * 1024];
    uint16_t pc;
    uint16_t index;
    uint8_t registers[16];
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delay;
    uint8_t sound;
    uint8_t display[32 * 64];
};

static void read_rom(struct chip_8 *chip_8, char *name)
{
    FILE *file = fopen(name, "rb");
    if (!file)
    {
        fprintf(stderr, "Error opening %s", name);
        exit(0);
    }
    fseek(file, 0L, SEEK_END);
    int bytes = ftell(file);
    rewind(file);
    if (!fread(chip_8->memory + ROM_OFFSET, bytes, sizeof(uint8_t), file))
    {
        fprintf(stderr, "Error reading %s", name);
        exit(0);
    }
    fclose(file);
}

static void clearScreen(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

static void drawPixels(SDL_Renderer *renderer, uint8_t display[2048])
{
    clearScreen(renderer);
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COLUMN; j++)
        {
            if (display[i * COLUMN + j])
            {
                SDL_RenderDrawPoint(renderer, j, i);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error starting SDL2: %s", SDL_GetError());
        exit(0);
    }

    SDL_Window *window;
    SDL_Renderer *renderer;

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

    clearScreen(renderer);

    struct chip_8 chip_8 = {
        .memory = {},
        .pc = ROM_OFFSET,
        .index = 0,
        .registers = {},
        .stack = {},
        .sp = 0,
        .delay = 0,
        .sound = 0,
        .display = {},
    };

    read_rom(&chip_8, "../test.ch8");
    memcpy(chip_8.memory + FONT_OFFSET, font, sizeof(font));

    uint16_t opcode = 0;
    uint8_t left = 0;
    uint8_t right = 0;
    uint8_t code = 0;

    while (1)
    {
        left = chip_8.memory[chip_8.pc++];
        right = chip_8.memory[chip_8.pc++];
        opcode = (left << 8) | right;
        code = left >> 4;

        if (opcode == 0x00E0) // 00E0 (clear screen)
        {
            memset(chip_8.display, 0, sizeof(chip_8.display));
            clearScreen(renderer);
        }
        else if (code == 0x1) // 1NNN (jump)
        {
            chip_8.pc = opcode & 0x0FFF;
        }
        else if (code == 0x6) // 6XNN (set register VX)
        {
            chip_8.registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        }
        else if (code == 0x7) // 7XNN (add value to register VX)
        {
            chip_8.registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        }
        else if (code == 0xA) // ANNN (set index register I)
        {
            chip_8.index = opcode & 0x0FFF;
        }
        else if (code == 0xD) // DXYN (display/draw)
        {
            chip_8.registers[0x0F] = 0;

            uint8_t n = opcode & 0x000F;
            uint8_t y = chip_8.registers[(opcode & 0x00F0) >> 4] % 32;
            uint8_t x = chip_8.registers[(opcode & 0x0F00) >> 8] % 64;

            uint8_t *sprite = chip_8.memory + chip_8.index;

            for (uint8_t r = 0; r < n && r + y < ROW; r++)
            {
                uint8_t *row = chip_8.display + ((y + r) * COLUMN);
                for (uint8_t c = 0; c < 8 && c + x < COLUMN; c++)
                {
                    uint8_t pixel = (*sprite >> (7 - c)) & 1;
                    *(row + x + c) = pixel;
                }
                sprite++;
            }

            drawPixels(renderer, chip_8.display);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}