#include "MusicPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../music/MusicPlayer.h"
#include <string.h>
#include <stdio.h>
#include <Arduino.h>

// ── sabitler (PH/PW ile ölçeklenir) ──────────────────────────────────────────

#define TITLE_Y          PH(42)
#define TITLE_AREA_X     PW(4)
#define TITLE_AREA_W     (SCREEN_W - PW(8))
#define MAX_VISIBLE      (TITLE_AREA_W / 6)   // kaç karakter sığar
#define SCROLL_SPEED_MS  420
#define SCROLL_GAP_CHARS 4

// ── scroll state ─────────────────────────────────────────────────────────────

struct ScrollState {
  int      offset = 0;
  uint32_t lastMs = 0;
  bool     active = false;
};

static ScrollState _titleScroll;
static ScrollState _artistScroll;

// ── yardımcı ─────────────────────────────────────────────────────────────────

static int centerX(const char* text, uint8_t size) {
  return (SCREEN_W - (int)strlen(text) * 6 * size) / 2;
}

static void formatTime(char* buf, uint16_t secs) {
  sprintf(buf, "%d:%02d", secs / 60, secs % 60);
}

static void drawNote(int x, int y, uint16_t color) {
  tft.fillCircle(x,     y + 8, 3, color);
  tft.fillCircle(x + 9, y + 6, 3, color);
  tft.drawFastVLine(x + 3,  y,     9, color);
  tft.drawFastVLine(x + 12, y - 2, 9, color);
  tft.drawFastHLine(x + 3,  y,    10, color);
}

// Karakter tabanlı scroll — sabit alan, sabit konum, ST7735 sarma riski yok.
static void drawScrollLine(const char* text, ScrollState& s,
                           int areaX, int y, uint16_t color) {
  int tlen = strlen(text);
  tft.setTextSize(1);
  tft.setTextColor(color, activeTheme->bg);

  if (!s.active) {
    tft.fillRect(areaX, y, TITLE_AREA_W, 8, activeTheme->bg);
    tft.setCursor(areaX + (TITLE_AREA_W - tlen * 6) / 2, y);
    tft.print(text);
  } else {
    int cycle = tlen + SCROLL_GAP_CHARS;
    tft.setCursor(areaX, y);
    for (int i = 0; i < MAX_VISIBLE; i++) {
      int pos = (s.offset + i) % cycle;
      tft.print(pos < tlen ? text[pos] : ' ');
    }
  }
}

// ── scroll API ───────────────────────────────────────────────────────────────

static void resetScroll(ScrollState& s, const char* text) {
  s.offset = 0;
  s.lastMs = millis();
  s.active = ((int)strlen(text) > MAX_VISIBLE);
}

static bool tickScroll(ScrollState& s, const char* text) {
  if (!s.active) return false;
  uint32_t now = millis();
  if (now - s.lastMs < SCROLL_SPEED_MS) return false;
  s.lastMs = now;
  s.offset = (s.offset + 1) % ((int)strlen(text) + SCROLL_GAP_CHARS);
  return true;
}

void musicPageResetScroll() {
  const Track* t = musicCurrentTrack();
  resetScroll(_titleScroll,  t->title);
  resetScroll(_artistScroll, t->artist);
}

bool musicPageScrollTick() {
  const Track* t = musicCurrentTrack();
  bool ta = tickScroll(_titleScroll,  t->title);
  bool tb = tickScroll(_artistScroll, t->artist);
  return ta || tb;
}

// forward declaration
void _drawBarAndStatus();

// ── tam ekran çizim ──────────────────────────────────────────────────────────

void drawMusicPage() {
  musicPageResetScroll();

  const Track* t = musicCurrentTrack();
  tft.fillScreen(activeTheme->bg);

  // Başlık
  drawNote(PW(10), PH(4), activeTheme->accent);
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor(PW(28), PH(8));
  tft.print("MUSIC");
  tft.drawFastHLine(PW(6), PH(20), SCREEN_W - PW(12), activeTheme->accent);

  // Sanatçı + parça adı
  drawScrollLine(t->artist, _artistScroll, TITLE_AREA_X, PH(29), activeTheme->dim);
  drawScrollLine(t->title,  _titleScroll,  TITLE_AREA_X, TITLE_Y, activeTheme->light);

  // Parça sayacı
  char counter[12];
  sprintf(counter, "%d / %d", musicIndex + 1, TRACK_COUNT);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(centerX(counter, 1), PH(56));
  tft.print(counter);

  // Progress bar border
  tft.drawRect(PW(8), PH(72), SCREEN_W - PW(16), 8, activeTheme->dim);

  // Statik alt kısım
  tft.drawFastHLine(PW(6), PH(106), SCREEN_W - PW(12), activeTheme->dim);
  tft.drawFastHLine(PW(6), PH(128), SCREEN_W - PW(12), activeTheme->dim);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(PW(4),          PH(138)); tft.print("|< prev");
  tft.setCursor(SCREEN_W - PW(52), PH(138)); tft.print("next >|");
  tft.setCursor(centerX("[ ] play/pause", 1), PH(150));
  tft.print("[ ] play/pause");

  _drawBarAndStatus();
}

// ── kısmi güncelleme ─────────────────────────────────────────────────────────

void drawMusicPagePartial() {
  const Track* t = musicCurrentTrack();

  if (_artistScroll.active)
    drawScrollLine(t->artist, _artistScroll, TITLE_AREA_X, PH(29), activeTheme->dim);
  if (_titleScroll.active)
    drawScrollLine(t->title,  _titleScroll,  TITLE_AREA_X, TITLE_Y, activeTheme->light);

  _drawBarAndStatus();
}

// ── bar + süre + durum ────────────────────────────────────────────────────────

void _drawBarAndStatus() {
  const Track* t = musicCurrentTrack();

  const int BAR_X = PW(8);
  const int BAR_Y = PH(72);
  const int BAR_W = SCREEN_W - PW(16);
  const int BAR_H = 8;

  int fill = 0;
  if (t->duration > 0) {
    fill = ((uint32_t)musicPos * (BAR_W - 2)) / t->duration;
    if (fill > BAR_W - 2) fill = BAR_W - 2;
  }
  tft.fillRect(BAR_X + 1, BAR_Y + 1, fill,           BAR_H - 2, activeTheme->accent);
  tft.fillRect(BAR_X + 1 + fill, BAR_Y + 1, BAR_W - 2 - fill, BAR_H - 2, activeTheme->bg);
  if (fill < BAR_W - 3)
    tft.fillRect(BAR_X + 1 + fill, BAR_Y, 2, BAR_H, activeTheme->light);

  char elapsed[8], total[8], timeStr[18];
  formatTime(elapsed, musicPos);
  formatTime(total, t->duration);
  sprintf(timeStr, "%s / %s", elapsed, total);
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(centerX(timeStr, 1), PH(84));
  tft.print(timeStr);

  const char* statusStr = musicPlaying ? "> PLAYING" : "|| PAUSED";
  tft.setTextColor(musicPlaying ? activeTheme->accent : activeTheme->dim, activeTheme->bg);
  tft.setCursor(centerX(statusStr, 1), PH(114));
  tft.print(statusStr);
}
