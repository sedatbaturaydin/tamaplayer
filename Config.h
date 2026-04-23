#pragma once

// TFT pinleri
#define TFT_CS    42
#define TFT_RST   40
#define TFT_DC    41
#define TFT_MOSI  39
#define TFT_SCLK  38

// Butonlar
#define BTN_UP     4
#define BTN_SELECT 7
#define BTN_DOWN   5
#define BTN_BACK   6

// ── Ekran yönü ───────────────────────────────────────────────────────────────
// 0 = dikey  (128×160)   
// 1 = yatay  (160×128)
// 2 = dikey ters
// 3 = yatay ters
#define SCREEN_ROTATION 2

#if (SCREEN_ROTATION % 2) == 0
  #define SCREEN_W 128
  #define SCREEN_H 160
#else
  #define SCREEN_W 160
  #define SCREEN_H 128
#endif

#define PH(y) ((y) * SCREEN_H / 160)
#define PW(x) ((x) * SCREEN_W / 128)

// Buton timing
#define DEBOUNCE_DELAY 50
#define LONG_PRESS_TIME 700

// SD kart (ikinci SPI bus - HSPI)
#define SD_SCK   12
#define SD_MISO  14
#define SD_MOSI  13
#define SD_CS    11

// MP3 klasoru
#define MP3_DIR     "/MP3"
#define MP3_MAX_TRACKS 32
#define MP3_NAME_MAX   32
