#include "Pages.h"
#include "DisplayManager.h"

void drawCenteredText(const char* title, const char* subtitle, uint16_t bgColor) {
  tft.fillScreen(bgColor);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 30);
  tft.println(title);

  tft.setTextSize(1);
  tft.setCursor(18, 70);
  tft.println(subtitle);
}

void drawPage(AppState page) {
  switch (page) {
    case PAGE_PET:
      drawCenteredText("PET", "pet page placeholder", ST77XX_GREEN);
      break;

    case PAGE_MUSIC:
      drawCenteredText("MUSIC", "music page placeholder", ST77XX_BLUE);
      break;

    case PAGE_STATUS:
      drawCenteredText("STATUS", "status page placeholder", ST77XX_RED);
      break;

    case PAGE_SETTINGS:
      drawCenteredText("SETTINGS", "settings page placeholder", ST77XX_MAGENTA);
      break;

    default:
      tft.fillScreen(ST77XX_BLACK);
      break;
  }
}