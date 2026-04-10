#include "HomePage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"

static const char* menuItems[] = {
  "pet",
  "music",
  "settings"
};

static const int menuItemCount = 3;

int getHomeMenuCount() {
  return menuItemCount;
}

void drawHomePage(int selectedIndex) {
  tft.fillScreen(activeTheme->bg);

  // Title
  tft.setTextColor(activeTheme->light);
  tft.setTextSize(1);
  tft.setCursor(34, 12);
  tft.println("tamaplayer");

  // Page breadcrumb
  tft.setTextColor(activeTheme->accent);
  tft.setCursor(52, 30);
  tft.println("home");

  tft.drawFastHLine(10, 43, 108, activeTheme->accent);

  for (int i = 0; i < menuItemCount; i++) {
    int y = 57 + (i * 22);

    if (i == selectedIndex) {
      tft.fillRoundRect(8, y - 3, 112, 16, 4, activeTheme->accent);
      tft.setTextColor(activeTheme->onAccent);
      tft.setCursor(16, y);
      tft.print("> ");
      tft.println(menuItems[i]);
    } else {
      tft.setTextColor(activeTheme->dim);
      tft.setCursor(24, y);
      tft.println(menuItems[i]);
    }
  }
}