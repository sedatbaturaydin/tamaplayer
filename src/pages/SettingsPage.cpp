#include "SettingsPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"

void drawSettingsPage(int selectedTheme) {
  tft.fillScreen(activeTheme->bg);

  // Başlık ("SETTINGS" = 48px)
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light);
  tft.setCursor((SCREEN_W - 48) / 2, PH(12));
  tft.println("SETTINGS");

  tft.drawFastHLine(PW(10), PH(24), SCREEN_W - PW(20), activeTheme->accent);

  // Alt başlık ("select theme:" = 78px)
  tft.setTextColor(activeTheme->dim);
  tft.setCursor((SCREEN_W - 78) / 2, PH(34));
  tft.println("select theme:");

  // Tema seçenekleri
  for (int i = 0; i < THEME_COUNT; i++) {
    int y = PH(50) + i * PH(26);
    bool isSelected = (i == selectedTheme);
    bool isActive   = (i == currentThemeIndex);

    if (isSelected) {
      tft.fillRoundRect(PW(8), y - 3, SCREEN_W - PW(16), PH(18), 4, activeTheme->accent);
      tft.setTextColor(activeTheme->onAccent);
      tft.setCursor(PW(14), y + 1);
      tft.print("> ");
      tft.print(THEMES[i].name);
      if (isActive) tft.print(" *");
    } else {
      tft.setTextColor(activeTheme->dim);
      tft.setCursor(PW(22), y + 1);
      tft.print(THEMES[i].name);
      if (isActive) {
        tft.setTextColor(activeTheme->accent);
        tft.print(" *");
      }
    }
  }

  // Yön ipuçları
  tft.setTextColor(activeTheme->dim);
  tft.setCursor(PW(20), PH(136));
  tft.println("up/down: cycle");
  tft.setCursor(PW(14), PH(148));
  tft.println("hold: apply+back");
}
