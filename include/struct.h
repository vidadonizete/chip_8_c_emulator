#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>

#define FONT_OFFSET 0x50
#define ROM_OFFSET 0x200
#define ROW 32
#define COLUMN 64
#define BYTE_BITS 8
#define BYTE_COLUMN COLUMN / BYTE_BITS

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
    uint8_t display[ROW][BYTE_COLUMN];
};

#endif //STRUCT_H