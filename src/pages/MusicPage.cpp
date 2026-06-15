#include "MusicPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../display/StatusBar.h"
#include "../display/Icons.h"
#include "../music/MusicPlayer.h"
#include "../i18n/Strings.h"
#include <string.h>
#include <stdio.h>
#include <Arduino.h>

static MusicView _view = MUSIC_VIEW_LIST;

void musicPageSetView(MusicView v) { _view = v; }
MusicView musicPageGetView()       { return _view; }

// ── LIST VIEW ────────────────────────────────────────────────────────────────

static const int LIST_TOP     = 34;
static const int LIST_ROW_H   = 12;
static const int LIST_PAD_X   = 6;
static const int LIST_CHARS   = 18;

static int _visibleRows() {
  int h = SCREEN_H - LIST_TOP - 10;
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

static void truncName(const char* src, char* dst, int maxChars) {
  int n = (int)strlen(src);
  if (n <= maxChars) { strcpy(dst, src); return; }
  int keep = maxChars - 3;
  if (keep < 1) keep = 1;
  memcpy(dst, src, keep);
  dst[keep]     = '.';
  dst[keep + 1] = '.';
  dst[keep + 2] = '.';
  dst[keep + 3] = '\0';
}

static void drawListHeader() {
  tft.fillRect(0, STATUSBAR_H, SCREEN_W, PH(28) - STATUSBAR_H, activeTheme->bg);
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor(PW(6), PH(18));
  tft.print(T(STR_MUSIC));

  int total = musicTrackCount();
  char counter[12];
  if (total == 0) snprintf(counter, sizeof(counter), "--");
  else            snprintf(counter, sizeof(counter), "%d/%d", musicIndex + 1, total);
  int counterW = (int)strlen(counter) * 6;
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(SCREEN_W - PW(6) - counterW, PH(18));
  tft.print(counter);

  tft.drawFastHLine(PW(6), PH(28), SCREEN_W - PW(12), activeTheme->accent);
}

static void drawList() {
  int total = musicTrackCount();
  int rows  = _visibleRows();
  int off   = _scrollOffset(musicIndex, total);

  tft.fillRect(0, LIST_TOP, SCREEN_W, rows * LIST_ROW_H, activeTheme->bg);

  if (total == 0) {
    const char* msg = musicSdOk() ? T(STR_NO_MP3) : T(STR_NO_SD);
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
      fg = activeTheme->onAccent;
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

// ── PLAYING VIEW ─────────────────────────────────────────────────────────────

static const int PV_TITLE_Y     = 30;
static const int PV_ARTIST_Y    = 50;
static const int PV_PROGRESS_X  = 12;
static const int PV_PROGRESS_W  = SCREEN_W - 24;
static const int PV_PROGRESS_Y  = 88;
static const int PV_PROGRESS_H  = 3;
static const int PV_TIME_Y      = PV_PROGRESS_Y + 8;
static const int PV_TRANSPORT_Y = 130;
static const int PV_TRANSPORT_R = 10;

static const int TEXT_MAX_CHARS = 20;  // size 1, ~128/6 = 21, marj icin 20
static const int MARQUEE_GAP    = 4;   // tekrar arasi bosluk char sayisi
static const unsigned long MARQUEE_INTERVAL_MS = 350;

static int _titleScroll  = 0;
static int _artistScroll = 0;
static unsigned long _lastScrollMs = 0;

static void resetMarqueeState() {
  _titleScroll  = 0;
  _artistScroll = 0;
  _lastScrollMs = millis();
}

// Sigmiyorsa scrollPos'ten baslayarak kayan bant goster, sigiyorsa ortala
static void drawScrollText(const char* text, int y, int maxChars,
                           uint16_t fg, int& scrollPos) {
  int n = (int)strlen(text);
  // Alani temizle
  tft.fillRect(0, y, SCREEN_W, 8, activeTheme->bg);
  tft.setTextSize(1);
  tft.setTextColor(fg, activeTheme->bg);

  if (n <= maxChars) {
    int w = n * 6;
    tft.setCursor((SCREEN_W - w) / 2, y);
    tft.print(text);
    return;
  }

  int wrapLen = n + MARQUEE_GAP;
  char window[TEXT_MAX_CHARS + 1];
  if (maxChars > TEXT_MAX_CHARS) maxChars = TEXT_MAX_CHARS;
  for (int i = 0; i < maxChars; i++) {
    int idx = (scrollPos + i) % wrapLen;
    window[i] = (idx < n) ? text[idx] : ' ';
  }
  window[maxChars] = '\0';
  tft.setCursor((SCREEN_W - maxChars * 6) / 2, y);
  tft.print(window);
}

static void advanceMarquee() {
  unsigned long now = millis();
  if (now - _lastScrollMs < MARQUEE_INTERVAL_MS) return;
  _lastScrollMs = now;

  const char* title  = musicTitle();
  const char* artist = musicArtist();
  int tlen = (int)strlen(title);
  int alen = (int)strlen(artist);

  if (tlen > TEXT_MAX_CHARS) {
    _titleScroll = (_titleScroll + 1) % (tlen + MARQUEE_GAP);
  } else {
    _titleScroll = 0;
  }
  if (alen > TEXT_MAX_CHARS) {
    _artistScroll = (_artistScroll + 1) % (alen + MARQUEE_GAP);
  } else {
    _artistScroll = 0;
  }
}

static void drawTitleArtist() {
  drawScrollText(musicTitle(), PV_TITLE_Y, TEXT_MAX_CHARS,
                 activeTheme->light, _titleScroll);

  const char* artist = musicArtist();
  if (artist && artist[0] != '\0') {
    drawScrollText(artist, PV_ARTIST_Y, TEXT_MAX_CHARS,
                   activeTheme->dim, _artistScroll);
  } else {
    tft.fillRect(0, PV_ARTIST_Y, SCREEN_W, 8, activeTheme->bg);
  }
}

static void drawProgressAndTime() {
  tft.fillRect(PV_PROGRESS_X, PV_PROGRESS_Y, PV_PROGRESS_W, PV_PROGRESS_H, activeTheme->dim);
  uint32_t total = musicTotalSec();
  uint32_t cur   = musicCurrentSec();
  int fillW = 0;
  if (total > 0 && cur <= total) {
    fillW = (int)((uint64_t)PV_PROGRESS_W * cur / total);
  }
  if (fillW > 0) {
    tft.fillRect(PV_PROGRESS_X, PV_PROGRESS_Y, fillW, PV_PROGRESS_H, activeTheme->accent);
  }

  char tcur[8], ttot[8];
  snprintf(tcur, sizeof(tcur), "%lu:%02lu", cur / 60, cur % 60);
  snprintf(ttot, sizeof(ttot), "%lu:%02lu", total / 60, total % 60);

  tft.fillRect(0, PV_TIME_Y, SCREEN_W, 8, activeTheme->bg);
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(PV_PROGRESS_X, PV_TIME_Y);
  tft.print(tcur);
  int tw = (int)strlen(ttot) * 6;
  tft.setCursor(SCREEN_W - PV_PROGRESS_X - tw, PV_TIME_Y);
  tft.print(ttot);
}

static void drawTransport() {
  int cx = SCREEN_W / 2;
  int gap = 28;
  uint16_t c = activeTheme->accent;

  drawPrevCircle(cx - gap, PV_TRANSPORT_Y, PV_TRANSPORT_R, c);
  if (musicIsPlaying()) {
    drawPauseCircle(cx, PV_TRANSPORT_Y, PV_TRANSPORT_R + 2, c);
  } else {
    drawPlayCircle(cx, PV_TRANSPORT_Y, PV_TRANSPORT_R + 2, c);
  }
  drawNextCircle(cx + gap, PV_TRANSPORT_Y, PV_TRANSPORT_R, c);
}

static void drawPlaying() {
  drawTitleArtist();
  drawProgressAndTime();
  drawTransport();
}

// ── DISPATCHER ───────────────────────────────────────────────────────────────

void drawMusicPage() {
  tft.fillScreen(activeTheme->bg);
  drawStatusBar();
  if (_view == MUSIC_VIEW_LIST) {
    drawListHeader();
    drawList();
  } else {
    resetMarqueeState();
    drawPlaying();
  }
}

void drawMusicPagePartial() {
  if (_view == MUSIC_VIEW_LIST) {
    drawListHeader();
    drawList();
  } else {
    advanceMarquee();
    drawTitleArtist();
    drawProgressAndTime();
    // Transport bandini temizle + tekrar ciz (play/pause toggle icin)
    tft.fillRect(0, PV_TRANSPORT_Y - PV_TRANSPORT_R - 4,
                 SCREEN_W, 2 * (PV_TRANSPORT_R + 4), activeTheme->bg);
    drawTransport();
  }
}
