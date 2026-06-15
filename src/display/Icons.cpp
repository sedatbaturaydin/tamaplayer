#include "Icons.h"
#include "DisplayManager.h"

void drawBatteryIcon(int x, int y, uint16_t c) {
  // Govde 11x7, sag ucta + cikinti 1x3
  tft.drawRect(x, y, 11, 7, c);
  tft.drawFastVLine(x + 11, y + 2, 3, c);
  // 3 dolu cubuk
  tft.fillRect(x + 2, y + 2, 2, 3, c);
  tft.fillRect(x + 5, y + 2, 2, 3, c);
  tft.fillRect(x + 8, y + 2, 1, 3, c);
}

void drawSdIcon(int x, int y, uint16_t c) {
  // SD kart silueti: 9x10, sol ust kose pah
  tft.drawLine(x + 2, y,     x + 8, y,     c);
  tft.drawLine(x + 8, y,     x + 8, y + 9, c);
  tft.drawLine(x + 8, y + 9, x,     y + 9, c);
  tft.drawLine(x,     y + 9, x,     y + 2, c);
  tft.drawLine(x,     y + 2, x + 2, y,     c);
  // Ust kenar pin noktalari
  tft.drawPixel(x + 4, y + 2, c);
  tft.drawPixel(x + 6, y + 2, c);
}

void drawPlayCircle(int cx, int cy, int r, uint16_t c) {
  tft.drawCircle(cx, cy, r, c);
  int s = r - 4;
  if (s < 3) s = 3;
  tft.fillTriangle(cx - s/2, cy - s,
                   cx - s/2, cy + s,
                   cx + s,   cy,
                   c);
}

void drawPauseCircle(int cx, int cy, int r, uint16_t c) {
  tft.drawCircle(cx, cy, r, c);
  int h = r - 2;
  if (h < 4) h = 4;
  tft.fillRect(cx - 4, cy - h/2, 2, h, c);
  tft.fillRect(cx + 2, cy - h/2, 2, h, c);
}

void drawPrevCircle(int cx, int cy, int r, uint16_t c) {
  tft.drawCircle(cx, cy, r, c);
  int s = r - 4;
  if (s < 3) s = 3;
  tft.fillTriangle(cx + s/2, cy - s,
                   cx + s/2, cy + s,
                   cx - s/2, cy,
                   c);
  tft.drawFastVLine(cx - s/2 - 1, cy - s, 2*s + 1, c);
}

void drawNextCircle(int cx, int cy, int r, uint16_t c) {
  tft.drawCircle(cx, cy, r, c);
  int s = r - 4;
  if (s < 3) s = 3;
  tft.fillTriangle(cx - s/2, cy - s,
                   cx - s/2, cy + s,
                   cx + s/2, cy,
                   c);
  tft.drawFastVLine(cx + s/2 + 1, cy - s, 2*s + 1, c);
}
