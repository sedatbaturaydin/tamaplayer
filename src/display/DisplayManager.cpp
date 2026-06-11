#include "DisplayManager.h"
#include "Theme.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
  SPI.begin(TFT_SCK, -1, TFT_MOSI);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(SCREEN_ROTATION);
  tft.fillScreen(activeTheme->bg);
}