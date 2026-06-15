#pragma once
#include <stdint.h>

// Status bar ikonlari
void drawBatteryIcon(int x, int y, uint16_t color);  // 12x7 px
void drawSdIcon(int x, int y, uint16_t color);       // 9x10 px

// Transport circles (sadece kontur + icin icon)
void drawPlayCircle(int cx, int cy, int r, uint16_t color);
void drawPauseCircle(int cx, int cy, int r, uint16_t color);
void drawPrevCircle(int cx, int cy, int r, uint16_t color);
void drawNextCircle(int cx, int cy, int r, uint16_t color);
