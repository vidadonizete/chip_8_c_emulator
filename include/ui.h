#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include <struct.h>

void init_ui();

void draw_ui(uint8_t display[ROW][BYTE_COLUMN]);

void clear_ui();

bool check_ui();

void close_ui();

#endif //UI_H