#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <struct.h>
#include <ui.h>

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

int main(int argc, char *argv[])
{
    init_ui();

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

    read_rom(&chip_8, "../ibm.ch8");
    memcpy(chip_8.memory + FONT_OFFSET, font, sizeof(font));

    uint16_t opcode = 0;
    uint8_t left = 0;
    uint8_t right = 0;
    uint8_t code = 0;

    while (check_ui())
    {
        left = chip_8.memory[chip_8.pc++];
        right = chip_8.memory[chip_8.pc++];
        opcode = (left << 8) | right;
        code = left >> 4;

        if (left == 0x00)
        {
            if (right == 0xE0) // 00E0 (clear screen)
            {
                memset(chip_8.display, 0, sizeof(chip_8.display));
                clear_ui();
            }
            else if (right == 0xEE) // 00EE pop stack to PC
            {
                chip_8.pc = chip_8.stack[--chip_8.sp];
            }
        }
        else if (code == 0x01) // 1NNN set PC to NNN
        {
            chip_8.pc = opcode & 0x0FFF;
        }
        else if (code == 0x02) // 2NNN calls the subroutine at memory location NNN
        {
            chip_8.stack[chip_8.sp++] = chip_8.pc - 2;
            chip_8.pc = opcode & 0x0FFF;
        }
        else if (code == 0x06) // 6XNN (set register VX)
        {
            chip_8.registers[(opcode & 0x0F00) >> 8] = opcode; // & 0x00FF;
        }
        else if (code == 0x07) // 7XNN (add value to register VX)
        {
            chip_8.registers[(opcode & 0x0F00) >> 8] += opcode; // & 0x00FF;
        }
        else if (code == 0x0A) // ANNN (set index register I)
        {
            chip_8.index = opcode & 0x0FFF;
        }
        else if (code == 0x0D) // DXYN (display/draw)
        {
            uint8_t n = opcode & 0x000F;
            uint8_t y = chip_8.registers[(opcode & 0x00F0) >> 4] % ROW;
            uint8_t x = chip_8.registers[(opcode & 0x0F00) >> 8] % COLUMN;

            uint8_t *sprite = chip_8.memory + chip_8.index;

            for (uint8_t r = 0; r < n && r + y < ROW; r++, sprite++)
            {
                uint8_t *column = chip_8.display[r + y];
                uint8_t offset = x % BYTE_BITS;
                uint8_t position = x / BYTE_BITS;
                *(column + position++) ^= *sprite >> offset;
                if (position >= BYTE_COLUMN) continue;
                *(column + position) ^= *sprite << (BYTE_BITS - offset);
            }

            draw_ui(chip_8.display);
        }
        else
        {
            printf("Unknown opcode: 0x%08X\n", opcode);
        }
    }

    close_ui();

    return 0;
}