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

struct chip_8
{
    uint8_t memory[4 * 1024];
    uint16_t pc;
    uint16_t I;
    uint8_t registers[16];
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delay;
    uint8_t sound;
    // 32x64 == 256 * 8 bits
    uint8_t display[256];
};

static void read_rom(struct chip_8 *chip_8, char *name)
{
    FILE *file = fopen("../ibm.ch8", "rb");
    if (!file)
    {
        fprintf(stderr, "Error opening %s", name);
        exit(0);
    }
    fseek(file, 0L, SEEK_END);
    int bytes = ftell(file);
    rewind(file);
    if (!fread(chip_8->memory + ROM_OFFSET, bytes, 1, file))
    {
        fprintf(stderr, "Error reading %s", name);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error starting SDL2: %s", SDL_GetError());
        exit(0);
    }

    SDL_Window *window = SDL_CreateWindow("CHIP-8", 0, 0, 64, 32, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    struct chip_8 chip_8 = {
        .memory = {},
        .pc = 0x200,
        .I = 0,
        .registers = {},
        .stack = {},
        .sp = 0,
        .delay = 0,
        .sound = 0,
        .display = {},
    };
    read_rom(&chip_8, "../ibm.ch8");
    memcpy(chip_8.memory + FONT_OFFSET, font, sizeof(font));

    uint16_t opcode = 0;
    uint8_t left = 0;
    uint8_t right = 0;

    while (0)
    {
        left = chip_8.memory[chip_8.pc++];
        right = chip_8.memory[chip_8.pc++];
    }

    SDL_DestroyWindow(window);
    return 0;
}