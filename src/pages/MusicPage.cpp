#include "MusicPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../music/MusicPlayer.h"
#include <string.h>
#include <stdio.h>
#include <Arduino.h>

static int centerX(const char* text, uint8_t size) {
  return (SCREEN_W - (int)strlen(text) * 6 * size) / 2;
}

static void drawNote(int x, int y, uint16_t color) {
  tft.fillCircle(x,     y + 8, 3, color);
  tft.fillCircle(x + 9, y + 6, 3, color);
  tft.drawFastVLine(x + 3,  y,     9, color);
  tft.drawFastVLine(x + 12, y - 2, 9, color);
  tft.drawFastHLine(x + 3,  y,    10, color);
}

static void drawStatus() {
  int total = musicTrackCount();

  const char* statusStr;
  uint16_t    color;
  if (total == 0) {
    statusStr = "NO SD?";
    color     = activeTheme->dim;
  } else if (musicPlaying) {
    statusStr = "> PLAYING";
    color     = activeTheme->accent;
  } else {
    statusStr = "|| PAUSED";
    color     = activeTheme->dim;
  }

  int y = PH(100);
  tft.fillRect(0, y, SCREEN_W, 20, activeTheme->bg);
  tft.setTextSize(1);
  tft.setTextColor(color, activeTheme->bg);
  tft.setCursor(centerX(statusStr, 1), y);
  tft.print(statusStr);

  // DFPlayer'in bildirdigi gercek durum (debug)
  int st   = musicActualState();
  int file = musicActualFileNum();
  char buf[24];
  const char* stStr = (st == 1) ? "PLAY" : (st == 2) ? "PAUSE" : (st == 0) ? "STOP" : "?";
  snprintf(buf, sizeof(buf), "DFP: %s #%d", stStr, file);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(centerX(buf, 1), y + 10);
  tft.print(buf);
}

void drawMusicPage() {
  int total = musicTrackCount();
  tft.fillScreen(activeTheme->bg);

  // Ust baslik
  drawNote(PW(10), PH(4), activeTheme->accent);
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor(PW(28), PH(8));
  tft.print("MUSIC");
  tft.drawFastHLine(PW(6), PH(20), SCREEN_W - PW(12), activeTheme->accent);

  // Parca adi (buyuk)
  char trackStr[16];
  if (total == 0) {
    snprintf(trackStr, sizeof(trackStr), "--");
  } else {
    snprintf(trackStr, sizeof(trackStr), "Track %d", musicIndex + 1);
  }
  tft.setTextSize(2);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor(centerX(trackStr, 2), PH(50));
  tft.print(trackStr);

  // Counter
  char counter[12];
  if (total == 0) {
    snprintf(counter, sizeof(counter), "0 / 0");
  } else {
    snprintf(counter, sizeof(counter), "%d / %d", musicIndex + 1, total);
  }
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(centerX(counter, 1), PH(78));
  tft.print(counter);

  // Status
  drawStatus();

  // Alt bilgi
  tft.drawFastHLine(PW(6), PH(128), SCREEN_W - PW(12), activeTheme->dim);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor(PW(4),          PH(138)); tft.print("|< prev");
  tft.setCursor(SCREEN_W - PW(52), PH(138)); tft.print("next >|");
  tft.setCursor(centerX("[ ] play/pause", 1), PH(150));
  tft.print("[ ] play/pause");
}

void drawMusicPagePartial() {
  drawStatus();
}
