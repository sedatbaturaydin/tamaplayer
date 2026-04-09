#include "SettingsPage.h"
#include "DisplayManager.h"
#include "Theme.h"

void drawSettingsPage(int selectedTheme) {
  tft.fillScreen(activeTheme->bg);

  // Title
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light);
  tft.setCursor(40, 12);
  tft.println("SETTINGS");

  tft.drawFastHLine(10, 24, 108, activeTheme->accent);

  // Subtitle
  tft.setTextColor(activeTheme->dim);
  tft.setCursor(22, 30);
  tft.println("select theme:");

  // Theme options
  for (int i = 0; i < THEME_COUNT; i++) {
    int y = 46 + (i * 26);
    bool isSelected = (i == selectedTheme);
    bool isActive   = (i == currentThemeIndex);

    if (isSelected) {
      tft.fillRoundRect(8, y - 3, 112, 18, 4, activeTheme->accent);
      tft.setTextColor(activeTheme->onAccent);
      tft.setCursor(14, y + 1);
      tft.print("> ");
      tft.print(THEMES[i].name);
      if (isActive) tft.print(" *");
    } else {
      tft.setTextColor(activeTheme->dim);
      tft.setCursor(22, y + 1);
      tft.print(THEMES[i].name);
      if (isActive) {
        tft.setTextColor(activeTheme->accent);
        tft.print(" *");
      }
    }
  }

  // Hints
  tft.setTextColor(activeTheme->dim);
  tft.setCursor(20, 136);
  tft.println("press: cycle");
  tft.setCursor(14, 148);
  tft.println("hold: apply+back");
}
