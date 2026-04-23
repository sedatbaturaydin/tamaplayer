#include "MusicPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../music/MusicPlayer.h"
#include <string.h>
#include <stdio.h>
#include <Arduino.h>

static const int LIST_TOP     = 26;    // baslik altindan sonra ilk satir
static const int LIST_ROW_H   = 12;    // satir yuksekligi (px)
static const int LIST_PAD_X   = 6;     // sol/sag padding
static const int LIST_CHARS   = 18;    // bir satirda max karakter (128px / 6 ~= 21, marj icin 18)

static int _visibleRows() {
  int h = SCREEN_H - LIST_TOP - 10;  // alt kenarda 10px bosluk
  int n = h / LIST_ROW_H;
  if (n < 1) n = 1;
  return n;
}

static int _scrollOffset(int selected, int total) {
  int rows = _visibleRows();
  if (total <= rows) return 0;
  int off = selected - rows / 2;
  if (off < 0) off = 0;
  if (off > total - rows) off = total - rows;
  return off;
}

static void drawHeader() {
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor(PW(6), PH(8));
  tft.print("MUSIC");

  int total = musicTrackCount();
  char counter[12];
  if (total == 0) snprintf(counter, sizeof(counter), "--");
  else            snprintf(counter, sizeof(counter), "%d/%d", musicIndex + 1, total);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  int w = (int)strlen(counter) * 6;
  tft.setCursor(SCREEN_W - PW(6) - w, PH(8));
  tft.print(counter);

  tft.drawFastHLine(PW(6), PH(20), SCREEN_W - PW(12), activeTheme->accent);
}

static void truncName(const char* src, char* dst, int maxChars) {
  int n = (int)strlen(src);
  if (n <= maxChars) { strcpy(dst, src); return; }
  // son "..." ile kes
  int keep = maxChars - 3;
  if (keep < 1) keep = 1;
  memcpy(dst, src, keep);
  dst[keep]     = '.';
  dst[keep + 1] = '.';
  dst[keep + 2] = '.';
  dst[keep + 3] = '\0';
}

static void drawList() {
  int total = musicTrackCount();
  int rows  = _visibleRows();
  int off   = _scrollOffset(musicIndex, total);

  // liste alanini temizle
  tft.fillRect(0, LIST_TOP, SCREEN_W, rows * LIST_ROW_H, activeTheme->bg);

  if (total == 0) {
    const char* msg = musicSdOk() ? "No MP3 files" : "No SD card";
    tft.setTextSize(1);
    tft.setTextColor(activeTheme->dim, activeTheme->bg);
    int w = (int)strlen(msg) * 6;
    tft.setCursor((SCREEN_W - w) / 2, LIST_TOP + 20);
    tft.print(msg);
    return;
  }

  tft.setTextSize(1);
  char line[LIST_CHARS + 4];

  for (int i = 0; i < rows && (off + i) < total; i++) {
    int idx = off + i;
    int y   = LIST_TOP + i * LIST_ROW_H;
    bool selected = (idx == musicIndex);

    uint16_t fg, bg;
    if (selected) {
      fg = activeTheme->bg;
      bg = activeTheme->accent;
      tft.fillRect(LIST_PAD_X - 2, y - 1, SCREEN_W - 2 * (LIST_PAD_X - 2), LIST_ROW_H, bg);
    } else {
      fg = activeTheme->light;
      bg = activeTheme->bg;
    }

    truncName(musicTrackName(idx), line, LIST_CHARS);
    tft.setTextColor(fg, bg);
    tft.setCursor(LIST_PAD_X, y + 2);
    tft.print(line);
  }
}

void drawMusicPage() {
  tft.fillScreen(activeTheme->bg);
  drawHeader();
  drawList();
}

void drawMusicPagePartial() {
  // secim degisti: basligin sag tarafindaki counter + liste yenile
  drawHeader();
  drawList();
}
