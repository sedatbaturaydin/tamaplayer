#include "StatusBar.h"
#include "DisplayManager.h"
#include "Theme.h"
#include "Icons.h"
#include "../music/MusicPlayer.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

void drawStatusBar() {
  tft.fillRect(0, 0, SCREEN_W, STATUSBAR_H, activeTheme->bg);

  // Sol: SD ikonu (sd mount ok ise)
  if (musicSdOk()) {
    drawSdIcon(4, 1, activeTheme->light);
  }

  // Orta: saat (millis bazli, taslak — gercek RTC degil)
  unsigned long minsTotal = millis() / 60000UL;
  unsigned int hh = (minsTotal / 60UL) % 24;
  unsigned int mm = minsTotal % 60UL;
  char buf[8];
  snprintf(buf, sizeof(buf), "%02u:%02u", hh, mm);
  int w = (int)strlen(buf) * 6;
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor((SCREEN_W - w) / 2, 3);
  tft.print(buf);

  // Sag: pil ikonu
  drawBatteryIcon(SCREEN_W - 16, 3, activeTheme->light);
}
