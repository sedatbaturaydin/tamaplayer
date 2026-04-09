#include "Pages.h"
#include "DisplayManager.h"
#include "Theme.h"

static int centerX(const char* text, uint8_t textSize) {
  int px = strlen(text) * 6 * textSize;
  return (SCREEN_W - px) / 2;
}

void drawCenteredText(const char* title, const char* subtitle) {
  tft.fillScreen(activeTheme->bg);

  tft.setTextSize(2);
  tft.setTextColor(activeTheme->light);
  tft.setCursor(centerX(title, 2), 34);
  tft.println(title);

  tft.setTextSize(1);
  tft.setTextColor(activeTheme->dim);
  tft.setCursor(centerX(subtitle, 1), 72);
  tft.println(subtitle);

  tft.setCursor(centerX("hold: back", 1), 148);
  tft.println("hold: back");
}

void drawPage(AppState page) {
  switch (page) {
    case PAGE_MUSIC:
      drawCenteredText("MUSIC", "coming soon");
      break;

    default:
      tft.fillScreen(activeTheme->bg);
      break;
  }
}