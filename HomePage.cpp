#include "HomePage.h"
#include "DisplayManager.h"

static const char* menuItems[] = {
  "pet",
  "music",
  "status",
  "settings"
};

static const int menuItemCount = 4;

int getHomeMenuCount() {
  return menuItemCount;
}

void drawHomePage(int selectedIndex) {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(18, 10);
  tft.println("tamaplayer");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setCursor(22, 32);
  tft.println("home");

  tft.drawFastHLine(10, 45, 108, ST77XX_WHITE);

  for (int i = 0; i < menuItemCount; i++) {
    int y = 58 + (i * 20);

    if (i == selectedIndex) {
      tft.fillRoundRect(8, y - 2, 112, 14, 3, ST77XX_MAGENTA);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(14, y);
      tft.print(">");
      tft.setCursor(26, y);
      tft.println(menuItems[i]);
    } else {
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(26, y);
      tft.println(menuItems[i]);
    }
  }
}