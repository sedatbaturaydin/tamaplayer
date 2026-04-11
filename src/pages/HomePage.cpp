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

  // Başlık ("tamaplayer" = 60px, "home" = 24px)
  tft.setTextColor(activeTheme->light);
  tft.setTextSize(1);
  tft.setCursor((SCREEN_W - 60) / 2, PH(12));
  tft.println("tamaplayer");

  tft.setTextColor(activeTheme->accent);
  tft.setCursor((SCREEN_W - 24) / 2, PH(30));
  tft.println("home");

  tft.drawFastHLine(PW(10), PH(43), SCREEN_W - PW(20), activeTheme->accent);

  for (int i = 0; i < menuItemCount; i++) {
    int y = PH(57) + i * PH(22);

    if (i == selectedIndex) {
      tft.fillRoundRect(PW(8), y - 3, SCREEN_W - PW(16), PH(16), 4, activeTheme->accent);
      tft.setTextColor(activeTheme->onAccent);
      tft.setCursor(PW(16), y);
      tft.print("> ");
      tft.println(menuItems[i]);
    } else {
      tft.setTextColor(activeTheme->dim);
      tft.setCursor(PW(24), y);
      tft.println(menuItems[i]);
    }
  }
}
