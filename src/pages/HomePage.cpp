#include "HomePage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../display/StatusBar.h"
#include "../i18n/Strings.h"

static const StringId menuStringIds[] = {
  STR_PET,
  STR_MUSIC,
  STR_SETTINGS
};

static const int menuItemCount = sizeof(menuStringIds) / sizeof(menuStringIds[0]);

int getHomeMenuCount() {
  return menuItemCount;
}

void drawHomePage(int selectedIndex) {
  tft.fillScreen(activeTheme->bg);
  drawStatusBar();

  // Baslik
  tft.setTextColor(activeTheme->light);
  tft.setTextSize(1);
  const char* title = "tamaplayer";
  int titleW = (int)strlen(title) * 6;
  tft.setCursor((SCREEN_W - titleW) / 2, PH(18));
  tft.println(title);

  tft.setTextColor(activeTheme->accent);
  const char* sub = T(STR_HOME);
  int subW = (int)strlen(sub) * 6;
  tft.setCursor((SCREEN_W - subW) / 2, PH(34));
  tft.println(sub);

  tft.drawFastHLine(PW(10), PH(46), SCREEN_W - PW(20), activeTheme->accent);

  for (int i = 0; i < menuItemCount; i++) {
    int y = PH(57) + i * PH(22);
    const char* label = T(menuStringIds[i]);

    if (i == selectedIndex) {
      tft.fillRoundRect(PW(8), y - 3, SCREEN_W - PW(16), PH(16), 4, activeTheme->accent);
      tft.setTextColor(activeTheme->onAccent);
      tft.setCursor(PW(16), y);
      tft.print("> ");
      tft.println(label);
    } else {
      tft.setTextColor(activeTheme->dim);
      tft.setCursor(PW(24), y);
      tft.println(label);
    }
  }
}
